// CPU utilization and instruction-cache benchmark for de/serialization.
//
// Runs ONE backend per process (selected on the command line):
//   ./bench proto    -> Protobuf SerializeToString / ParseFromString
//   ./bench ser1de   -> SERenaDE (ser1de) SerializeToString / ParseFromString
//
// For each backend it measures the serialize and deserialize phases separately,
// using a saturated (no rate limiting) loop over a fixed in-memory corpus of all
// ten message types. Hardware counters (perf_event_open) are fenced around ONLY
// the de/ser calls, so the numbers reflect the de/serialization work itself and
// the accelerator-offloaded portion of SERenaDE (IAA) naturally drops off the CPU
// counters.
//
// Reported per phase:
//   CPU utilization: wall/CPU time, CPU-time per op, CPUs utilized, cycles,
//                    cycles/op, instructions, IPC
//   Instruction cache: L1 i-cache read misses (+ per op), iTLB read misses (+/op)

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <google/protobuf/message.h>
#include <google/protobuf/util/message_differencer.h>

#include "Message0.pb.h"
#include "Message1.pb.h"
#include "Message2.pb.h"
#include "Message3.pb.h"
#include "Message4.pb.h"
#include "Message5.pb.h"
#include "Message6.pb.h"
#include "Message7.pb.h"
#include "Message8.pb.h"
#include "Message9.pb.h"
#include "access_message0.h"
#include "access_message1.h"
#include "access_message2.h"
#include "access_message3.h"
#include "access_message4.h"
#include "access_message5.h"
#include "access_message6.h"
#include "access_message7.h"
#include "access_message8.h"
#include "access_message9.h"

#include "ser1de.h"

#include "perf_counters.h"

namespace fp = fleetbench::proto;
using google::protobuf::Message;

static constexpr size_t kDefaultTotalOps = 200000;
static constexpr size_t kDefaultCorpusMessages = 2600;

// Builds a corpus that mirrors the message mix used by the existing
// fleetbench_throughput benchmark: 26 messages per batch spanning all ten
// generated proto types. `messages` holds populated sources; `deser_out` holds
// empty objects of the matching concrete type used as ParseFromString targets.
static void build_corpus(size_t target_messages, std::string& s,
                         std::vector<Message*>& messages,
                         std::vector<Message*>& deser_out) {
    auto add = [&](Message* src, Message* dst) {
        messages.push_back(src);
        deser_out.push_back(dst);
    };

    while (messages.size() < target_messages) {
        // Message0 x3
        { auto* m = new fp::Message0(); fp::Message0_Set_1(m, &s); add(m, new fp::Message0()); }
        { auto* m = new fp::Message0(); fp::Message0_Set_2(m, &s); add(m, new fp::Message0()); }
        { auto* m = new fp::Message0(); fp::Message0_Set_3(m, &s); add(m, new fp::Message0()); }
        // Message1 x3
        { auto* m = new fp::Message1(); fp::Message1_Set_1(m, &s); add(m, new fp::Message1()); }
        { auto* m = new fp::Message1(); fp::Message1_Set_2(m, &s); add(m, new fp::Message1()); }
        { auto* m = new fp::Message1(); fp::Message1_Set_3(m, &s); add(m, new fp::Message1()); }
        // Message2 x3
        { auto* m = new fp::Message2(); fp::Message2_Set_1(m, &s); add(m, new fp::Message2()); }
        { auto* m = new fp::Message2(); fp::Message2_Set_2(m, &s); add(m, new fp::Message2()); }
        { auto* m = new fp::Message2(); fp::Message2_Set_3(m, &s); add(m, new fp::Message2()); }
        // Message3 x3
        { auto* m = new fp::Message3(); fp::Message3_Set_1(m, &s); add(m, new fp::Message3()); }
        { auto* m = new fp::Message3(); fp::Message3_Set_2(m, &s); add(m, new fp::Message3()); }
        { auto* m = new fp::Message3(); fp::Message3_Set_3(m, &s); add(m, new fp::Message3()); }
        // Message4 x3
        { auto* m = new fp::Message4(); fp::Message4_Set_1(m, &s); add(m, new fp::Message4()); }
        { auto* m = new fp::Message4(); fp::Message4_Set_2(m, &s); add(m, new fp::Message4()); }
        { auto* m = new fp::Message4(); fp::Message4_Set_3(m, &s); add(m, new fp::Message4()); }
        // Message5 x1
        { auto* m = new fp::Message5(); fp::Message5_Set_1(m, &s); add(m, new fp::Message5()); }
        // Message6 x1
        { auto* m = new fp::Message6(); fp::Message6_Set_1(m, &s); add(m, new fp::Message6()); }
        // Message7 x4
        { auto* m = new fp::Message7(); fp::Message7_Set_1(m, &s); add(m, new fp::Message7()); }
        { auto* m = new fp::Message7(); fp::Message7_Set_2(m, &s); add(m, new fp::Message7()); }
        { auto* m = new fp::Message7(); fp::Message7_Set_3(m, &s); add(m, new fp::Message7()); }
        { auto* m = new fp::Message7(); fp::Message7_Set_4(m, &s); add(m, new fp::Message7()); }
        // Message8 x3
        { auto* m = new fp::Message8(); fp::Message8_Set_1(m, &s); add(m, new fp::Message8()); }
        { auto* m = new fp::Message8(); fp::Message8_Set_2(m, &s); add(m, new fp::Message8()); }
        { auto* m = new fp::Message8(); fp::Message8_Set_3(m, &s); add(m, new fp::Message8()); }
        // Message9 x2
        { auto* m = new fp::Message9(); fp::Message9_Set_1(m, &s); add(m, new fp::Message9()); }
        { auto* m = new fp::Message9(); fp::Message9_Set_2(m, &s); add(m, new fp::Message9()); }
    }
}

static std::string fmt_ll(long long v) {
    return v < 0 ? std::string("n/a") : std::to_string(v);
}

// Emits a human-readable block plus a single machine-parseable RESULT line that
// scripts/run.sh consumes to build the comparison table.
static void report(const char* backend, const char* phase, size_t ops,
                   const PerfCounters& pc) {
    const double wall = pc.wall_seconds();
    const double cpu = pc.cpu_seconds();
    const long long instr = pc.instructions();
    const long long cyc = pc.cycles();
    const long long l1i = pc.l1i_miss();
    const long long itlb = pc.itlb_miss();

    const double cpus = wall > 0 ? cpu / wall : 0.0;
    const double cpu_us_per_op = ops ? (cpu * 1e6) / ops : 0.0;
    const double ipc = (cyc > 0 && instr >= 0) ? static_cast<double>(instr) / cyc : -1.0;
    const double cyc_per_op = (cyc >= 0 && ops) ? static_cast<double>(cyc) / ops : -1.0;
    const double l1i_per_op = (l1i >= 0 && ops) ? static_cast<double>(l1i) / ops : -1.0;
    const double itlb_per_op = (itlb >= 0 && ops) ? static_cast<double>(itlb) / ops : -1.0;

    std::printf("\n[%s / %s]  ops=%zu\n", backend, phase, ops);
    std::printf("  CPU utilization:\n");
    std::printf("    wall_time      : %.3f ms\n", wall * 1e3);
    std::printf("    cpu_time       : %.3f ms  (CPUs utilized %.3f)\n", cpu * 1e3, cpus);
    std::printf("    cpu_time/op    : %.4f us\n", cpu_us_per_op);
    std::printf("    instructions   : %s\n", fmt_ll(instr).c_str());
    std::printf("    cycles         : %s", fmt_ll(cyc).c_str());
    if (cyc_per_op >= 0) std::printf("  (%.1f cycles/op)", cyc_per_op);
    std::printf("\n    IPC            : ");
    if (ipc >= 0) std::printf("%.3f\n", ipc); else std::printf("n/a\n");
    std::printf("  Instruction cache:\n");
    std::printf("    L1I read-miss  : %s", fmt_ll(l1i).c_str());
    if (l1i_per_op >= 0) std::printf("  (%.4f /op)", l1i_per_op);
    std::printf("\n    iTLB read-miss : %s", fmt_ll(itlb).c_str());
    if (itlb_per_op >= 0) std::printf("  (%.4f /op)", itlb_per_op);
    std::printf("\n");

    // Stable, parseable line (one per phase).
    std::printf(
        "RESULT backend=%s phase=%s ops=%zu wall_ms=%.3f cpu_ms=%.3f "
        "cpu_us_per_op=%.4f cpus=%.3f instructions=%lld cycles=%lld ipc=%.3f "
        "cyc_per_op=%.3f l1i_miss=%lld l1i_miss_per_op=%.4f itlb_miss=%lld "
        "itlb_miss_per_op=%.4f\n",
        backend, phase, ops, wall * 1e3, cpu * 1e3, cpu_us_per_op, cpus, instr,
        cyc, ipc, cyc_per_op, l1i, l1i_per_op, itlb, itlb_per_op);
    std::fflush(stdout);
}

static size_t warmup_ops(size_t n) { return n < 2000 ? n : 2000; }

// ---- Protobuf backend -------------------------------------------------------
static void run_proto(std::vector<Message*>& messages,
                      std::vector<Message*>& deser_out, size_t total_ops) {
    const size_t n = messages.size();
    std::vector<std::string> ser(n);

    // Serialize phase.
    for (size_t i = 0; i < warmup_ops(n); ++i)
        messages[i % n]->SerializeToString(&ser[i % n]);
    {
        PerfCounters pc;
        pc.start();
        for (size_t i = 0; i < total_ops; ++i)
            messages[i % n]->SerializeToString(&ser[i % n]);
        pc.stop();
        report("proto", "serialize", total_ops, pc);
    }

    // Guarantee every buffer is valid before deserialization (outside timing).
    for (size_t i = 0; i < n; ++i)
        messages[i]->SerializeToString(&ser[i]);

    // Deserialize phase.
    for (size_t i = 0; i < warmup_ops(n); ++i)
        deser_out[i % n]->ParseFromString(ser[i % n]);
    {
        PerfCounters pc;
        pc.start();
        for (size_t i = 0; i < total_ops; ++i)
            deser_out[i % n]->ParseFromString(ser[i % n]);
        pc.stop();
        report("proto", "deserialize", total_ops, pc);
    }
}

// ---- SERenaDE (ser1de) backend ---------------------------------------------
static void verify_ser1de(Ser1de& ser1de, std::vector<Message*>& messages) {
    google::protobuf::util::MessageDifferencer differ;
    const size_t check = messages.size() < 52 ? messages.size() : 52;
    bool ok = true;
    for (size_t i = 0; i < check && ok; ++i) {
        std::string buf;
        ser1de.SerializeToString(*messages[i], &buf);
        std::unique_ptr<Message> rt(messages[i]->New());
        ser1de.ParseFromString(buf, rt.get());
        ok = differ.Compare(*messages[i], *rt);
    }
    std::cout << "ser1de round-trip check: "
              << (ok ? "ALL CORRECT" : "ERROR: DATA MISMATCH") << std::endl;
}

static void run_ser1de(std::vector<Message*>& messages,
                       std::vector<Message*>& deser_out, size_t total_ops) {
    Ser1de ser1de;
    const size_t n = messages.size();
    std::vector<std::string> ser(n);

    verify_ser1de(ser1de, messages);

    // Serialize phase.
    for (size_t i = 0; i < warmup_ops(n); ++i)
        ser1de.SerializeToString(*messages[i % n], &ser[i % n]);
    {
        PerfCounters pc;
        pc.start();
        for (size_t i = 0; i < total_ops; ++i)
            ser1de.SerializeToString(*messages[i % n], &ser[i % n]);
        pc.stop();
        report("ser1de", "serialize", total_ops, pc);
    }

    // Guarantee every buffer is valid before deserialization (outside timing).
    for (size_t i = 0; i < n; ++i)
        ser1de.SerializeToString(*messages[i], &ser[i]);

    // Deserialize phase. Clear before each parse so reuse matches protobuf
    // ParseFromString semantics (which clears the target message internally).
    for (size_t i = 0; i < warmup_ops(n); ++i) {
        deser_out[i % n]->Clear();
        ser1de.ParseFromString(ser[i % n], deser_out[i % n]);
    }
    {
        PerfCounters pc;
        pc.start();
        for (size_t i = 0; i < total_ops; ++i) {
            deser_out[i % n]->Clear();
            ser1de.ParseFromString(ser[i % n], deser_out[i % n]);
        }
        pc.stop();
        report("ser1de", "deserialize", total_ops, pc);
    }
}

static void usage(const char* argv0) {
    std::cerr << "usage: " << argv0
              << " <proto|ser1de> [total_ops] [corpus_messages]\n"
              << "  total_ops        operations per phase (default "
              << kDefaultTotalOps << ")\n"
              << "  corpus_messages  distinct messages built (default "
              << kDefaultCorpusMessages << ")\n";
}

int main(int argc, char** argv) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc < 2) {
        usage(argv[0]);
        return 2;
    }
    const std::string backend = argv[1];
    if (backend != "proto" && backend != "ser1de") {
        usage(argv[0]);
        return 2;
    }
    const size_t total_ops =
        argc > 2 ? std::strtoull(argv[2], nullptr, 10) : kDefaultTotalOps;
    const size_t corpus_messages =
        argc > 3 ? std::strtoull(argv[3], nullptr, 10) : kDefaultCorpusMessages;

    std::string s(1 << 20, 'a');
    std::vector<Message*> messages;
    std::vector<Message*> deser_out;
    build_corpus(corpus_messages, s, messages, deser_out);

    PerfCounters probe;
    std::cout << "==============================================================="
              << std::endl;
    std::cout << " backend=" << backend << "  corpus=" << messages.size()
              << " messages  total_ops/phase=" << total_ops << std::endl;
    std::cout << " hardware counters: "
              << (probe.hardware_available()
                      ? "available"
                      : "UNAVAILABLE (values will be n/a)")
              << std::endl;
    std::cout << "==============================================================="
              << std::endl;

    if (backend == "proto")
        run_proto(messages, deser_out, total_ops);
    else
        run_ser1de(messages, deser_out, total_ops);

    for (Message* m : messages) delete m;
    for (Message* m : deser_out) delete m;
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
