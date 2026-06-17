#!/usr/bin/env python3
"""Measure the PURE de/encoding fraction of a Protobuf perf profile.

Reads one or more flat perf reports (produced with
`perf report --stdio --no-children -g none`) and assigns the self CPU% of each
symbol to one of five buckets:

  codec_transform  - PURE de/encoding logic: the value<->wire transformation
                     math only (varint/zigzag/tag/length encode+decode, fixed
                     width byte assembly, length-delimited framing, string UTF-8
                     validation). NO buffer copy, NO allocation, NO field access.
  buffer_mgmt      - moving encoded bytes in/out of buffers + buffer bookkeeping
                     (EpsCopy*Stream, CodedStream flush/ensure-space, memcpy).
  allocation       - new/delete/malloc/free/tcmalloc, std::string growth, arena,
                     repeated-field growth.
  object_field     - message object iteration / field read+write / dispatch /
                     metadata (generated _InternalSerialize/_InternalParse/
                     ByteSizeLong, accessors, TcParser dispatch, unknown fields).
  other            - unresolved / runtime / loop / everything else.

Aggregate bucket (computed from the above, for Ser1de comparison):

  wire_path        - the native Protobuf wire-format path that Ser1de replaces:
                     codec_transform + buffer_mgmt + wire-driven parse/serialize
                     dispatch (TcParser, _InternalParse/_InternalSerialize,
                     ByteSizeLong, SetHas, etc.). Unresolved [unknown] raw
                     addresses are heuristically included (likely libc memcpy
                     from wire reads). The complement (allocation + remaining
                     object_field + other) approximates work Ser1de still pays
                     (field allocation, container metadata, scatter targets).

The headline number is `codec_transform` = the pure de/encoding percentage.

At -O3 the codec primitives stay out-of-line in libprotobuf (a separately built
static lib), so they are attributable as distinct symbols; this is what makes
the split possible. It remains a regex-based approximation: the few varint bytes
that a write primitive stores live inside the codec symbol and cannot be peeled
off from the transform that produced them. The script prints the top
unclassified symbols so the buckets can be validated and tuned below.

Usage: categorize_symbols.py LABEL=report.txt [LABEL=report.txt ...]
"""

import re
import sys

# Bucket definitions, checked in this order; first match wins.

# 1. PURE de/encoding: value<->wire transformation math only.
CODEC_TRANSFORM = [
    # varint / tag / length size computation (encode-side math)
    r"VarintSize", r"TagSize", r"\bInt32Size\b", r"\bInt64Size\b",
    r"UInt32Size", r"UInt64Size", r"SInt32Size", r"SInt64Size",
    r"\bEnumSize\b", r"LengthDelimitedSize", r"\bMessageSize\b",
    # varint / tag write
    r"WriteVarint", r"WriteTag", r"ZigZagEncode", r"MakeTag",
    r"WriteLittleEndian", r"WritePrimitive",
    r"WriteFloat", r"WriteDouble", r"WriteFixed", r"WriteSFixed",
    # varint / tag read (decode-side math)
    r"ShiftMixParseVarint", r"VarintParse", r"ParseVarint", r"ReadVarint",
    r"\bReadTag\b", r"\bReadTagFallback\b", r"ReadSize", r"ExpectTag",
    r"ZigZagDecode", r"DecodeVarint", r"ReadLittleEndian", r"ValidateEnum",
    # length-delimited framing of submessages (tag + length emission)
    r"InternalWriteMessage", r"WriteLengthDelimited",
    # fixed-width / byte assembly transform
    r"EndianHelper", r"UnalignedLoad", r"UnalignedStore",
    r"little_endian", r"\bToHost\b", r"\bFromHost\b",
    # string field UTF-8 validation (intrinsic to string codec; called out
    # separately in the report so it can be subtracted if undesired)
    r"utf8", r"VerifyUtf8String", r"IsStructurallyValid", r"ValidateUTF8",
    r"SkipAscii",
]

# 2. buffer management: byte copy in/out + buffer bookkeeping.
BUFFER_MGMT = [
    r"EpsCopyOutputStream", r"EpsCopyInputStream",
    r"WriteRaw", r"WriteAliasedRaw", r"WriteStringMaybeAliased", r"WriteCord",
    r"::Flush", r"EnsureSpace", r"::Trim\b", r"::Refresh", r"::Next\b",
    r"\bSkip\b", r"GetDirectBufferPointer", r"ArrayOutputStream",
    r"ArrayInputStream", r"BackUp", r"ParseContext",
    r"\bmemcpy\b", r"\bmemmove\b", r"\bmemset\b", r"__memmove", r"__memcpy",
    r"__memset",
]

# 3. allocation.
ALLOCATION = [
    r"\boperator new\b", r"\boperator delete\b",
    r"\bmalloc\b", r"\bfree\b", r"\bcfree\b", r"\brealloc\b", r"\bcalloc\b",
    r"\b_Znwm\b", r"\b_ZdlPv\b", r"\b_Znam\b", r"\b_ZdaPv\b",
    r"tcmalloc", r"TCMalloc", r"je_malloc",
    r"basic_string", r"_M_create", r"_M_construct", r"_M_append",
    r"_M_mutate", r"_M_replace", r"_M_realloc", r"_M_assign", r"_M_dispose",
    r"google::protobuf::Arena", r"AllocateAligned", r"ArenaImpl",
    r"AllocateMaybeAligned", r"RepeatedPtrField.*(Extend|Reserve|AddAllocated)",
    r"InternalExtend",
]

# 4. message object iteration / field access / dispatch / metadata.
OBJECT_FIELD = [
    r"::_InternalSerialize", r"::_InternalParse", r"::ByteSizeLong",
    r"::Clear\b", r"::New\b", r"MergeFrom", r"CopyFrom", r"MergeImpl",
    r"SharedCtor", r"SharedDtor", r"InternalSwap", r"Construct", r"Destruct",
    r"Destroy", r"~", r"DestructorSkippable",
    r"RepeatedField<", r"RepeatedPtrField", r"\bset_", r"\bmutable_",
    r"\badd_", r"_impl_",
    r"TcParser", r"TcParseTable", r"TcFieldData", r"fast_entry", r"MiniParse",
    r"FastFieldEntry",
    r"MaybeComputeUnknownFieldsSize", r"InternalMetadata", r"have_unknown_fields",
    r"unknown_fields", r"UnknownFieldSet", r"CachedSize", r"GetCachedSize",
    r"SetHas", r"HasBit",
]

# Wire-format parse/serialize dispatch (subset of object_field rolled into wire_path).
WIRE_OBJECT = [
    r"::_InternalSerialize", r"::_InternalParse", r"::ByteSizeLong",
    r"TcParser", r"TcParseTable", r"TcFieldData", r"fast_entry", r"MiniParse",
    r"FastFieldEntry",
    r"SetHas", r"HasBit",
    r"MaybeComputeUnknownFieldsSize",
    r"SerializeWithCachedSizes", r"SerializeToArray", r"MergeFromImpl",
    r"ReadPackedVarint",
]

BUCKETS = [
    ("codec_transform", CODEC_TRANSFORM),
    ("buffer_mgmt", BUFFER_MGMT),
    ("allocation", ALLOCATION),
    ("object_field", OBJECT_FIELD),
]

UTF8_RE = re.compile(r"utf8|VerifyUtf8String|IsStructurallyValid|ValidateUTF8|SkipAscii")
RAW_ADDR_RE = re.compile(r"^0x[0-9a-fA-F]+$")

# perf flat line: "  NN.NN%  command  dso  [.] symbol"
LINE_RE = re.compile(r"^\s*(\d+\.\d+)%\s+(\S+)\s+(\S+)\s+\[[^\]]*\]\s+(.*)$")


def classify(symbol, dso):
    for name, patterns in BUCKETS:
        for pat in patterns:
            if re.search(pat, symbol):
                return name
    # Heuristic: unresolved raw address in libc is almost certainly the
    # memcpy/memmove implementation for this workload -> buffer management.
    if RAW_ADDR_RE.match(symbol) and "libc" in dso:
        return "buffer_mgmt"
    return "other"


def is_wire_object(symbol):
    for pat in WIRE_OBJECT:
        if re.search(pat, symbol):
            return True
    return False


def wire_path_contribution(pct, dso, sym, bucket):
    """Share of profile attributed to the native Protobuf wire-format path."""
    if bucket in ("codec_transform", "buffer_mgmt"):
        return pct, False
    if bucket == "object_field" and is_wire_object(sym):
        return pct, False
    # Unresolved addresses (often memcpy from wire reads) not caught above.
    if bucket == "other" and RAW_ADDR_RE.match(sym) and "unknown" in dso.lower():
        return pct, True
    return 0.0, False


def parse_report(path):
    rows = []
    with open(path) as f:
        for line in f:
            if line.lstrip().startswith("#") or not line.strip():
                continue
            m = LINE_RE.match(line)
            if not m:
                continue
            rows.append((float(m.group(1)), m.group(3), m.group(4).strip()))
    return rows


def summarize(label, path):
    rows = parse_report(path)
    buckets = {name: 0.0 for name, _ in BUCKETS}
    buckets["other"] = 0.0
    wire_path = 0.0
    wire_heuristic = 0.0
    utf8 = 0.0
    other_syms = []
    total = 0.0
    for pct, dso, sym in rows:
        total += pct
        b = classify(sym, dso)
        buckets[b] += pct
        wp, heuristic = wire_path_contribution(pct, dso, sym, b)
        wire_path += wp
        if heuristic:
            wire_heuristic += wp
        if b == "codec_transform" and UTF8_RE.search(sym):
            utf8 += pct
        if b == "other":
            other_syms.append((pct, sym))

    classified = sum(buckets[n] for n, _ in BUCKETS)
    codec = buckets["codec_transform"]
    populate = buckets["allocation"] + buckets["object_field"]
    # object_field symbols in wire_path were counted in wire_path, not populate.
    wire_object_in_object = 0.0
    for pct, dso, sym in rows:
        if classify(sym, dso) == "object_field" and is_wire_object(sym):
            wire_object_in_object += pct
    populate -= wire_object_in_object
    remaining_other = buckets["other"] - wire_heuristic

    print("=" * 72)
    print(f" {label.upper()}  (samples cover {total:.1f}% of profile)")
    print("=" * 72)
    for name in ("codec_transform", "buffer_mgmt", "allocation", "object_field",
                 "other"):
        tag = "  <== PURE de/encoding" if name == "codec_transform" else ""
        print(f"    {name:<16}: {buckets[name]:6.2f}% of profile{tag}")
    if codec > 0:
        print(f"      (of which string UTF-8 validation: {utf8:.2f}% of profile;"
              f" codec without UTF-8 = {codec - utf8:.2f}%)")
    if classified > 0:
        print(f"    --> PURE de/encoding = {100*codec/classified:.1f}% of"
              f" CLASSIFIED work ({100*(codec-utf8)/classified:.1f}% excluding"
              f" UTF-8 validation)")
    if total > 0:
        print(f"    --> wire_path (Ser1de replaces) = {100*wire_path/total:.1f}%"
              f" of profile", end="")
        if wire_heuristic > 0:
            print(f"  (incl. {100*wire_heuristic/total:.1f}% unresolved-addr"
                  f" heuristic)", end="")
        print()
        print(f"    --> populate_object (Ser1de still pays)  ="
              f" {100*populate/total:.1f}% of profile"
              f"  (alloc + non-wire object)")
        print(f"    --> remaining other                      ="
              f" {100*remaining_other/total:.1f}% of profile")
    other_syms.sort(reverse=True)
    if other_syms:
        print("    top unclassified symbols (tune regexes in this script):")
        for pct, sym in other_syms[:10]:
            short = sym if len(sym) <= 78 else sym[:75] + "..."
            print(f"        {pct:6.2f}%  {short}")
    print()
    return (label, buckets, utf8, classified, wire_path, populate,
            remaining_other, wire_heuristic, total)


def main():
    args = sys.argv[1:]
    if not args:
        print(__doc__)
        sys.exit(2)
    results = []
    for a in args:
        if "=" not in a:
            print(f"skipping malformed arg (need LABEL=path): {a}", file=sys.stderr)
            continue
        label, path = a.split("=", 1)
        results.append(summarize(label, path))

    print("=" * 72)
    print(" SUMMARY: bucket shares (% of sampled profile; columns sum to ~100%)")
    print("=" * 72)
    print(f"  {'operation':<14}{'codec':>9}{'buffer':>9}{'alloc':>9}"
          f"{'object':>9}{'other':>9}  {'codec excl utf8':>16}")
    print("  " + "-" * 70)
    for row in results:
        label, buckets, utf8, classified = row[0], row[1], row[2], row[3]
        total = sum(buckets.values())
        if total <= 0:
            print(f"  {label:<14}{'n/a':>9}")
            continue
        c = 100 * buckets["codec_transform"] / total
        b = 100 * buckets["buffer_mgmt"] / total
        a = 100 * buckets["allocation"] / total
        o = 100 * buckets["object_field"] / total
        x = 100 * buckets["other"] / total
        cnu = 100 * (buckets["codec_transform"] - utf8) / total
        print(f"  {label:<14}{c:>8.1f}%{b:>8.1f}%{a:>8.1f}%{o:>8.1f}%"
              f"{x:>8.1f}%  {cnu:>14.1f}%")
    print("\n  Five buckets sum to 100% of the sampled profile above.")
    print("  'codec excl utf8' is a subset of 'codec' (not an extra bucket).")
    print("  Per-operation detail above also shows '% of CLASSIFIED work'")
    print("  (codec+buffer+alloc+object only, excluding 'other').")

    print("\n" + "=" * 72)
    print(" SUMMARY: wire_path vs populate_object (Ser1de comparison proxy)")
    print("=" * 72)
    print(f"  {'operation':<14}{'wire_path':>12}{'populate':>12}{'other':>10}")
    print("  " + "-" * 50)
    for row in results:
        label = row[0]
        wire_path, populate, remaining_other, total = row[4], row[5], row[6], row[8]
        if total <= 0:
            print(f"  {label:<14}{'n/a':>12}")
            continue
        w = 100 * wire_path / total
        p = 100 * populate / total
        r = 100 * remaining_other / total
        print(f"  {label:<14}{w:>11.1f}%{p:>11.1f}%{r:>9.1f}%")
    print("\n  wire_path = codec + buffer + wire parse/serialize dispatch")
    print("  (TcParser, _InternalParse/_InternalSerialize, ByteSizeLong, ...).")
    print("  Includes unresolved [unknown] addresses (likely wire memcpy).")
    print("  populate = allocation + object_field not in wire_path.")
    print("  Ser1de replaces wire_path with decompress + gather/scatter + header.")


if __name__ == "__main__":
    main()
