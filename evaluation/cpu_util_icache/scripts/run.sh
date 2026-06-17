#!/usr/bin/env bash
# Single entry point: build the benchmark, run both backends as separate
# processes (pinned to a single core), and print a side-by-side
# Protobuf-vs-SERenaDE comparison of CPU utilization and instruction-cache
# impact for serialize and deserialize.
#
# Usage:
#   bash scripts/run.sh [total_ops] [corpus_messages]
#   sudo bash scripts/run.sh           # works without -E (env auto-derived below)
#   CORE=3 bash scripts/run.sh         # pin to a different core (default 0)
#
# Everything stays inside this directory (build/, results_*.txt, summary.txt).
# Delete the directory to remove all trace of this benchmark.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "${ROOT_DIR}"

TOTAL_OPS="${1:-200000}"
CORPUS="${2:-2600}"
CORE="${CORE:-0}"

# --- Environment -------------------------------------------------------------
# `sudo` strips these unless invoked with -E, so default them from the known
# repo layout (<ser1de_root>/evaluation/cpu_util_icache) when unset. Values
# already present in the environment are respected.
SER1DE_ROOT="$(cd "${ROOT_DIR}/../.." && pwd)"
export PROTO_PATH="${PROTO_PATH:-${SER1DE_ROOT}/protobuf}"
export QPL_PATH="${QPL_PATH:-${SER1DE_ROOT}/qpl_install_dir}"
export SER1DE_LIB_PATH="${SER1DE_LIB_PATH:-${SER1DE_ROOT}/ser1de}"

err=0
[ -d "${PROTO_PATH}/src" ]            || { echo "ERROR: PROTO_PATH invalid (no src/): ${PROTO_PATH}" >&2; err=1; }
[ -d "${QPL_PATH}/include" ]          || { echo "ERROR: QPL_PATH invalid (no include/): ${QPL_PATH}" >&2; err=1; }
[ -f "${SER1DE_LIB_PATH}/ser1de.h" ]  || { echo "ERROR: SER1DE_LIB_PATH invalid (no ser1de.h): ${SER1DE_LIB_PATH}" >&2; err=1; }
[ "${err}" -eq 0 ] || { echo "Fix the paths above (or export the variables) and re-run." >&2; exit 1; }
echo ">>> PROTO_PATH=${PROTO_PATH}"
echo ">>> QPL_PATH=${QPL_PATH}"
echo ">>> SER1DE_LIB_PATH=${SER1DE_LIB_PATH}"

# --- Core pinning ------------------------------------------------------------
PIN=()
if command -v taskset >/dev/null 2>&1; then
    PIN=(taskset -c "${CORE}")
    echo ">>> Pinning benchmark to core ${CORE}"
else
    echo ">>> WARNING: taskset not found; running WITHOUT core pinning" >&2
fi

# --- Build -------------------------------------------------------------------
echo ">>> Configuring + building (build/) ..."
cmake -S . -B build >/dev/null
cmake --build build -j >/dev/null
echo ">>> Build OK: ${ROOT_DIR}/build/bench"

# --- Run both backends as separate processes ---------------------------------
echo ">>> Running Protobuf run ..."
"${PIN[@]}" ./build/bench proto  "${TOTAL_OPS}" "${CORPUS}" | tee results_proto.txt
echo ">>> Running SERenaDE (ser1de) run ..."
"${PIN[@]}" ./build/bench ser1de "${TOTAL_OPS}" "${CORPUS}" | tee results_ser1de.txt

# --- Side-by-side comparison summary -----------------------------------------
echo ">>> Building comparison summary ..."
awk '
/^RESULT / {
    delete kv
    for (i = 2; i <= NF; i++) { split($i, p, "="); kv[p[1]] = p[2] }
    key = kv["backend"] "|" kv["phase"]
    cpu[key]       = kv["cpu_us_per_op"]
    cycph[key]     = kv["cyc_per_op"]
    ipc[key]       = kv["ipc"]
    l1i[key]       = kv["l1i_miss_per_op"]
    itlb[key]      = kv["itlb_miss_per_op"]
    seen[kv["phase"]] = 1
}
function ratio(s, p,   a, b) {
    a = s["ser1de|" p]; b = s["proto|" p]
    if (b + 0 == 0) return "n/a"
    return sprintf("%.3f", a / b)
}
function row(label, s, p,   a, b) {
    a = s["proto|" p]; b = s["ser1de|" p]
    printf "  %-22s %14s %14s %12s\n", label, a, b, ratio(s, p)
}
END {
    printf "\n=====================================================================\n"
    printf " CPU + instruction-cache comparison  (ser1de/proto ratio: <1 = ser1de better)\n"
    printf "=====================================================================\n"
    for (ph = 1; ph <= 2; ph++) {
        phase = (ph == 1) ? "serialize" : "deserialize"
        if (!(phase in seen)) continue
        printf "\n[%s]\n", phase
        printf "  %-22s %14s %14s %12s\n", "metric", "protobuf", "serenade", "ratio"
        printf "  ---------------------------------------------------------------\n"
        row("cpu_time_us/op",  cpu,   phase)
        row("cycles/op",       cycph, phase)
        row("IPC",             ipc,   phase)
        row("L1I_miss/op",     l1i,   phase)
        row("iTLB_miss/op",    itlb,  phase)
    }
    printf "\n"
}
' results_proto.txt results_ser1de.txt | tee summary.txt

echo ">>> Done. Raw output: results_proto.txt, results_ser1de.txt | Summary: summary.txt"
