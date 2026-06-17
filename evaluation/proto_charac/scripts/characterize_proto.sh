#!/usr/bin/env bash
# Single entry point: build, then characterize Protobuf de/serialization and
# report what fraction is PURE de/encoding logic (the value<->wire transform
# math only: varint/zigzag/tag/length encode+decode, fixed-width byte assembly,
# length-delimited framing, string UTF-8 validation) versus buffer management,
# allocation, and message-object/field access.
#
# Workload: one unique Fleetbench message per operation (same as fleetbench
# latency/throughput), so first-parse field allocation is included.
#
#   Method A (profiler)       : perf sampling of each phase; symbols bucketed
#                               into codec_transform / buffer_mgmt / allocation
#                               / object_field / other. codec_transform is the
#                               headline "pure de/encoding %".
#   Method B (instrumentation): exact serialize size/encode/alloc split and a
#                               deserialize first-parse/heap/arena decomposition
#                               (coarser, but uses precise in-process counters).
#
# Usage:
#   bash scripts/characterize_proto.sh [ops] [core]
#   sudo bash scripts/characterize_proto.sh        # cleanest perf results; pinned to core 0
#
# Everything stays inside this directory (build/, *.txt, *.perf.data).
# Delete the directory to remove all trace of this benchmark.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "${ROOT_DIR}"

OPS="${1:-10001}"
CORE="${2:-${CORE:-0}}"

# --- Environment (sudo strips these; default from the repo layout) -----------
SER1DE_ROOT="$(cd "${ROOT_DIR}/../.." && pwd)"
export PROTO_PATH="${PROTO_PATH:-${SER1DE_ROOT}/protobuf}"
[ -d "${PROTO_PATH}/src" ] || { echo "ERROR: PROTO_PATH invalid (no src/): ${PROTO_PATH}" >&2; exit 1; }
echo ">>> PROTO_PATH=${PROTO_PATH}"

# --- Build -------------------------------------------------------------------
echo ">>> Configuring + building (build/) ..."
cmake -S . -B build >/dev/null
cmake --build build -j >/dev/null
echo ">>> Build OK: proto_run, proto_breakdown"

# --- Core pinning prefix -----------------------------------------------------
PIN=()
command -v taskset >/dev/null 2>&1 && PIN=(taskset -c "${CORE}")
[ "${#PIN[@]}" -gt 0 ] && echo ">>> Pinning instrumentation to core ${CORE}"

# --- Method B: in-process instrumentation ------------------------------------
echo
echo "############### METHOD B: instrumentation ###############"
"${PIN[@]}" ./build/proto_breakdown "${OPS}" | tee proto_breakdown.txt

# --- Method A: sampling profiler + symbol bucketing --------------------------
echo
echo "############### METHOD A: profiler ###############"
prof_ok=1
bash scripts/profile_proto.sh "${OPS}" "${CORE}" || prof_ok=0

if [ "${prof_ok}" -eq 1 ] && [ -f serialize.report.txt ] && [ -f deserialize.report.txt ]; then
    PY="$(command -v python3 || command -v python || true)"
    if [ -n "${PY}" ]; then
        "${PY}" scripts/categorize_symbols.py \
            serialize=serialize.report.txt \
            deserialize=deserialize.report.txt | tee proto_profile_summary.txt
    else
        echo ">>> python not found; raw perf reports are in serialize.report.txt / deserialize.report.txt" >&2
    fi
else
    echo ">>> Profiler step did not complete (perf unavailable?). Instrumentation results above still stand." >&2
fi

echo
echo ">>> Done."
echo "    PURE de/encoding %   : see 'codec_transform' / SUMMARY in proto_profile_summary.txt"
echo "    Method A (profiler)  : proto_profile_summary.txt + serialize.report.txt / deserialize.report.txt"
echo "    Method B (exact)     : proto_breakdown.txt"
