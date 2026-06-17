// Shared corpus builder for the Protobuf characterization benchmarks.
//
// Builds one unique Fleetbench message per operation (same pattern as the
// fleetbench_latency / fleetbench_throughput benchmarks): an in-memory mix of
// all ten message types (26 messages per batch), populated via the generated
// access_message*.h Set helpers. `messages` holds populated sources;
// `deser_out` holds empty objects of the matching concrete type used as
// first-parse ParseFromString targets (one per op, no modulo reuse).

#ifndef PROTO_CHARAC_CORPUS_H
#define PROTO_CHARAC_CORPUS_H

#include <string>
#include <vector>

#include <google/protobuf/message.h>

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

namespace proto_charac {

namespace fp = fleetbench::proto;

inline void build_corpus(size_t target_messages, std::string& s,
                         std::vector<google::protobuf::Message*>& messages,
                         std::vector<google::protobuf::Message*>& deser_out) {
    using google::protobuf::Message;
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

}  // namespace proto_charac

#endif  // PROTO_CHARAC_CORPUS_H
