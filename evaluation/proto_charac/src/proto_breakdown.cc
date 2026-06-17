// Method B: in-process instrumentation that splits Protobuf serialize and
// deserialize into the two coarse buckets requested:
//   - encode/decode work  (wire-format codec + size computation)
//   - memory + object setup/teardown  (allocation / object construction)
//
// SERIALIZE is cleanly separable into three sub-steps, each measured in its own
// tight loop fenced with hardware counters (cycles) + wall time:
//   1. size pass   : ByteSizeLong()                       -> encode/decode
//   2. encode pass : SerializeWithCachedSizesToArray()    -> encode/decode
//      (pure cached-size write; does NOT recompute ByteSizeLong)
//   3. alloc pass  : fresh std::string resize(sz)         -> memory
//
// DESERIALIZE interleaves decode and object building, so it is decomposed by
// comparing three parse modes (one unique object per op, matching fleetbench):
//   - first-parse : ParseFromString once each into a pre-allocated empty object
//                   (field allocation included; message shell allocated outside
//                   the timed loop, same as fleetbench_latency/throughput)
//   - fresh-heap  : New() + parse + delete each op        (adds shell alloc)
//   - fresh-arena : arena New() + parse + arena reset     (arena shell alloc)
// memory(allocation) share  ~ (fresh_heap - first_parse) / fresh_heap
// encode/decode share       ~ first_parse / fresh_heap
// The arena mode shows how much of the shell allocation overhead an arena removes.
//
//   ./proto_breakdown <ops>
// Defaults: ops=10001.

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include <google/protobuf/arena.h>
#include <google/protobuf/message.h>

#include "corpus.h"
#include "perf_counters.h"

using google::protobuf::Arena;
using google::protobuf::Message;

static constexpr size_t kDefaultOps = 10001;

struct PassResult {
    double wall = 0.0;
    long long cycles = -1;
    long long instructions = -1;
};

// Runs `fn` once for warmup, then measures it under hardware counters.
template <typename Fn>
static PassResult measure(Fn&& fn) {
    fn();  // warmup (cache/branch-predictor)
    PerfCounters pc;
    pc.start();
    fn();
    pc.stop();
    return {pc.wall_seconds(), pc.cycles(), pc.instructions()};
}

static double cyc_per_op(const PassResult& r, size_t ops) {
    return (r.cycles >= 0 && ops) ? static_cast<double>(r.cycles) / ops : -1.0;
}

static void bucket_table(const char* op, double encode, double memory) {
    const double total = encode + memory;
    const double ep = total > 0 ? 100.0 * encode / total : 0.0;
    const double mp = total > 0 ? 100.0 * memory / total : 0.0;
    std::printf("\n  ==> %s coarse split (by cycles/op):\n", op);
    std::printf("        encode/decode work        : %6.2f %%\n", ep);
    std::printf("        memory + object setup     : %6.2f %%\n", mp);
    std::printf("RESULT op=%s encode_decode_pct=%.2f memory_object_pct=%.2f\n",
                op, ep, mp);
}

int main(int argc, char** argv) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    const size_t ops = argc > 1 ? std::strtoull(argv[1], nullptr, 10) : kDefaultOps;

    std::string s(1 << 20, 'a');
    std::vector<Message*> messages;
    std::vector<Message*> deser_out;
    proto_charac::build_corpus(ops, s, messages, deser_out);
    const size_t n = messages.size();

    // Prime + size the encode scratch buffer (max serialized size in corpus).
    size_t max_size = 0;
    for (size_t i = 0; i < n; ++i)
        max_size = std::max(max_size, messages[i]->ByteSizeLong());
    std::vector<uint8_t> buf(max_size + 16);

    // Pre-serialized inputs for the deserialize side.
    std::vector<std::string> ser(n);
    for (size_t i = 0; i < n; ++i) messages[i]->SerializeToString(&ser[i]);

    {
        PerfCounters probe;
        std::printf("===============================================================\n");
        std::printf(" Protobuf de/encoding characterization (instrumentation)\n");
        std::printf(" messages=%zu  ops/pass=%zu  hw_counters=%s\n", n, ops,
                    probe.hardware_available() ? "available" : "UNAVAILABLE");
        std::printf(" (one object per op; first-parse field alloc included)\n");
        std::printf("===============================================================\n");
    }

    // ----------------------------------------------------------------------
    // SERIALIZE
    // ----------------------------------------------------------------------
    PassResult size_pass = measure([&] {
        for (size_t i = 0; i < ops; ++i) messages[i]->ByteSizeLong();
    });
    // Ensure every cached size is valid before the pure-write pass.
    for (size_t i = 0; i < n; ++i) messages[i]->ByteSizeLong();
    PassResult encode_pass = measure([&] {
        for (size_t i = 0; i < ops; ++i)
            messages[i]->SerializeWithCachedSizesToArray(buf.data());
    });
    PassResult alloc_pass = measure([&] {
        for (size_t i = 0; i < ops; ++i) {
            std::string out;
            out.resize(messages[i]->GetCachedSize());
            // touch so the allocation is not optimized away
            if (!out.empty()) out[0] = 0;
        }
    });

    const double s_size = cyc_per_op(size_pass, ops);
    const double s_encode = cyc_per_op(encode_pass, ops);
    const double s_alloc = cyc_per_op(alloc_pass, ops);
    std::printf("\n[SERIALIZE]  cycles/op by sub-step:\n");
    std::printf("    size pass  (ByteSizeLong)              : %10.1f\n", s_size);
    std::printf("    encode pass(SerializeWithCachedSizes)  : %10.1f\n", s_encode);
    std::printf("    alloc pass (output string resize)      : %10.1f\n", s_alloc);
    bucket_table("serialize", s_size + s_encode, s_alloc);

    // ----------------------------------------------------------------------
    // DESERIALIZE
    // ----------------------------------------------------------------------
    PassResult first_parse_pass = measure([&] {
        for (size_t i = 0; i < ops; ++i)
            deser_out[i]->ParseFromString(ser[i]);
    });
    PassResult heap_pass = measure([&] {
        for (size_t i = 0; i < ops; ++i) {
            Message* m = messages[i]->New();
            m->ParseFromString(ser[i]);
            delete m;
        }
    });
    PassResult arena_pass = measure([&] {
        Arena arena;
        for (size_t i = 0; i < ops; ++i) {
            Message* m = messages[i]->New(&arena);
            m->ParseFromString(ser[i]);
            arena.Reset();
        }
    });

    const double d_first = cyc_per_op(first_parse_pass, ops);
    const double d_heap = cyc_per_op(heap_pass, ops);
    const double d_arena = cyc_per_op(arena_pass, ops);
    std::printf("\n[DESERIALIZE]  cycles/op by parse mode:\n");
    std::printf("    first-parse(pre-alloc shell, field alloc): %10.1f\n", d_first);
    std::printf("    fresh-heap (New + parse + delete each op) : %10.1f\n", d_heap);
    std::printf("    fresh-arena(arena New + parse + reset)   : %10.1f\n", d_arena);
    if (d_heap > 0) {
        const double mem = d_heap - d_first;          // shell alloc + ctor/dtor
        const double enc = d_first;                     // decode + field alloc
        bucket_table("deserialize", enc, mem > 0 ? mem : 0.0);
        std::printf("    (arena removes %.1f%% of the heap shell overhead)\n",
                    (d_heap - d_first) > 0
                        ? 100.0 * (d_heap - d_arena) / (d_heap - d_first)
                        : 0.0);
    }

    for (Message* m : messages) delete m;
    for (Message* m : deser_out) delete m;
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
