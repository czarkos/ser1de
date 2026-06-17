# proto_charac

Standalone characterization of **Protobuf alone**: what fraction of
`SerializeToString` / `ParseFromString` CPU is **PURE de/encoding logic** - the
value<->wire transformation math only - versus buffer management, allocation, and
message-object/field access.

"Pure de/encoding" here is the strict definition: **only the value<->wire
transform** (varint/zigzag/tag/length encode+decode, fixed-width byte assembly,
length-delimited framing, and string UTF-8 validation). It explicitly **excludes**
the byte copy in/out of the buffer, buffer bookkeeping, allocation, and reading or
writing field values into the message object.

## Workload (matches fleetbench latency/throughput)

Each timed operation uses **one unique message** at index `i` — no modulo reuse.
Message shells are pre-allocated before timing (as in the fleetbench benchmarks);
**first-parse field allocation** and **first SerializeToString into an empty
output string** are included in the measured loops.

Measured two complementary ways:

- **Method A - sampling profiler** (`proto_run` under `perf`): the primary method.
  It samples each phase in isolation (pinned to one core), then
  `categorize_symbols.py` sorts symbols into five buckets and reports
  `codec_transform` = the **pure de/encoding %**. This works because at `-O3` the
  codec primitives stay out-of-line in libprotobuf (a separately built static
  lib), so `CodedOutputStream::WriteVarint*`, `WireFormatLite::Int*Size`,
  `ShiftMixParseVarint`, etc. are attributable as distinct symbols and can be
  separated from the buffer/stream and object/field symbols.

- **Method B - in-process instrumentation** (`proto_breakdown`): a coarser but
  exact cross-check fenced with hardware counters (cycles).
  - Serialize is split into three tight loops: `ByteSizeLong` (size), the
    cached-size write `SerializeWithCachedSizesToArray` (encode), and output
    `std::string` allocation.
  - Deserialize is decomposed by parse mode: `first-parse` (once each into a
    pre-allocated empty object, field alloc included), `fresh-heap` (adds
    `New()`/`delete()` per op), `fresh-arena` (arena shell alloc). The shell
    allocation share is `(fresh_heap - first_parse) / fresh_heap`; the arena
    mode shows how much shell overhead an arena removes.
  - Note: Method B cannot peel the buffer copy off the transform (they are one
    fused instruction stream in `SerializeWithCachedSizesToArray`), so the strict
    pure-codec number comes from Method A; Method B bounds the allocation share.

## Bucket definitions (isolating pure de/encoding)

The profiler categorizer (`scripts/categorize_symbols.py`) sorts every sampled
symbol into five buckets (first match wins, in this order):

- **codec_transform** (the headline **PURE de/encoding**): value<->wire transform
  math only - varint/tag/length size computation, varint/tag/zigzag encode &
  decode (`WriteVarint*`, `WriteTag*`, `ShiftMixParseVarint`, `ReadSize*`,
  `MakeTag`, `ValidateEnum`), fixed-width writes and endian/unaligned byte
  assembly, length-delimited submessage framing (`InternalWriteMessage`), and
  string-field UTF-8 validation (`utf8_range_*`, `VerifyUtf8String`).
- **buffer_mgmt**: moving encoded bytes in/out of buffers + buffer bookkeeping -
  `EpsCopyOutputStream` / `EpsCopyInputStream`, `CodedStream` flush/ensure-space/
  refill, `ParseContext` boundary handling, raw/string buffer writes, `memcpy`/
  `memmove` (incl. the unresolved libc copy routine, attributed by heuristic).
- **allocation**: `operator new`/`delete`, `malloc`/`free`, tcmalloc,
  `std::string` growth, `Arena`, repeated-field growth.
- **object_field**: message-object iteration / field read+write / dispatch /
  metadata - generated `_InternalSerialize` / `_InternalParse` / `ByteSizeLong`,
  accessors (`set_`/`mutable_`/`add_`, `RepeatedField`), `TcParser` table
  dispatch, `New`/`Clear`/ctor/dtor, has-bits, unknown fields, cached sizes.
- **other**: unresolved addresses, runtime/abseil helpers, loop, everything else.

Aggregate bucket (for Ser1de comparison):

- **wire_path**: the native Protobuf wire-format path Ser1de replaces —
  `codec_transform` + `buffer_mgmt` + wire-driven parse/serialize dispatch
  from `object_field` (`TcParser`, `_InternalParse`/`_InternalSerialize`,
  `ByteSizeLong`, `SetHas`, …). Unresolved `[unknown]` raw addresses (~5%)
  are heuristically included (likely wire `memcpy`). The complement
  **`populate_object`** = `allocation` + remaining `object_field` approximates
  work Ser1de still pays (field allocation, container metadata, scatter targets).

The report prints `codec_transform` both as a share of the whole profile and as a
share of *classified* work, and breaks out the string UTF-8 validation sub-share
(`codec without UTF-8`) so it can be subtracted if you don't consider string
validation part of the codec.

Caveats: this is a regex-based approximation. The few varint bytes a write
primitive stores live inside the codec symbol and cannot be peeled off from the
transform that produced them, and sampling has noise. The summary prints the top
unclassified symbols so the regexes (in `scripts/categorize_symbols.py`) can be
tuned; with the current rules the unclassified ("other") bucket is only a few
percent.

## Prerequisites

- `PROTO_PATH` - protobuf checkout (auto-derived from the repo layout if unset,
  so `sudo` works without `-E`).
- The generated proto code (`Message0..9.pb.*`) and `access_message*.h` are
  referenced read-only from the sibling `../fleetbench_throughput` directory.
- `perf` for Method A: `/usr/bin/perf` is broken for this kernel, so the script
  locates a real binary under `/usr/lib/linux-tools/*/perf`. Method A samples
  `cycles:u` (user space), so it works at `perf_event_paranoid=2`; running under
  `sudo` gives the cleanest results. Method B needs no perf tool (uses
  `perf_event_open` directly) and no IAA - this is pure Protobuf.

## Run

```bash
cd /home/christos/dev/final_results/ser1de/evaluation/proto_charac
bash scripts/characterize_proto.sh            # default: 10001 ops, core 0
sudo bash scripts/characterize_proto.sh       # cleanest perf sampling
bash scripts/characterize_proto.sh 10001 3     # custom ops, core
```

Default ops is 10001 (same scale as `fleetbench_latency`). Larger values (e.g.
150000) match throughput benchmarks but use substantially more memory because
each op gets its own message pair.

Outputs (all inside this directory):

- `proto_breakdown.txt` - Method B exact split (serialize + deserialize)
- `proto_profile_summary.txt` - Method A bucketed profiler split
- `serialize.report.txt`, `deserialize.report.txt` - raw flat perf reports
- `serialize.perf.data`, `deserialize.perf.data` - raw perf samples

## Manual use

```bash
cmake -S . -B build && cmake --build build -j
./build/proto_breakdown 10001                 # instrumentation only
bash scripts/profile_proto.sh 10001 0         # profiler only
python3 scripts/categorize_symbols.py serialize=serialize.report.txt deserialize=deserialize.report.txt
```

## Related documentation

- [`BUCKET_BREAKDOWN.md`](BUCKET_BREAKDOWN.md) — verbose explanation of profiler buckets
  (`codec`, `buffer`, `allocation`, `object`, `other`, `wire_path`, `populate_object`)

## Self-contained / deletable

Everything lives inside this directory (including `build/`, reports, and
`perf.data`). Nothing outside it is created or modified - including the sibling
`cpu_util_icache` and `fleetbench_throughput` benchmarks. `perf_counters.h` is
copied in for independence. Delete the directory to remove the benchmark with no
side effects.
