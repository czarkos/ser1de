#!/usr/bin/env bash
# Run benchmark binaries in parallel with workers split across two NUMA nodes.
# Aggregated Gbps (sum per type) is printed after each round.
#
# Node 0: CPUs 0-31   — first -u workers use taskset -c (0 + index % 32)
# Node 1: CPUs 32-63 — next -v workers use taskset -c (32 + index % 32)
#
# Usage:
#   run_parallel_benchmark_numa.sh [-t TRIALS] [-r ROUNDS] [-b BINARY] -u N0 -v N1
#
#   -t TRIALS   Passed to each benchmark (repetitions per type). Default: 1
#   -r ROUNDS   Repeat the full parallel batch; aggregate printed each round. Default: 1
#   -b BINARY   Benchmark executable (default: ../build/test from project root)
#   -u N0       Number of parallel workers pinned to NUMA node 0 (CPUs 0-31)
#   -v N1       Number of parallel workers pinned to NUMA node 1 (CPUs 32-63)
#
# At least one of -u or -v must be positive; N0+N1 is the total worker count.
#
# Examples:
#   ./run_parallel_benchmark_numa.sh -u 8 -v 8 -t 1
#   ./run_parallel_benchmark_numa.sh -u 16 -v 0 -t 1 -b "$PWD/build/test"
#   ./run_parallel_benchmark_numa.sh -r 5 -u 4 -v 4 -t 1

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
DEFAULT_BINARY="${PROJECT_DIR}/build/test"

readonly NODE0_BASE=0
readonly NODE1_BASE=32
readonly CPUS_PER_NODE=32

TRIALS=1
ROUNDS=1
BINARY="${DEFAULT_BINARY}"
THREADS0=-1
THREADS1=-1

usage() {
  sed -n '2,28p' "$0" | sed 's/^# \{0,1\}//'
  exit "${1:-0}"
}

if ! command -v taskset >/dev/null 2>&1; then
  echo "Error: taskset not found (install util-linux)." >&2
  exit 1
fi

while [[ $# -gt 0 ]]; do
  case "$1" in
    -h|--help) usage 0 ;;
    -t) TRIALS="$2"; shift 2 ;;
    -r) ROUNDS="$2"; shift 2 ;;
    -b) BINARY="$2"; shift 2 ;;
    -u) THREADS0="$2"; shift 2 ;;
    -v) THREADS1="$2"; shift 2 ;;
    -*)
      echo "Unknown option: $1" >&2
      usage 1
      ;;
    *)
      echo "Unexpected argument: $1 (this script does not take BINARY list args; use -b)" >&2
      usage 1
      ;;
  esac
done

if [[ "${THREADS0}" -lt 0 || "${THREADS1}" -lt 0 ]]; then
  echo "Error: both -u and -v are required (use 0 for no workers on that node)." >&2
  usage 1
fi

if [[ "${THREADS0}" -eq 0 && "${THREADS1}" -eq 0 ]]; then
  echo "Error: need at least one worker (-u and/or -v must be positive)." >&2
  exit 1
fi

if [[ ! -x "${BINARY}" ]]; then
  echo "Error: benchmark not executable: ${BINARY}" >&2
  exit 1
fi

BINARIES=()
for ((i = 0; i < THREADS0; i++)); do
  BINARIES+=("${BINARY}")
done
for ((i = 0; i < THREADS1; i++)); do
  BINARIES+=("${BINARY}")
done

total_workers=${#BINARIES[@]}

aggregate_csv() {
  awk -v workers="$1" -F, '
    $1 == "type" { next }
    NF >= 3 && $1 != "" {
      type = $1
      sum[type] += ($3 + 0)
    }
    END {
      for (t in sum) idx[++c] = t
      for (i = 1; i <= c; i++) {
        for (j = i + 1; j <= c; j++) {
          if (idx[j] < idx[i]) { tmp = idx[i]; idx[i] = idx[j]; idx[j] = tmp }
        }
      }
      print "type\tworkers\tsum_gbps\tmean_gbps_per_worker"
      for (i = 1; i <= c; i++) {
        t = idx[i]
        s = sum[t]
        mean = (workers > 0) ? (s / workers) : 0
        printf "%s\t%d\t%.4f\t%.4f\n", t, workers, s, mean
      }
    }
  '
}

echo "NUMA-split parallel benchmark: ${THREADS0} worker(s) on node0 (CPUs ${NODE0_BASE}-$((NODE0_BASE + CPUS_PER_NODE - 1))), ${THREADS1} on node1 (CPUs ${NODE1_BASE}-$((NODE1_BASE + CPUS_PER_NODE - 1)))"
echo "Total workers: ${total_workers}, ${TRIALS} trial(s) each, ${ROUNDS} round(s)"
echo "Binary: ${BINARY}"
echo ""

TMPDIR_PARALLEL="$(mktemp -d "${TMPDIR:-/tmp}/parallel_bench_numa.XXXXXX")"
cleanup() { rm -rf "${TMPDIR_PARALLEL}"; }
trap cleanup EXIT

for ((round = 1; round <= ROUNDS; round++)); do
  echo "========== Round ${round}/${ROUNDS} =========="
  pids=()
  for i in "${!BINARIES[@]}"; do
    out="${TMPDIR_PARALLEL}/worker_${i}.csv"
    if [[ "${i}" -lt "${THREADS0}" ]]; then
      core=$((NODE0_BASE + (i % CPUS_PER_NODE)))
    else
      j=$((i - THREADS0))
      core=$((NODE1_BASE + (j % CPUS_PER_NODE)))
    fi
    taskset -c "${core}" "${BINARIES[$i]}" "${TRIALS}" > "${out}" 2>&1 &
    pids+=($!)
  done
  fail=0
  for pid in "${pids[@]}"; do
    if ! wait "${pid}"; then
      fail=1
    fi
  done
  if [[ "${fail}" -ne 0 ]]; then
    echo "One or more benchmark processes failed. Output:" >&2
    cat "${TMPDIR_PARALLEL}"/worker_*.csv >&2 || true
    exit 1
  fi

  cat "${TMPDIR_PARALLEL}"/worker_*.csv | aggregate_csv "${total_workers}"
  echo ""
done

echo "Done."
