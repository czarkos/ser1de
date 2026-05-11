#!/usr/bin/env bash
# Run one or more benchmark binaries in parallel and print aggregated Gbps
# (sum of per-process Gbps per benchmark type) after each round.
# Each worker is pinned with taskset to one of the first 32 CPUs (0-31);
# worker index i uses CPU (i % 32).
#
# Usage:
#   run_parallel_benchmark.sh [-t TRIALS] [-r ROUNDS] [-b BINARY] [-n COPIES] [--] [BINARY ...]
#
#   -t TRIALS   Argument passed to each benchmark (repetitions per type). Default: 1
#   -r ROUNDS   Repeat the full parallel batch this many times; aggregate printed each round. Default: 1
#   -b BINARY   Path to benchmark executable when using -n (default: ../build/test from project root)
#   -n COPIES   Run COPIES parallel instances of -b BINARY (do not combine with extra BINARY args)
#   BINARY ...  If given (and -n not used), run each listed binary once in parallel (different builds)
#
# Examples:
#   ./run_parallel_benchmark.sh -n 8 -t 1
#   ./run_parallel_benchmark.sh -t 3 ./build/test ./other_build/test
#   ./run_parallel_benchmark.sh -r 5 -n 4 -t 1 -b "$PWD/build/test"

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
DEFAULT_BINARY="${PROJECT_DIR}/build/test"
readonly MAX_CPU_INDEX=31
TRIALS=1
ROUNDS=1
COPIES=0
BINARY="${DEFAULT_BINARY}"
BINARIES=()

usage() {
  sed -n '2,25p' "$0" | sed 's/^# \{0,1\}//'
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
    -n) COPIES="$2"; shift 2 ;;
    --) shift; break ;;
    -*)
      echo "Unknown option: $1" >&2
      usage 1
      ;;
    *)
      BINARIES+=("$1")
      shift
      ;;
  esac
done
while [[ $# -gt 0 ]]; do
  BINARIES+=("$1")
  shift
done

if [[ "${COPIES}" -gt 0 && ${#BINARIES[@]} -gt 0 ]]; then
  echo "Error: use either -n COPIES with -b, or list BINARY paths, not both." >&2
  exit 1
fi

if [[ "${COPIES}" -gt 0 ]]; then
  if [[ ! -x "${BINARY}" ]]; then
    echo "Error: benchmark not executable: ${BINARY}" >&2
    exit 1
  fi
  BINARIES=()
  for ((i = 1; i <= COPIES; i++)); do
    BINARIES+=("${BINARY}")
  done
elif [[ ${#BINARIES[@]} -eq 0 ]]; then
  BINARIES=("${DEFAULT_BINARY}")
fi

for b in "${BINARIES[@]}"; do
  if [[ ! -x "$b" ]]; then
    echo "Error: not executable: $b" >&2
    exit 1
  fi
done

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

echo "Parallel benchmark: ${#BINARIES[@]} worker(s), ${TRIALS} trial(s) each, ${ROUNDS} round(s)"
echo "CPU affinity: each worker runs as taskset -c N with N = worker_index % $((MAX_CPU_INDEX + 1)) (CPUs 0-${MAX_CPU_INDEX})"
echo "Binaries:"
for i in "${!BINARIES[@]}"; do
  core=$((i % (MAX_CPU_INDEX + 1)))
  echo "  [$i] CPU ${core}: ${BINARIES[$i]}"
done
echo ""

TMPDIR_PARALLEL="$(mktemp -d "${TMPDIR:-/tmp}/parallel_bench.XXXXXX")"
cleanup() { rm -rf "${TMPDIR_PARALLEL}"; }
trap cleanup EXIT

for ((round = 1; round <= ROUNDS; round++)); do
  echo "========== Round ${round}/${ROUNDS} =========="
  pids=()
  for i in "${!BINARIES[@]}"; do
    out="${TMPDIR_PARALLEL}/worker_${i}.csv"
    core=$((i % (MAX_CPU_INDEX + 1)))
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

  cat "${TMPDIR_PARALLEL}"/worker_*.csv | aggregate_csv "${#BINARIES[@]}"
  echo ""
done

echo "Done."
