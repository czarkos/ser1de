# Protobuf characterization bucket breakdown

This document explains what each bucket in `proto_profile_summary.txt` means, how
symbols are assigned in `scripts/categorize_symbols.py`, and how the aggregate
buckets relate to a Ser1de comparison.

The benchmark profiles **`SerializeToString` / `ParseFromString`** on Fleetbench
messages at **`-O3`**, using **one unique object per operation** (same pattern
as `fleetbench_latency` / `fleetbench_throughput`), so first-parse field
allocation is included in the timed work.

---

## How assignment works

`categorize_symbols.py` reads a flat `perf report` (`perf report --stdio
--no-children -g none`) and assigns each symbol's **self CPU%** to a bucket using
**regex patterns, first match wins**:

1. `codec_transform`
2. `buffer_mgmt`
3. `allocation`
4. `object_field`
5. `other` (catch-all)

Two **aggregate buckets** are computed on top for Ser1de comparison:

- **`wire_path`** — native Protobuf wire-format work that Ser1de replaces
- **`populate_object`** — field allocation and non-wire object work Ser1de still pays

This is **sampling + regex classification**, not exact accounting. Some symbols
are mis-bucketed; some work is fused inside a single function and cannot be split.

At **`-O3`**, codec primitives in libprotobuf tend to stay **out-of-line** as
distinct symbols, which is what makes the split possible. The benchmark
intentionally matches the optimization level used by the fleetbench Ser1de
evaluations.

---

## The five primary buckets

### 1. `codec_transform` — pure wire math

**Definition:** The **value ↔ wire transform only**: varint/zigzag/tag
encode and decode, fixed-width byte assembly, length-delimited framing, and
string UTF-8 validation. Explicitly **excludes** buffer copy, heap allocation,
and reading/writing field values in the message object.

**Typical serialize symbols:**

| Pattern / symbol | Role |
|------------------|------|
| `VarintSize`, `TagSize`, `Int32Size`, … | Compute encoded size of fields |
| `WriteVarint`, `WriteTag`, `ZigZagEncode`, `MakeTag` | Emit varints and tags |
| `WriteFixed32`, `WriteLittleEndian`, … | Fixed-width encode |
| `InternalWriteMessage`, `WriteLengthDelimited` | Length-delimited submessage framing |

**Typical deserialize symbols:**

| Pattern / symbol | Role |
|------------------|------|
| `ShiftMixParseVarint`, `ParseVarint`, `ReadVarint` | Decode varints |
| `ReadTag`, `ReadTagFallback`, `ExpectTag` | Read field tags |
| `ReadSize`, `ZigZagDecode`, `ValidateEnum` | Length / zigzag / enum decode |

**UTF-8 sub-share:** String validation (`utf8_range_*`, `VerifyUtf8String`,
`SkipAscii`, …) is counted inside `codec_transform` but also reported
separately as **codec without UTF-8** for users who do not treat validation as
part of the codec.

**What this is NOT:**

- `memcpy`, `ReadString` → `buffer_mgmt`
- `malloc`, `_M_create` → `allocation`
- `TcParser::Fast*` (often contains `ParseContext` in the symbol name) → usually `buffer_mgmt`

**Plain language:** *The arithmetic and bit manipulation of the Protobuf wire format.*

**Typical share (current profile):** ~11–14% of sampled profile.

---

### 2. `buffer_mgmt` — stream navigation and byte movement

**Definition:** Moving encoded bytes in/out of buffers plus buffer bookkeeping:
input/output streams, flush/ensure-space/refill, and raw memory copies.

**Includes:**

| Category | Examples |
|----------|----------|
| Input/output streams | `EpsCopyInputStream`, `EpsCopyOutputStream`, `ParseContext` |
| Stream bookkeeping | `EnsureSpace`, `Flush`, `Next`, `BackUp`, `Skip`, `Refresh`, `BackUp` |
| Raw writes | `WriteRaw`, `WriteStringMaybeAliased`, `WriteAliasedRaw` |
| libc copies | `memcpy`, `memmove`, `memset`, `__memcpy`, … |
| Unresolved libc (heuristic) | Raw addresses in `libc.so.6` → classified as buffer |

**Important nuance:** Many **`TcParser`** handlers appear in this bucket because
their mangled names contain **`ParseContext`**, which matches the buffer regex
before the object regex is checked. A significant part of the parse loop is
therefore counted as buffer, not object or codec.

**On deserialize:** Walk the serialized blob, read length-delimited chunks, copy
string/byte payloads into field storage (`ReadString`, …).

**On serialize:** Write encoded bytes into the growing output string, ensure
output space, flush stream chunks.

**Plain language:** *Read/write bytes in the serialized stream and copy payloads — includes much of the wire parsing control flow.*

**Typical share:** ~42–47% of sampled profile (usually the **largest** bucket).

---

### 3. `allocation` — heap and container growth

**Definition:** Obtaining or growing memory: heap allocators, `std::string`
capacity growth, repeated-field backing-store extension, arena allocation.

**Includes:**

| Category | Examples |
|----------|----------|
| Heap | `malloc`, `free`, `cfree`, `operator new`, `operator delete` |
| String growth | `basic_string`, `_M_create`, `_M_append`, `_M_replace`, `_M_realloc` |
| Repeated-field growth | `InternalExtend`, `RepeatedPtrField` + `Reserve` / `AddAllocated` |
| Arena | `google::protobuf::Arena`, `ArenaImpl`, `AllocateAligned` |

**Typical deserialize triggers (first-parse workload):**

- Grow `std::string` field buffers
- Extend `RepeatedPtrField` arrays
- Allocate nested submessages

**What this is NOT:**

- Container metadata (`using_sso`, `allocated_size`) → usually `object_field`
- Copying bytes into already-allocated storage → `buffer_mgmt`

**Plain language:** *Pay the memory allocator to get space for fields.*

**Typical share:** ~17–18% of sampled profile.

---

### 4. `object_field` — message object structure

**Definition:** Iteration, dispatch, and metadata on the in-memory C++ message
object: generated parse/serialize glue, has-bits, repeated-field container logic,
cached sizes, lifecycle — excluding pure wire math, stream I/O, and explicit
allocators (when those match allocation patterns first).

**Includes:**

| Category | Examples |
|----------|----------|
| Generated code | `::_InternalParse`, `::_InternalSerialize`, `::ByteSizeLong` |
| Fast parser tables | `TcParser::FindFieldEntry`, `TcFieldData`, `fast_entry`, `MiniParse` |
| Field presence | `SetHas`, `HasBit` |
| Repeated containers | `RepeatedPtrFieldBase::using_sso`, `allocated_size`, `ClearNonEmpty`, `Destroy`, `Add` |
| Metadata | `CachedSize`, `GetCachedSize`, `InternalMetadata`, `UnknownFieldSet` |
| Lifecycle | `Clear`, `New`, ctors/dtors, `MergeFrom`, `CopyFrom` |

**Split within this bucket:**

- **Wire-path slice** (counted again inside aggregate `wire_path`): symbols
  matching `WIRE_OBJECT` patterns — `_InternalParse`, `TcParser*`, `ByteSizeLong`, …
- **Populate slice** (part of aggregate `populate_object`): container bookkeeping,
  `Clear`, cached sizes, non-wire helpers

**Plain language:** *Manage the shape of the C++ object — which field, has-bits, repeated-field containers.*

**Typical share:** ~15–18% of sampled profile.

---

### 5. `other` — unclassified

**Definition:** Catch-all for symbols that did not match any of the four primary
regex lists.

**Common contents:**

| Category | Examples | Notes |
|----------|----------|-------|
| Unresolved addresses | `0xffffffffaba01320` in `[unknown]` | ~5%; likely hot libc `memcpy`; heuristic rolls into `wire_path` |
| String internals | `TaggedStringPtr::as_int`, `GetIfAllocated`, … | Protobuf SSO / tagged pointer helpers |
| Abseil | `string_view`, `log_internal::Check_*` | Often from protobuf debug checks |
| Runtime | `__popcountdi2`, atomics, `std::min`, `std::exchange` | Compiler / STL helpers |
| Misc | `MessageLite::GetArena` | Small metadata |

**Plain language:** *Helpers, unresolved symbols, and patterns not yet in the regex lists.*

**Typical share:** ~9–10% of sampled profile (~5% is the single unresolved address).

`other` is not a meaningful cost phase — it means *we could not label this confidently*.

---

## Aggregate buckets (Ser1de comparison)

### 6. `wire_path` — what Ser1de replaces

**Formula:**

```
wire_path
  = codec_transform
  + buffer_mgmt
  + object_field symbols matching WIRE_OBJECT
  + unresolved [unknown] raw addresses (heuristic)
```

**`WIRE_OBJECT` patterns include:** `::_InternalParse`, `::_InternalSerialize`,
`::ByteSizeLong`, `TcParser`, `TcParseTable`, `TcFieldData`, `SetHas`, `HasBit`,
`SerializeWithCachedSizes`, `SerializeToArray`, `ReadPackedVarint`, …

**Conceptual content:** The full **native Protobuf wire-format path** — wire
math, stream traversal and payload copy, and wire-driven parse/serialize dispatch.

**Ser1de replaces this with:** gather + compress (serialize), or decompress +
scatter + header (deserialize). No wire tags/varints, no `TcParser` loop, no
`ParseContext`.

**Typical share:** ~**71%** of sampled profile (serialize and deserialize).

---

### 7. `populate_object` — what Ser1de still pays

**Formula:**

```
populate_object
  = allocation
  + object_field NOT matching WIRE_OBJECT
```

**Conceptual content:**

- All **field allocation** (~18%): malloc, string growth, repeated-field extend
- **Non-wire object work** (~6%): container checks, `Clear`, cached sizes, string SSO metadata

**Ser1de analogue:** `generate_scatter_ptrs_and_allocate_from_sizes` plus
`ScatterWithMemCpy` — pre-size field storage and copy flat bytes into object slots.

**Typical share:** ~**24%** of sampled profile.

---

## Deserialize pipeline (bucket map)

```
Serialized bytes (std::string)
        │
        ▼
┌─────────────────────────────────────────┐
│  wire_path (~71%)                       │
│  ┌──────────────┐  ┌──────────────────┐ │
│  │ codec (~12%) │  │ buffer (~46%)    │ │
│  │ varint/tag   │  │ ParseContext,    │ │
│  │ UTF-8 check  │  │ ReadString,      │ │
│  │              │  │ memcpy, TcParser │ │
│  └──────────────┘  └──────────────────┘ │
│  + wire dispatch from object (few %)    │
└─────────────────────────────────────────┘
        │
        ▼
┌─────────────────────────────────────────┐
│  populate_object (~24%)                 │
│  ┌──────────────┐  ┌──────────────────┐ │
│  │ alloc (~18%) │  │ object (~6%)     │ │
│  │ malloc,      │  │ container meta,  │ │
│  │ string grow, │  │ Clear, cached    │ │
│  │ Extend       │  │ size, SSO        │ │
│  └──────────────┘  └──────────────────┘ │
└─────────────────────────────────────────┘
        │
        ▼
   remaining other (~5%)
```

---

## How the summary tables relate

| View | Columns | Sums to | Best for |
|------|---------|---------|----------|
| Five buckets | codec, buffer, alloc, object, other | ~100% of profile | Detailed libprotobuf anatomy |
| Ser1de proxy | wire_path, populate, remaining other | 100% of profile | Comparing against Ser1de |
| Classified work | codec, buffer, alloc, object only | 100% excluding other | “Pure codec % of known work” |

**Do not sum `codec` and `codec excl utf8`** in the five-bucket summary — the
latter is a subset of the former, not an additional bucket.

---

## Example numbers (reference run)

From a representative `proto_profile_summary.txt` (10k ops, first-parse workload):

| Bucket | Serialize | Deserialize |
|--------|-----------|-------------|
| codec | 14.4% | 11.5% |
| buffer | 41.6% | 46.2% |
| allocation | 17.0% | 17.8% |
| object | 17.6% | 14.4% |
| other | 9.4% | 10.2% |
| **wire_path** | **72.0%** | **70.9%** |
| **populate_object** | **24.4%** | **24.2%** |

---

## Caveats

1. **First-match regex** — e.g. `ParseContext` in a symbol forces `buffer_mgmt` even when the function also dispatches fields.
2. **Symbols ≠ logical phases** — one function may combine codec, buffer, and object work; perf attributes time to the whole symbol.
3. **`-O3` profile** — matches fleetbench; an `-O0` profile would show different paths and ratios.
4. **`wire_path` vs wall-clock** — ~71% CPU on the wire path corresponds roughly to ~63% deserialize latency savings for Ser1de in fleetbench; the gap is Ser1de's own pipeline cost (decompress, gather, header, schema) plus sampling vs timing differences.
5. **Tune regexes** — the report lists top `other` symbols so patterns in `categorize_symbols.py` can be improved over time.

---

## Related files

| File | Role |
|------|------|
| `scripts/categorize_symbols.py` | Bucket regex definitions and report generation |
| `scripts/profile_proto.sh` | Produces `serialize.report.txt`, `deserialize.report.txt` |
| `proto_profile_summary.txt` | Latest bucketed output |
| `proto_breakdown.txt` | Method B cycle-based split (coarser, exact counters) |
| `README.md` | How to build and run the benchmark |
