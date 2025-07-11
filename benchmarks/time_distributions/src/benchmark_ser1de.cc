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

static constexpr size_t kNofIterations = 10001;

static constexpr size_t warmup = 5;

void report_timings(std::vector<std::chrono::nanoseconds> perfs, std::string stat) {
    std::cout << stat << "(ns)";
    for (auto perf: perfs) std::cout << ", " << perf.count();
    std::cout << "\n";
}

inline void benchmark_serialize(std::vector<google::protobuf::Message*>& messages, std::vector<std::string>& ser_outs, std::vector<std::chrono::nanoseconds>& serialization_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < messages.size(); ++i) {
        begin = std::chrono::steady_clock::now();
        auto outcome = messages[i]->SerializeToString(&ser_outs[i]);
        end = std::chrono::steady_clock::now();

        if (!outcome) {
            std::cerr << "Benchmark error." << std::endl;
            return;
        }

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        serialization_durations.push_back(duration);
    }
}

inline void benchmark_deserialize(std::vector<std::string>& ser_outs, std::vector<google::protobuf::Message*>& deser_messages_out, std::vector<std::chrono::nanoseconds>& deserialization_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < deser_messages_out.size(); ++i) {
        begin = std::chrono::steady_clock::now();
        auto outcome = deser_messages_out[i]->ParseFromString(ser_outs[i]);
        end = std::chrono::steady_clock::now();

        if (!outcome) {
            std::cerr << "Benchmark error." << std::endl;
            return;
        }

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        deserialization_durations.push_back(duration);
    }
}

inline size_t benchmark_ser1de_serialize(Ser1de &ser1de, std::vector<google::protobuf::Message*>& messages, std::vector<std::string>& ser_outs, std::vector<std::chrono::nanoseconds>& serialization_durations) {
    //size_t BATCHING = 1;
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < messages.size(); ++i) {
        begin = std::chrono::steady_clock::now();
        ser1de.SerializeToStringDebug(*messages[i], &ser_outs[i]);
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        serialization_durations.push_back(duration);
    }
    //std::cout << "gather_out (bytes), " << ser1de.get_ser_gather_out_size() << "\n";
    return ser1de.get_ser_gather_out_size();
}

inline void benchmark_ser1de_deserialize(Ser1de &ser1de, std::vector<std::string>& ser_outs, std::vector<google::protobuf::Message*>& deser_messages_out, std::vector<std::chrono::nanoseconds>& deserialization_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < deser_messages_out.size(); ++i) {
        begin = std::chrono::steady_clock::now();
        ser1de.ParseFromStringDebug(ser_outs[i], deser_messages_out[i]);
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        deserialization_durations.push_back(duration);
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
    for (size_t i = 0; i < kNofIterations && all_correct; ++i) {
        all_correct = differencer.Compare(*messages[i], *ser1de_deser_messages_out[i]);
        //all_correct = google::protobuf::util::MessageDifferencer::Equivalent(messages[i], ser1de_deser_messages_out[i]);
    }
    std::cout << (all_correct ? "ALL CORRECT" : "ERROR: DATA MISSMATCH") << std::endl;

    if (!all_correct) {
        std::cout << "Differences: " << differences << std::endl;
    }
    assert(all_correct);
}

int benchmark (Ser1de& ser1de, size_t message_id, size_t setter) {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::vector<google::protobuf::Message*> messages;
    std::vector<google::protobuf::Message*> proto_deser_messages_out;
    std::vector<google::protobuf::Message*> ser1de_deser_messages_out;

    std::vector<std::string> proto_ser_outs(kNofIterations);
    std::vector<std::string> ser1de_ser_outs(kNofIterations);

    // vectors and time structs for holding performance numbers
    std::vector<std::chrono::nanoseconds> proto_serialization_durations, proto_deserialization_durations;
    std::vector<std::chrono::nanoseconds> ser1de_serialization_durations, ser1de_deserialization_durations;

    std::string s("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz0123456789");
    for (size_t i = 0; i < kNofIterations; ++i) {
        switch (message_id) {
            case 0: {
                // initialize and populate the original message
                google::protobuf::Message* m = new fleetbench::proto::Message0();
                if (setter == 1) {
                    fleetbench::proto::Message0_Set_1(reinterpret_cast<fleetbench::proto::Message0*>(m), &s);
                } else if (setter == 2) {
                    fleetbench::proto::Message0_Set_2(reinterpret_cast<fleetbench::proto::Message0*>(m), &s);
                }
                else if (setter == 3) {
                    fleetbench::proto::Message0_Set_3(reinterpret_cast<fleetbench::proto::Message0*>(m), &s);
                }
                messages.push_back(m);
                // initialize proto deser message
                google::protobuf::Message* proto_deser_m = new fleetbench::proto::Message0();
                proto_deser_messages_out.push_back(proto_deser_m);
                // initialize ser1de deser message
                google::protobuf::Message* ser1de_deser_m = new fleetbench::proto::Message0();
                ser1de_deser_messages_out.push_back(ser1de_deser_m);
                break;
            }
            case 1: {
                // initialize and populate the original message
                google::protobuf::Message* m = new fleetbench::proto::Message1();
                if (setter == 1) {
                    fleetbench::proto::Message1_Set_1(reinterpret_cast<fleetbench::proto::Message1*>(m), &s);
                } else if (setter == 2) {
                    fleetbench::proto::Message1_Set_2(reinterpret_cast<fleetbench::proto::Message1*>(m), &s);
                }
                else if (setter == 3) {
                    fleetbench::proto::Message1_Set_3(reinterpret_cast<fleetbench::proto::Message1*>(m), &s);
                }
                messages.push_back(m);
                // initialize proto deser message
                google::protobuf::Message* proto_deser_m = new fleetbench::proto::Message1();
                proto_deser_messages_out.push_back(proto_deser_m);
                // initialize ser1de deser message
                google::protobuf::Message* ser1de_deser_m = new fleetbench::proto::Message1();
                ser1de_deser_messages_out.push_back(ser1de_deser_m);
                break;
            }
            case 2: {
                // initialize and populate the original message
                google::protobuf::Message* m = new fleetbench::proto::Message2();
                if (setter == 1) {
                    fleetbench::proto::Message2_Set_1(reinterpret_cast<fleetbench::proto::Message2*>(m), &s);
                } else if (setter == 2) {
                    fleetbench::proto::Message2_Set_2(reinterpret_cast<fleetbench::proto::Message2*>(m), &s);
                }
                else if (setter == 3) {
                    fleetbench::proto::Message2_Set_3(reinterpret_cast<fleetbench::proto::Message2*>(m), &s);
                }
                messages.push_back(m);
                // initialize proto deser message
                google::protobuf::Message* proto_deser_m = new fleetbench::proto::Message2();
                proto_deser_messages_out.push_back(proto_deser_m);
                // initialize ser1de deser message
                google::protobuf::Message* ser1de_deser_m = new fleetbench::proto::Message2();
                ser1de_deser_messages_out.push_back(ser1de_deser_m);
                break;
            }
            case 3: {
                // initialize and populate the original message
                google::protobuf::Message* m = new fleetbench::proto::Message3();
                if (setter == 1) {
                    fleetbench::proto::Message3_Set_1(reinterpret_cast<fleetbench::proto::Message3*>(m), &s);
                } else if (setter == 2) {
                    fleetbench::proto::Message3_Set_2(reinterpret_cast<fleetbench::proto::Message3*>(m), &s);
                }
                else if (setter == 3) {
                    fleetbench::proto::Message3_Set_3(reinterpret_cast<fleetbench::proto::Message3*>(m), &s);
                }
                messages.push_back(m);
                // initialize proto deser message
                google::protobuf::Message* proto_deser_m = new fleetbench::proto::Message3();
                proto_deser_messages_out.push_back(proto_deser_m);
                // initialize ser1de deser message
                google::protobuf::Message* ser1de_deser_m = new fleetbench::proto::Message3();
                ser1de_deser_messages_out.push_back(ser1de_deser_m);
                break;
            }
            case 4: {
                // initialize and populate the original message
                google::protobuf::Message* m = new fleetbench::proto::Message4();
                if (setter == 1) {
                    fleetbench::proto::Message4_Set_1(reinterpret_cast<fleetbench::proto::Message4*>(m), &s);
                } else if (setter == 2) {
                    fleetbench::proto::Message4_Set_2(reinterpret_cast<fleetbench::proto::Message4*>(m), &s);
                }
                else if (setter == 3) {
                    fleetbench::proto::Message4_Set_3(reinterpret_cast<fleetbench::proto::Message4*>(m), &s);
                }
                messages.push_back(m);
                // initialize proto deser message
                google::protobuf::Message* proto_deser_m = new fleetbench::proto::Message4();
                proto_deser_messages_out.push_back(proto_deser_m);
                // initialize ser1de deser message
                google::protobuf::Message* ser1de_deser_m = new fleetbench::proto::Message4();
                ser1de_deser_messages_out.push_back(ser1de_deser_m);
                break;
            }
            case 5: {
                // initialize and populate the original message
                google::protobuf::Message* m = new fleetbench::proto::Message5();
                fleetbench::proto::Message5_Set_1(reinterpret_cast<fleetbench::proto::Message5*>(m), &s);
                messages.push_back(m);
                // initialize proto deser message
                google::protobuf::Message* proto_deser_m = new fleetbench::proto::Message5();
                proto_deser_messages_out.push_back(proto_deser_m);
                // initialize ser1de deser message
                google::protobuf::Message* ser1de_deser_m = new fleetbench::proto::Message5();
                ser1de_deser_messages_out.push_back(ser1de_deser_m);
                break;
            }
            case 6: {
                // initialize and populate the original message
                google::protobuf::Message* m = new fleetbench::proto::Message6();
                fleetbench::proto::Message6_Set_1(reinterpret_cast<fleetbench::proto::Message6*>(m), &s);
                messages.push_back(m);
                // initialize proto deser message
                google::protobuf::Message* proto_deser_m = new fleetbench::proto::Message6();
                proto_deser_messages_out.push_back(proto_deser_m);
                // initialize ser1de deser message
                google::protobuf::Message* ser1de_deser_m = new fleetbench::proto::Message6();
                ser1de_deser_messages_out.push_back(ser1de_deser_m);
                break;
            }
            case 7: {
                // initialize and populate the original message
                google::protobuf::Message* m = new fleetbench::proto::Message7();
                if (setter == 1) {
                    fleetbench::proto::Message7_Set_1(reinterpret_cast<fleetbench::proto::Message7*>(m), &s);
                } else if (setter == 2) {
                    fleetbench::proto::Message7_Set_2(reinterpret_cast<fleetbench::proto::Message7*>(m), &s);
                }
                else if (setter == 3) {
                    fleetbench::proto::Message7_Set_3(reinterpret_cast<fleetbench::proto::Message7*>(m), &s);
                }
                else if (setter == 4) {
                    fleetbench::proto::Message7_Set_4(reinterpret_cast<fleetbench::proto::Message7*>(m), &s);
                }
                messages.push_back(m);
                // initialize proto deser message
                google::protobuf::Message* proto_deser_m = new fleetbench::proto::Message7();
                proto_deser_messages_out.push_back(proto_deser_m);
                // initialize ser1de deser message
                google::protobuf::Message* ser1de_deser_m = new fleetbench::proto::Message7();
                ser1de_deser_messages_out.push_back(ser1de_deser_m);
                break;
            }
            case 8: {
                // initialize and populate the original message
                google::protobuf::Message* m = new fleetbench::proto::Message8();
                if (setter == 1) {
                    fleetbench::proto::Message8_Set_1(reinterpret_cast<fleetbench::proto::Message8*>(m), &s);
                } else if (setter == 2) {
                    fleetbench::proto::Message8_Set_2(reinterpret_cast<fleetbench::proto::Message8*>(m), &s);
                }
                else if (setter == 3) {
                    fleetbench::proto::Message8_Set_3(reinterpret_cast<fleetbench::proto::Message8*>(m), &s);
                }
                messages.push_back(m);
                // initialize proto deser message
                google::protobuf::Message* proto_deser_m = new fleetbench::proto::Message8();
                proto_deser_messages_out.push_back(proto_deser_m);
                // initialize ser1de deser message
                google::protobuf::Message* ser1de_deser_m = new fleetbench::proto::Message8();
                ser1de_deser_messages_out.push_back(ser1de_deser_m);
                break;
            }
            case 9: {
                // initialize and populate the original message 
                google::protobuf::Message* m = new fleetbench::proto::Message9();
                if (setter == 1) {
                    fleetbench::proto::Message9_Set_1(reinterpret_cast<fleetbench::proto::Message9*>(m), &s);
                } else if (setter == 2) {
                    fleetbench::proto::Message9_Set_2(reinterpret_cast<fleetbench::proto::Message9*>(m), &s);
                }
                messages.push_back(m);
                // initialize proto deser message
                google::protobuf::Message* proto_deser_m = new fleetbench::proto::Message9();
                proto_deser_messages_out.push_back(proto_deser_m);
                // initialize ser1de deser message
                google::protobuf::Message* ser1de_deser_m = new fleetbench::proto::Message9();
                ser1de_deser_messages_out.push_back(ser1de_deser_m);
                break;
            }
        }
        // initialize the output strings
        proto_ser_outs.push_back("");
        ser1de_ser_outs.push_back("");
    }

    benchmark_serialize(messages, proto_ser_outs, proto_serialization_durations);

    benchmark_deserialize(proto_ser_outs, proto_deser_messages_out, proto_deserialization_durations);

    size_t ser1de_ser_out_size = benchmark_ser1de_serialize(ser1de, messages, ser1de_ser_outs, ser1de_serialization_durations);

    benchmark_ser1de_deserialize(ser1de, ser1de_ser_outs, ser1de_deser_messages_out, ser1de_deserialization_durations);

    //report_timings(proto_serialization_durations, "proto_serialization");
    //report_timings(proto_deserialization_durations, "proto_deserialization");
    //report_timings(ser1de_serialization_durations, "ser1de_serialization");
    //report_timings(ser1de_deserialization_durations, "ser1de_deserialization");

    auto proto_serialization_time = std::accumulate(proto_serialization_durations.begin(), proto_serialization_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations;  
    auto proto_deserialization_time = std::accumulate(proto_deserialization_durations.begin(), proto_deserialization_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations;
    auto ser1de_serialization_time = std::accumulate(ser1de_serialization_durations.begin(), ser1de_serialization_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations;
    auto ser1de_deserialization_time = std::accumulate(ser1de_deserialization_durations.begin(), ser1de_deserialization_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations;
    auto proto_end2end_time = proto_serialization_time + proto_deserialization_time;
    auto ser1de_end2end_time = ser1de_serialization_time + ser1de_deserialization_time;

    float ser_speedup = (static_cast<float>(proto_serialization_time) / static_cast<float>(ser1de_serialization_time) - 1) * 100;
    float deser_speedup = (static_cast<float>(proto_deserialization_time) / static_cast<float>(ser1de_deserialization_time) - 1) * 100;
    float end2end_speedup = (static_cast<float>(proto_end2end_time) / static_cast<float>(ser1de_end2end_time) - 1) * 100;
    float compression_ratio = (static_cast<float>(proto_ser_outs[0].size()) / static_cast<float>(ser1de_ser_outs[0].size()) - 1) * 100;

    std::cout << "---------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "\t\tMessage:" << message_id << ", Setter:" << setter << ", Flat Object Size: " << ser1de_ser_out_size << "bytes" << std::endl;
    std::cout << "---------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "\t\tSerialization, Deserialization, End2End, Packet Size" << std::endl;
    std::cout << "Protobuf:\t" << proto_serialization_time << "ns, " << proto_deserialization_time << "ns, " << proto_serialization_time + proto_deserialization_time << "ns, " << proto_ser_outs[0].size() << "bytes" << std::endl;
    std::cout << "Ser1de:\t\t" << ser1de_serialization_time << "ns, " << ser1de_deserialization_time << "ns, " << ser1de_serialization_time + ser1de_deserialization_time << "ns, " << ser1de_ser_outs[0].size() << "bytes" << std::endl;
    std::cout << "Speedups:\t" << ser_speedup << "%" << ", " << deser_speedup << "%" << ", " << end2end_speedup << "%" << std::endl;
    std::cout << "Compression:\t" << compression_ratio << "%" << std::endl;
    std::cout << "---------------------------------------------------------------------------------------------------------------" << std::endl;

    return 0;
}

int main () {
    Ser1de ser1de;

    benchmark(ser1de, 0, 1);
    benchmark(ser1de, 0, 2);
    benchmark(ser1de, 0, 3);
    benchmark(ser1de, 1, 1);
    benchmark(ser1de, 1, 2);
    benchmark(ser1de, 1, 3);
    benchmark(ser1de, 2, 1);
    benchmark(ser1de, 2, 2);
    benchmark(ser1de, 2, 3);
    benchmark(ser1de, 3, 1);
    benchmark(ser1de, 3, 2);
    benchmark(ser1de, 3, 3);
    benchmark(ser1de, 4, 1);
    benchmark(ser1de, 4, 2);
    benchmark(ser1de, 4, 3);
    benchmark(ser1de, 5, 1);
    benchmark(ser1de, 6, 1);
    benchmark(ser1de, 7, 1);
    benchmark(ser1de, 7, 2);
    benchmark(ser1de, 7, 3);
    benchmark(ser1de, 7, 4);
    benchmark(ser1de, 8, 1);
    benchmark(ser1de, 8, 2);
    benchmark(ser1de, 8, 3);
    benchmark(ser1de, 9, 1);
    benchmark(ser1de, 9, 2);
    /*
    */

    ser1de.exportLatenciesToCSV("ser1de_serialize_latencies.csv", "ser1de_parse_latencies.csv");
}