#!/usr/bin/env bash
# Method A: sample the proto-only runner under perf, one phase at a time,
# pinned to a single core, and emit a flat per-symbol CPU% report for each.
#
# Usage: bash scripts/profile_proto.sh [ops] [core]
# Produces (inside the benchmark dir): serialize.perf.data, deserialize.perf.data,
#          serialize.report.txt, deserialize.report.txt
#
# Notes:
# - /usr/bin/perf is a wrapper that is broken for this kernel; we locate a real
#   perf binary under /usr/lib/linux-tools/*/perf instead.
# - We sample the user-space cycles event (cycles:u) so it works at the default
#   perf_event_paranoid=2 without root. Run under sudo for the cleanest results.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "${ROOT_DIR}"

OPS="${1:-10001}"
CORE="${2:-${CORE:-0}}"
FREQ="${FREQ:-4000}"

find_perf() {
    local cand found=""
    for cand in /usr/lib/linux-tools/*/perf /usr/lib/linux-tools-*/perf; do
        [ -x "${cand}" ] || continue
        "${cand}" --version >/dev/null 2>&1 && found="${cand}"
    done
    if [ -z "${found}" ] && command -v perf >/dev/null 2>&1 && perf --version >/dev/null 2>&1; then
        found="$(command -v perf)"
    fi
    [ -n "${found}" ] && { echo "${found}"; return 0; }
    return 1
}

PERF="$(find_perf)" || { echo "ERROR: no working perf binary found." >&2; exit 1; }
echo ">>> Using perf: ${PERF} ($(${PERF} --version 2>/dev/null))"
[ -x ./build/proto_run ] || { echo "ERROR: ./build/proto_run not built (run characterize_proto.sh)." >&2; exit 1; }

PIN=()
command -v taskset >/dev/null 2>&1 && PIN=(taskset -c "${CORE}")
[ "${#PIN[@]}" -gt 0 ] && echo ">>> Pinning to core ${CORE}" || echo ">>> WARNING: taskset missing; no pinning" >&2

record_phase() {
    local phase="$1" data="$2"
    echo ">>> perf record [${phase}] ops=${OPS} ..."
    "${PERF}" record -e cycles:u -F "${FREQ}" -o "${data}" -- \
        "${PIN[@]}" ./build/proto_run "${OPS}" "${phase}"
}

report_phase() {
    local data="$1" out="$2"
    "${PERF}" report --stdio -i "${data}" --no-children -g none --percent-limit 0 \
        > "${out}" 2>/dev/null
    echo ">>> wrote ${out}"
}

record_phase serialize   serialize.perf.data
record_phase deserialize deserialize.perf.data
report_phase serialize.perf.data   serialize.report.txt
report_phase deserialize.perf.data deserialize.report.txt
