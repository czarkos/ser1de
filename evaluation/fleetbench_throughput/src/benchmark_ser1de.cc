#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <thread>

#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include"Message0.pb.h"
#include"Message1.pb.h"
#include"Message2.pb.h"
#include"Message3.pb.h"
#include"Message4.pb.h"
#include"Message5.pb.h"
#include"Message6.pb.h"
#include"Message7.pb.h"
#include"Message8.pb.h"
#include"Message9.pb.h"
#include"access_message0.h"
#include"access_message1.h"
#include"access_message2.h"
#include"access_message3.h"
#include"access_message4.h"
#include"access_message5.h"
#include"access_message6.h"
#include"access_message7.h"
#include"access_message8.h"
#include"access_message9.h"
#include"ser1de.h"

#include <tuple>

static constexpr size_t kNofWarmupIterations = 10;

enum BenchmarkType {
    PROTO_SERIALIZE = 0,
    PROTO_DESERIALIZE = 1,
    SER1DE_SERIALIZE = 2,
    SER1DE_DESERIALIZE = 3
};

inline void benchmark_serialize(std::vector<google::protobuf::Message*>& messages, std::vector<std::string>& ser_outs, size_t num_requests, std::vector<std::chrono::microseconds>& latencies) {
    // Warmup iterations
    for (size_t i = 0; i < kNofWarmupIterations; i++) {
        messages[i % num_requests]->SerializeToString(&ser_outs[i % num_requests]);
    }

    size_t rps = num_requests / 10;
    size_t interval = 1000000 / rps;
    auto benchmark_start_time = std::chrono::steady_clock::now();
    auto proposed_start_time = benchmark_start_time;
    for (int i = 0; i < num_requests; i++) {
        while (std::chrono::steady_clock::now() < proposed_start_time) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        messages[i]->SerializeToString(&ser_outs[i]);
        auto req_end_time = std::chrono::steady_clock::now();
        latencies[i] = std::chrono::duration_cast<std::chrono::microseconds>(req_end_time - proposed_start_time);
        proposed_start_time += std::chrono::microseconds(interval);
    }

    // auto start_time = std::chrono::steady_clock::now();
    // for (size_t i = 0; i < num_requests; i++) {
    //     auto req_start = std::chrono::steady_clock::now();
    //     messages[i]->SerializeToString(&ser_outs[i]);
    //     auto req_end = std::chrono::steady_clock::now();
    //     latencies[i] = std::chrono::duration_cast<std::chrono::microseconds>(req_end - req_start);
    // }
    // // auto end_time = std::chrono::steady_clock::now();
    // // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    // //std::cout << "Protobuf Serialize: " << duration.count() << " us" << std::endl;
}

inline void benchmark_deserialize(std::vector<std::string>& ser_outs, std::vector<google::protobuf::Message*>& deser_messages_out, size_t num_requests, std::vector<std::chrono::microseconds>& latencies) {
    // Warmup iterations
    for (size_t i = 0; i < kNofWarmupIterations; i++) {
        deser_messages_out[i % num_requests]->ParseFromString(ser_outs[i % num_requests]);
    }
    
    size_t rps = num_requests / 10;
    size_t interval = 1000000 / rps;
    auto benchmark_start_time = std::chrono::steady_clock::now();
    auto proposed_start_time = benchmark_start_time;
    for (int i = 0; i < num_requests; i++) {
        while (std::chrono::steady_clock::now() < proposed_start_time) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        deser_messages_out[i]->ParseFromString(ser_outs[i]);
        auto req_end_time = std::chrono::steady_clock::now();
        latencies[i] = std::chrono::duration_cast<std::chrono::microseconds>(req_end_time - proposed_start_time);
        proposed_start_time += std::chrono::microseconds(interval);
    }
}

inline void benchmark_ser1de_serialize(Ser1de &ser1de, std::vector<std::string>& ser_outs, std::vector<google::protobuf::Message*>& messages, size_t num_requests, std::vector<std::chrono::microseconds>& latencies) {
    // Warmup iterations
    for (size_t i = 0; i < kNofWarmupIterations; i++) {
        ser1de.SerializeToString(*messages[i % num_requests], &ser_outs[i % num_requests]);
    }
    
    size_t rps = num_requests / 10;
    size_t interval = 1000000 / rps;
    auto benchmark_start_time = std::chrono::steady_clock::now();
    auto proposed_start_time = benchmark_start_time;
    for (int i = 0; i < num_requests; i++) {
        while (std::chrono::steady_clock::now() < proposed_start_time) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        ser1de.SerializeToString(*messages[i], &ser_outs[i]);
        auto req_end_time = std::chrono::steady_clock::now();
        latencies[i] = std::chrono::duration_cast<std::chrono::microseconds>(req_end_time - proposed_start_time);
        proposed_start_time += std::chrono::microseconds(interval);
    }
}

inline void benchmark_ser1de_deserialize(Ser1de &ser1de, std::vector<std::string>& ser_outs, std::vector<google::protobuf::Message*>& deser_messages_out, size_t num_requests, std::vector<std::chrono::microseconds>& latencies) {
    // Warmup iterations
    for (size_t i = 0; i < kNofWarmupIterations; i++) {
        ser1de.ParseFromString(ser_outs[i % num_requests], deser_messages_out[i % num_requests]);
    }

    size_t rps = num_requests / 10;
    size_t interval = 1000000 / rps;
    auto benchmark_start_time = std::chrono::steady_clock::now();
    auto proposed_start_time = benchmark_start_time;
    for (int i = 0; i < num_requests; i++) {
        while (std::chrono::steady_clock::now() < proposed_start_time) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        ser1de.ParseFromString(ser_outs[i], deser_messages_out[i]);
        auto req_end_time = std::chrono::steady_clock::now();
        latencies[i] = std::chrono::duration_cast<std::chrono::microseconds>(req_end_time - proposed_start_time);
        proposed_start_time += std::chrono::microseconds(interval);
    }
}

void compare_messages(std::vector<google::protobuf::Message*>& messages, std::vector<google::protobuf::Message*>& ser1de_deser_messages_out) {
    google::protobuf::util::MessageDifferencer differencer;
    std::string differences;
    google::protobuf::util::DefaultFieldComparator comparator;
    differencer.ReportDifferencesToString(&differences);
    differencer.set_field_comparator(&comparator);

    // Verify correctness
    bool all_correct = true;
    for (size_t i = 0; i < messages.size() && all_correct; ++i) {
        all_correct = differencer.Compare(*messages[i], *ser1de_deser_messages_out[i]);
        //all_correct = google::protobuf::util::MessageDifferencer::Equivalent(messages[i], ser1de_deser_messages_out[i]);
    }
    std::cout << (all_correct ? "ALL CORRECT" : "ERROR: DATA MISSMATCH") << std::endl;

    if (!all_correct) {
        std::cout << "Differences: " << differences << std::endl;
    }
    assert(all_correct);
}

int benchmark(size_t num_requests, BenchmarkType type) {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Ser1de ser1de;

    std::vector<google::protobuf::Message*> messages;
    std::vector<google::protobuf::Message*> proto_deser_messages_out;
    std::vector<google::protobuf::Message*> ser1de_deser_messages_out;

    std::vector<std::string> proto_ser_outs(num_requests);
    std::vector<std::string> ser1de_ser_outs(num_requests);

    // Initialize the protos
    std::string s(1<<20, 'a');
    // define s as 1<<20 'a's
    for (size_t i = 0; i < num_requests; i += 26) {
        // Message 0
        google::protobuf::Message* m0_1 = new fleetbench::proto::Message0();
        fleetbench::proto::Message0_Set_1(reinterpret_cast<fleetbench::proto::Message0*>(m0_1), &s);
        google::protobuf::Message* m0_2 = new fleetbench::proto::Message0();
        fleetbench::proto::Message0_Set_2(reinterpret_cast<fleetbench::proto::Message0*>(m0_2), &s);
        google::protobuf::Message* m0_3 = new fleetbench::proto::Message0();
        fleetbench::proto::Message0_Set_3(reinterpret_cast<fleetbench::proto::Message0*>(m0_3), &s);
        messages.push_back(m0_1);
        messages.push_back(m0_2);
        messages.push_back(m0_3);
        // initialize proto deser message
        google::protobuf::Message* proto_deser_m0_1 = new fleetbench::proto::Message0();
        proto_deser_messages_out.push_back(proto_deser_m0_1);
        google::protobuf::Message* proto_deser_m0_2 = new fleetbench::proto::Message0();
        proto_deser_messages_out.push_back(proto_deser_m0_2);
        google::protobuf::Message* proto_deser_m0_3 = new fleetbench::proto::Message0();
        proto_deser_messages_out.push_back(proto_deser_m0_3);
        // initialize ser1de deser message
        google::protobuf::Message* ser1de_deser_m0_1 = new fleetbench::proto::Message0();
        ser1de_deser_messages_out.push_back(ser1de_deser_m0_1);
        google::protobuf::Message* ser1de_deser_m0_2 = new fleetbench::proto::Message0();
        ser1de_deser_messages_out.push_back(ser1de_deser_m0_2);
        google::protobuf::Message* ser1de_deser_m0_3 = new fleetbench::proto::Message0();
        ser1de_deser_messages_out.push_back(ser1de_deser_m0_3);
        // Message 1
        google::protobuf::Message* m1_1 = new fleetbench::proto::Message1();
        fleetbench::proto::Message1_Set_1(reinterpret_cast<fleetbench::proto::Message1*>(m1_1), &s);
        google::protobuf::Message* m1_2 = new fleetbench::proto::Message1();
        fleetbench::proto::Message1_Set_2(reinterpret_cast<fleetbench::proto::Message1*>(m1_2), &s);
        google::protobuf::Message* m1_3 = new fleetbench::proto::Message1();
        fleetbench::proto::Message1_Set_3(reinterpret_cast<fleetbench::proto::Message1*>(m1_3), &s);
        messages.push_back(m1_1);
        messages.push_back(m1_2);
        messages.push_back(m1_3);
        // initialize proto deser message
        google::protobuf::Message* proto_deser_m1_1 = new fleetbench::proto::Message1();
        proto_deser_messages_out.push_back(proto_deser_m1_1);
        google::protobuf::Message* proto_deser_m1_2 = new fleetbench::proto::Message1();
        proto_deser_messages_out.push_back(proto_deser_m1_2);
        google::protobuf::Message* proto_deser_m1_3 = new fleetbench::proto::Message1();
        proto_deser_messages_out.push_back(proto_deser_m1_3);
        // initialize ser1de deser message
        google::protobuf::Message* ser1de_deser_m1_1 = new fleetbench::proto::Message1();
        ser1de_deser_messages_out.push_back(ser1de_deser_m1_1);
        google::protobuf::Message* ser1de_deser_m1_2 = new fleetbench::proto::Message1();
        ser1de_deser_messages_out.push_back(ser1de_deser_m1_2);
        google::protobuf::Message* ser1de_deser_m1_3 = new fleetbench::proto::Message1();
        ser1de_deser_messages_out.push_back(ser1de_deser_m1_3);
        // Message 2
        google::protobuf::Message* m2_1 = new fleetbench::proto::Message2();
        fleetbench::proto::Message2_Set_1(reinterpret_cast<fleetbench::proto::Message2*>(m2_1), &s);
        google::protobuf::Message* m2_2 = new fleetbench::proto::Message2();
        fleetbench::proto::Message2_Set_2(reinterpret_cast<fleetbench::proto::Message2*>(m2_2), &s);
        google::protobuf::Message* m2_3 = new fleetbench::proto::Message2();
        fleetbench::proto::Message2_Set_3(reinterpret_cast<fleetbench::proto::Message2*>(m2_3), &s);
        messages.push_back(m2_1);
        messages.push_back(m2_2);
        messages.push_back(m2_3);
        // initialize proto deser message
        google::protobuf::Message* proto_deser_m2_1 = new fleetbench::proto::Message2();
        proto_deser_messages_out.push_back(proto_deser_m2_1);
        google::protobuf::Message* proto_deser_m2_2 = new fleetbench::proto::Message2();
        proto_deser_messages_out.push_back(proto_deser_m2_2);
        google::protobuf::Message* proto_deser_m2_3 = new fleetbench::proto::Message2();
        proto_deser_messages_out.push_back(proto_deser_m2_3);
        // initialize ser1de deser message
        google::protobuf::Message* ser1de_deser_m2_1 = new fleetbench::proto::Message2();
        ser1de_deser_messages_out.push_back(ser1de_deser_m2_1);
        google::protobuf::Message* ser1de_deser_m2_2 = new fleetbench::proto::Message2();
        ser1de_deser_messages_out.push_back(ser1de_deser_m2_2);
        google::protobuf::Message* ser1de_deser_m2_3 = new fleetbench::proto::Message2();
        ser1de_deser_messages_out.push_back(ser1de_deser_m2_3);
        // Message 3
        google::protobuf::Message* m3_1 = new fleetbench::proto::Message3();
        fleetbench::proto::Message3_Set_1(reinterpret_cast<fleetbench::proto::Message3*>(m3_1), &s);
        google::protobuf::Message* m3_2 = new fleetbench::proto::Message3();
        fleetbench::proto::Message3_Set_2(reinterpret_cast<fleetbench::proto::Message3*>(m3_2), &s);
        google::protobuf::Message* m3_3 = new fleetbench::proto::Message3();    
        fleetbench::proto::Message3_Set_3(reinterpret_cast<fleetbench::proto::Message3*>(m3_3), &s);
        messages.push_back(m3_1);
        messages.push_back(m3_2);
        messages.push_back(m3_3);
        // initialize proto deser message
        google::protobuf::Message* proto_deser_m3_1 = new fleetbench::proto::Message3();    
        proto_deser_messages_out.push_back(proto_deser_m3_1);   
        google::protobuf::Message* proto_deser_m3_2 = new fleetbench::proto::Message3();
        proto_deser_messages_out.push_back(proto_deser_m3_2);
        google::protobuf::Message* proto_deser_m3_3 = new fleetbench::proto::Message3();
        proto_deser_messages_out.push_back(proto_deser_m3_3);
        // initialize ser1de deser message
        google::protobuf::Message* ser1de_deser_m3_1 = new fleetbench::proto::Message3();
        ser1de_deser_messages_out.push_back(ser1de_deser_m3_1);
        google::protobuf::Message* ser1de_deser_m3_2 = new fleetbench::proto::Message3();
        ser1de_deser_messages_out.push_back(ser1de_deser_m3_2);
        google::protobuf::Message* ser1de_deser_m3_3 = new fleetbench::proto::Message3();
        ser1de_deser_messages_out.push_back(ser1de_deser_m3_3); 
        // Message 4
        google::protobuf::Message* m4_1 = new fleetbench::proto::Message4();
        fleetbench::proto::Message4_Set_1(reinterpret_cast<fleetbench::proto::Message4*>(m4_1), &s);
        google::protobuf::Message* m4_2 = new fleetbench::proto::Message4();
        fleetbench::proto::Message4_Set_2(reinterpret_cast<fleetbench::proto::Message4*>(m4_2), &s);
        google::protobuf::Message* m4_3 = new fleetbench::proto::Message4();    
        fleetbench::proto::Message4_Set_3(reinterpret_cast<fleetbench::proto::Message4*>(m4_3), &s);
        messages.push_back(m4_1);
        messages.push_back(m4_2);
        messages.push_back(m4_3);
        // initialize proto deser message
        google::protobuf::Message* proto_deser_m4_1 = new fleetbench::proto::Message4();
        proto_deser_messages_out.push_back(proto_deser_m4_1);
        google::protobuf::Message* proto_deser_m4_2 = new fleetbench::proto::Message4();
        proto_deser_messages_out.push_back(proto_deser_m4_2);
        google::protobuf::Message* proto_deser_m4_3 = new fleetbench::proto::Message4();
        proto_deser_messages_out.push_back(proto_deser_m4_3);
        // initialize ser1de deser message
        google::protobuf::Message* ser1de_deser_m4_1 = new fleetbench::proto::Message4();
        ser1de_deser_messages_out.push_back(ser1de_deser_m4_1);
        google::protobuf::Message* ser1de_deser_m4_2 = new fleetbench::proto::Message4();
        ser1de_deser_messages_out.push_back(ser1de_deser_m4_2);
        google::protobuf::Message* ser1de_deser_m4_3 = new fleetbench::proto::Message4();
        ser1de_deser_messages_out.push_back(ser1de_deser_m4_3); 
        // Message 5
        google::protobuf::Message* m5_1 = new fleetbench::proto::Message5();
        fleetbench::proto::Message5_Set_1(reinterpret_cast<fleetbench::proto::Message5*>(m5_1), &s);
        messages.push_back(m5_1);
        // initialize proto deser message   
        google::protobuf::Message* proto_deser_m5_1 = new fleetbench::proto::Message5();    
        proto_deser_messages_out.push_back(proto_deser_m5_1);
        // initialize ser1de deser message
        google::protobuf::Message* ser1de_deser_m5_1 = new fleetbench::proto::Message5();
        ser1de_deser_messages_out.push_back(ser1de_deser_m5_1); 
        // Message 6
        google::protobuf::Message* m6_1 = new fleetbench::proto::Message6();
        fleetbench::proto::Message6_Set_1(reinterpret_cast<fleetbench::proto::Message6*>(m6_1), &s);
        messages.push_back(m6_1);
        // initialize proto deser message   
        google::protobuf::Message* proto_deser_m6_1 = new fleetbench::proto::Message6();
        proto_deser_messages_out.push_back(proto_deser_m6_1);
        // initialize ser1de deser message
        google::protobuf::Message* ser1de_deser_m6_1 = new fleetbench::proto::Message6();
        ser1de_deser_messages_out.push_back(ser1de_deser_m6_1);
        // Message 7
        google::protobuf::Message* m7_1 = new fleetbench::proto::Message7();
        fleetbench::proto::Message7_Set_1(reinterpret_cast<fleetbench::proto::Message7*>(m7_1), &s);
        messages.push_back(m7_1);
        google::protobuf::Message* m7_2 = new fleetbench::proto::Message7();
        fleetbench::proto::Message7_Set_2(reinterpret_cast<fleetbench::proto::Message7*>(m7_2), &s);
        messages.push_back(m7_2);
        google::protobuf::Message* m7_3 = new fleetbench::proto::Message7();
        fleetbench::proto::Message7_Set_3(reinterpret_cast<fleetbench::proto::Message7*>(m7_3), &s);
        messages.push_back(m7_3);
        google::protobuf::Message* m7_4 = new fleetbench::proto::Message7();
        fleetbench::proto::Message7_Set_4(reinterpret_cast<fleetbench::proto::Message7*>(m7_4), &s);
        messages.push_back(m7_4);
        // initialize proto deser message
        google::protobuf::Message* proto_deser_m7_1 = new fleetbench::proto::Message7();
        proto_deser_messages_out.push_back(proto_deser_m7_1);
        google::protobuf::Message* proto_deser_m7_2 = new fleetbench::proto::Message7();
        proto_deser_messages_out.push_back(proto_deser_m7_2);
        google::protobuf::Message* proto_deser_m7_3 = new fleetbench::proto::Message7();
        proto_deser_messages_out.push_back(proto_deser_m7_3);
        google::protobuf::Message* proto_deser_m7_4 = new fleetbench::proto::Message7();
        proto_deser_messages_out.push_back(proto_deser_m7_4);
        // initialize ser1de deser message
        google::protobuf::Message* ser1de_deser_m7_1 = new fleetbench::proto::Message7();
        ser1de_deser_messages_out.push_back(ser1de_deser_m7_1);
        google::protobuf::Message* ser1de_deser_m7_2 = new fleetbench::proto::Message7();
        ser1de_deser_messages_out.push_back(ser1de_deser_m7_2);
        google::protobuf::Message* ser1de_deser_m7_3 = new fleetbench::proto::Message7();
        ser1de_deser_messages_out.push_back(ser1de_deser_m7_3);
        google::protobuf::Message* ser1de_deser_m7_4 = new fleetbench::proto::Message7();
        ser1de_deser_messages_out.push_back(ser1de_deser_m7_4);
        // Message 8    
        google::protobuf::Message* m8_1 = new fleetbench::proto::Message8();
        fleetbench::proto::Message8_Set_1(reinterpret_cast<fleetbench::proto::Message8*>(m8_1), &s);
        messages.push_back(m8_1);
        google::protobuf::Message* m8_2 = new fleetbench::proto::Message8();
        fleetbench::proto::Message8_Set_2(reinterpret_cast<fleetbench::proto::Message8*>(m8_2), &s);
        messages.push_back(m8_2);
        google::protobuf::Message* m8_3 = new fleetbench::proto::Message8();
        fleetbench::proto::Message8_Set_3(reinterpret_cast<fleetbench::proto::Message8*>(m8_3), &s);
        messages.push_back(m8_3);
        // initialize proto deser message
        google::protobuf::Message* proto_deser_m8_1 = new fleetbench::proto::Message8();
        proto_deser_messages_out.push_back(proto_deser_m8_1);   
        google::protobuf::Message* proto_deser_m8_2 = new fleetbench::proto::Message8();
        proto_deser_messages_out.push_back(proto_deser_m8_2);
        google::protobuf::Message* proto_deser_m8_3 = new fleetbench::proto::Message8();
        proto_deser_messages_out.push_back(proto_deser_m8_3);
        // initialize ser1de deser message
        google::protobuf::Message* ser1de_deser_m8_1 = new fleetbench::proto::Message8();
        ser1de_deser_messages_out.push_back(ser1de_deser_m8_1);
        google::protobuf::Message* ser1de_deser_m8_2 = new fleetbench::proto::Message8();
        ser1de_deser_messages_out.push_back(ser1de_deser_m8_2);
        google::protobuf::Message* ser1de_deser_m8_3 = new fleetbench::proto::Message8();
        ser1de_deser_messages_out.push_back(ser1de_deser_m8_3);
        // Message 9
        google::protobuf::Message* m9_1 = new fleetbench::proto::Message9();
        fleetbench::proto::Message9_Set_1(reinterpret_cast<fleetbench::proto::Message9*>(m9_1), &s);    
        messages.push_back(m9_1);
        google::protobuf::Message* m9_2 = new fleetbench::proto::Message9();
        fleetbench::proto::Message9_Set_2(reinterpret_cast<fleetbench::proto::Message9*>(m9_2), &s);    
        messages.push_back(m9_2);
        // initialize proto deser message
        google::protobuf::Message* proto_deser_m9_1 = new fleetbench::proto::Message9();
        proto_deser_messages_out.push_back(proto_deser_m9_1);
        google::protobuf::Message* proto_deser_m9_2 = new fleetbench::proto::Message9();
        proto_deser_messages_out.push_back(proto_deser_m9_2);
        // initialize ser1de deser message
        google::protobuf::Message* ser1de_deser_m9_1 = new fleetbench::proto::Message9();
        ser1de_deser_messages_out.push_back(ser1de_deser_m9_1);
        google::protobuf::Message* ser1de_deser_m9_2 = new fleetbench::proto::Message9();
        ser1de_deser_messages_out.push_back(ser1de_deser_m9_2);
    }
    
    std::cout << "-----------------------------------------------------" << std::endl;
    std::cout << "\tRequest per Second: " << num_requests/10 << " for 10 Seconds" << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;

    std::vector<std::chrono::microseconds> proto_ser_latencies(num_requests);
    std::vector<std::chrono::microseconds> proto_deser_latencies(num_requests);
    std::vector<std::chrono::microseconds> ser1de_ser_latencies(num_requests);
    std::vector<std::chrono::microseconds> ser1de_deser_latencies(num_requests);

    std::vector<std::chrono::microseconds> latencies(num_requests);
    
    // Replace the four benchmark calls with a switch statement
    switch (type) {
        case PROTO_SERIALIZE:
            benchmark_serialize(messages, proto_ser_outs, num_requests, latencies);
            std::cout << "\nTail Latencies (microseconds) - Proto Serialize:" << std::endl;
            break;
        case PROTO_DESERIALIZE:
            benchmark_serialize(messages, proto_ser_outs, num_requests, proto_ser_latencies); // Need to serialize first
            benchmark_deserialize(proto_ser_outs, proto_deser_messages_out, num_requests, latencies);
            std::cout << "\nTail Latencies (microseconds) - Proto Deserialize:" << std::endl;
            break;
        case SER1DE_SERIALIZE:
            benchmark_ser1de_serialize(ser1de, ser1de_ser_outs, messages, num_requests, latencies);
            std::cout << "\nTail Latencies (microseconds) - Ser1de Serialize:" << std::endl;
            break;
        case SER1DE_DESERIALIZE:
            benchmark_ser1de_serialize(ser1de, ser1de_ser_outs, messages, num_requests, ser1de_ser_latencies); // Need to serialize first
            benchmark_ser1de_deserialize(ser1de, ser1de_ser_outs, ser1de_deser_messages_out, num_requests, latencies);
            std::cout << "\nTail Latencies (microseconds) - Ser1de Deserialize:" << std::endl;
            break;
    }

    // Calculate and print tail latencies for the selected operation
    auto calculate_percentile = [](std::vector<std::chrono::microseconds>& latencies, double percentile) {
        std::vector<long long> sorted_latencies;
        sorted_latencies.reserve(latencies.size());
        for (const auto& lat : latencies) {
            sorted_latencies.push_back(lat.count());
        }
        std::sort(sorted_latencies.begin(), sorted_latencies.end());
        size_t idx = static_cast<size_t>(percentile * sorted_latencies.size());
        return sorted_latencies[idx];
    };

    std::cout << std::setw(15) << "p50" << std::setw(15) << "p95" << std::setw(15) << "p99" << std::endl;
    std::cout << std::string(45, '-') << std::endl;
    std::cout << std::setw(15) << calculate_percentile(latencies, 0.50)
              << std::setw(15) << calculate_percentile(latencies, 0.95)
              << std::setw(15) << calculate_percentile(latencies, 0.99) << std::endl;

    return 0;
}

int main() {
    std::vector<BenchmarkType> types = {
        PROTO_SERIALIZE,
        PROTO_DESERIALIZE,
        SER1DE_SERIALIZE,
        SER1DE_DESERIALIZE
    };

    // Print CSV header
    std::cout << "type,rps,p50,p95,p99" << std::endl;

    // Different RPS ranges for each type
    std::map<BenchmarkType, std::pair<int, int>> rps_ranges = {
        {PROTO_SERIALIZE, {7000, 9500}},      // 40k-80k requests
        {PROTO_DESERIALIZE, {4000, 7000}},    // 20k-60k requests
        {SER1DE_SERIALIZE, {7500, 10000}},    // 60k-100k requests
        {SER1DE_DESERIALIZE, {5000, 8000}}    // 30k-70k requests
    };

    for (auto type : types) {
        int start_rps = rps_ranges[type].first;
        int end_rps = rps_ranges[type].second;
        for (int rps = start_rps; rps <= end_rps; rps += 500) {
            benchmark(rps * 10, type);  // Convert RPS to request count (10 second duration)
        }
    }
    return 0;
}