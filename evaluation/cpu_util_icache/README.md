# cpu_util_icache

Standalone benchmark measuring the **CPU utilization** and **instruction-cache
impact** of de/serialization, comparing two backends:

- **Protobuf** (traditional software serialization)
- **SERenaDE / ser1de** (custom serialization with hardware-offloaded compression
  on Intel IAA)

Each backend is run as its **own process** (`./build/bench proto` and
`./build/bench ser1de`). For each run, the serialize and deserialize phases are
measured separately with a saturated (no rate-limiting) loop over a fixed corpus
of all ten message types.

## What it measures

Hardware counters are read via the `perf_event_open` syscall and fenced around
**only** the de/serialization calls, so the numbers attribute work to the de/ser
code (and SERenaDE's IAA-offloaded compression correctly drops off the CPU
counters since it runs off-core).

- CPU utilization: wall/CPU time, CPU-time per op, CPUs utilized, cycles,
  cycles/op, instructions, IPC
- Instruction cache: L1 i-cache read misses (+ per op), iTLB read misses (+ per op)

Counters are opened user-space only (`exclude_kernel`), so they work under the
default `perf_event_paranoid=2` without root. If a counter is unsupported it is
reported as `n/a` rather than aborting.

## Prerequisites

These environment variables must be set (same ones the project already uses):

- `PROTO_PATH` – protobuf checkout (provides `protoc`, headers, abseil, libs)
- `QPL_PATH` – Intel QPL install dir (IAA compression)
- `SER1DE_LIB_PATH` – directory containing `ser1de.h`

The generated proto code (`Message0..9.pb.*`) and `access_message*.h` are
referenced read-only from the sibling `../fleetbench_throughput` directory.

## Run

```bash
cd /home/christos/dev/final_results/ser1de/evaluation/cpu_util_icache
bash scripts/run.sh                 # default: 200000 ops/phase, 2600-message corpus
bash scripts/run.sh 500000          # custom ops per phase
bash scripts/run.sh 500000 5200     # custom ops and corpus size
```

Outputs (all inside this directory):

- `results_proto.txt`, `results_ser1de.txt` – full per-run reports
- `summary.txt` – side-by-side comparison table

## Manual build / run

```bash
cmake -S . -B build && cmake --build build -j
./build/bench proto
./build/bench ser1de
```

## Self-contained / deletable

Everything lives inside this directory (including `build/`). Nothing outside it
is created or modified. Delete the directory to remove the benchmark entirely
with no side effects.
