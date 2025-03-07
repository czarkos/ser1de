#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include"person.pb.h"
#include"proto_init.h"
#include"new_ser1de.h"

#include <tuple>

static constexpr size_t kNofIterations = 10001;

void report_timings(std::vector<std::chrono::nanoseconds> perfs, std::string stat) {
    std::cout << stat << "(ns)";
    for (auto perf: perfs) std::cout << ", " << perf.count();
    std::cout << "\n";
}

inline void benchmark_serialize(std::vector<M>& messages, std::vector<std::string>& ser_outs, std::vector<std::chrono::nanoseconds>& serialization_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < messages.size(); ++i) {
        begin = std::chrono::steady_clock::now();
        auto outcome = messages[i].SerializeToString(&ser_outs[i]);
        end = std::chrono::steady_clock::now();

        if (!outcome) {
            std::cerr << "Benchmark error." << std::endl;
            return;
        }

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        serialization_durations.push_back(duration);
    }
}

inline void benchmark_deserialize(std::vector<M>& messages, std::vector<std::string>& ser_outs, std::vector<M>& deser_messages_out, std::vector<std::chrono::nanoseconds>& deserialization_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < messages.size(); ++i) {
        begin = std::chrono::steady_clock::now();
        auto outcome = deser_messages_out[i].ParseFromString(ser_outs[i]);
        end = std::chrono::steady_clock::now();

        if (!outcome) {
            std::cerr << "Benchmark error." << std::endl;
            return;
        }

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        deserialization_durations.push_back(duration);
    }
}

inline void benchmark_ser1de_serialize(Ser1de &ser1de, std::vector<M>& messages, std::vector<std::string>& ser_outs, std::vector<std::chrono::nanoseconds>& serialization_durations) {
    //size_t BATCHING = 1;
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < messages.size(); ++i) {
        begin = std::chrono::steady_clock::now();
        ser1de.SerializeToString(messages[i], &ser_outs[i]);
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        serialization_durations.push_back(duration);
    }
    std::cout << "gather_out (bytes), " << ser1de.get_ser_gather_out_size() << "\n";
    std::cout << "compressed_out (bytes), " << ser1de.get_serComprOutputSize() << "\n";
}

inline void benchmark_ser1de_deserialize(Ser1de &ser1de, std::vector<M>& messages, std::vector<std::string>& ser_outs, std::vector<M>& deser_messages_out, std::vector<std::chrono::nanoseconds>& deserialization_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < messages.size(); ++i) {
        begin = std::chrono::steady_clock::now();
        ser1de.ParseFromString(ser_outs[i], &deser_messages_out[i]);
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        deserialization_durations.push_back(duration);
    }
}

int benchmark () {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Ser1de ser1de;

    std::vector<M> messages;
    std::vector<std::string> proto_ser_outs(kNofIterations);
    std::vector<std::string> ser1de_ser_outs(kNofIterations);

    // vectors and time structs for holding performance numbers
    std::vector<std::chrono::nanoseconds> proto_serialization_durations, proto_deserialization_durations;
    std::vector<std::chrono::nanoseconds> ser1de_serialization_durations, ser1de_deserialization_durations;

    for (size_t i = 0; i < kNofIterations; ++i) {
        M m;

        // populate the message
        person_generated_setters(m, i);

        messages.push_back(m);
        proto_ser_outs.push_back("");
        ser1de_ser_outs.push_back("");
    }

    //
    // Benchmark Serialize.
    //
    benchmark_serialize(messages, proto_ser_outs, proto_serialization_durations);
    //std::cout << "Serialization Output Size (protobuf) " << proto_ser_outs[0].size() << "\n"; 

    //
    // Benchmark Deserialize.
    //
    std::vector<M> proto_deser_messages_out;
    for (size_t i = 0; i < messages.size(); ++i) proto_deser_messages_out.push_back(M());
    benchmark_deserialize(messages, proto_ser_outs, proto_deser_messages_out, proto_deserialization_durations);

    //
    // Benchmark Ser1de Serialize.
    //
    benchmark_ser1de_serialize(ser1de, messages, ser1de_ser_outs, ser1de_serialization_durations);
    std::cout << "Serialization Output Size (protobuf) " << proto_ser_outs[0].size() << "\n"; 
    std::cout << "Serialization Output Size (ser1de) " << ser1de_ser_outs[0].size() << "\n"; 

    //
    // Benchmark Ser1de Deserialize.
    //
    // output messages after decompression and scatter
    std::vector<M> ser1de_deser_messages_out;
    for (size_t i = 0; i < kNofIterations; ++i) { ser1de_deser_messages_out.push_back(M()); }
    benchmark_ser1de_deserialize(ser1de, messages, ser1de_ser_outs, ser1de_deser_messages_out, ser1de_deserialization_durations);

    google::protobuf::util::MessageDifferencer differencer;

    std::string differences;
    google::protobuf::util::DefaultFieldComparator comparator;

    differencer.ReportDifferencesToString(&differences);
    differencer.set_field_comparator(&comparator);

    // Verify correctness
    bool all_correct = true;
    for (size_t i = 0; i < kNofIterations && all_correct; ++i) {
        all_correct = differencer.Compare(messages[i], ser1de_deser_messages_out[i]);
        all_correct = google::protobuf::util::MessageDifferencer::Equivalent(messages[i], ser1de_deser_messages_out[i]);
    }
    std::cout << (all_correct ? "ALL CORRECT" : "ERROR: DATA MISSMATCH") << std::endl;

    if (!all_correct) {
        std::cout << "Differences: " << differences << std::endl;
    }
    assert(all_correct);

    report_timings(proto_serialization_durations, "proto_serialization");
    report_timings(proto_deserialization_durations, "proto_deserialization");
    report_timings(ser1de_serialization_durations, "ser1de_serialization");
    report_timings(ser1de_deserialization_durations, "ser1de_deserialization");

    //std::cout << "Average serialization time: " << std::accumulate(proto_serialization_durations.begin(), proto_serialization_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average deserialization time: " << std::accumulate(proto_deserialization_durations.begin(), proto_deserialization_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average serialization time: " << std::accumulate(ser1de_serialization_durations.begin(), ser1de_serialization_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average deserialization time: " << std::accumulate(ser1de_deserialization_durations.begin(), ser1de_deserialization_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    return 0;
}

int main () {
    benchmark();
}