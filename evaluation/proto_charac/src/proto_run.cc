// Method A driver: a minimal Protobuf-only runner used under a sampling profiler.
//
// It builds one unique source + empty deserialize target per operation (matching
// the fleetbench_latency / fleetbench_throughput pattern), then runs ONLY the
// requested phase in a tight loop so that `perf record` can attribute samples
// to a single operation (serialize OR deserialize) without the other phase or
// setup polluting the profile.
//
// Each timed op uses index i directly (no modulo reuse), so first-parse field
// allocation and first SerializeToString into an empty output string are
// included in the profile.
//
//   ./proto_run <ops> [serialize|deserialize|both]
//
// Legacy (corpus arg ignored; ops drives corpus size):
//   ./proto_run <ops> <corpus> [serialize|deserialize|both]
//
// Defaults: ops=10001, phase=both.

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <google/protobuf/message.h>

#include "corpus.h"

using google::protobuf::Message;

static constexpr size_t kDefaultOps = 10001;

static bool is_phase(const char* arg) {
    return std::strcmp(arg, "serialize") == 0 ||
           std::strcmp(arg, "deserialize") == 0 ||
           std::strcmp(arg, "both") == 0;
}

static void serialize_loop(std::vector<Message*>& messages,
                           std::vector<std::string>& ser, size_t ops) {
    for (size_t i = 0; i < ops; ++i)
        messages[i]->SerializeToString(&ser[i]);
}

static void deserialize_loop(std::vector<std::string>& ser,
                             std::vector<Message*>& deser_out, size_t ops) {
    for (size_t i = 0; i < ops; ++i)
        deser_out[i]->ParseFromString(ser[i]);
}

int main(int argc, char** argv) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    const size_t ops =
        argc > 1 ? std::strtoull(argv[1], nullptr, 10) : kDefaultOps;

    std::string phase = "both";
    if (argc > 2) {
        if (is_phase(argv[2])) {
            phase = argv[2];
        } else if (argc > 3 && is_phase(argv[3])) {
            phase = argv[3];  // legacy: ops corpus phase
        }
    }

    if (!is_phase(phase.c_str())) {
        std::cerr << "usage: " << argv[0]
                  << " <ops> [serialize|deserialize|both]\n"
                  << "  legacy: " << argv[0]
                  << " <ops> <corpus> [serialize|deserialize|both]\n";
        return 2;
    }

    std::string s(1 << 20, 'a');
    std::vector<Message*> messages;
    std::vector<Message*> deser_out;
    proto_charac::build_corpus(ops, s, messages, deser_out);
    const size_t n = messages.size();

    // Pre-serialize each unique message so deserialize has valid wire input.
    std::vector<std::string> ser(n);
    for (size_t i = 0; i < n; ++i) messages[i]->SerializeToString(&ser[i]);

    std::cerr << "proto_run: messages=" << n << " ops=" << ops
              << " phase=" << phase << " (one object per op, no reuse)"
              << std::endl;

    if (phase == "serialize" || phase == "both")
        serialize_loop(messages, ser, ops);
    if (phase == "deserialize" || phase == "both")
        deserialize_loop(ser, deser_out, ops);

    for (Message* m : messages) delete m;
    for (Message* m : deser_out) delete m;
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
