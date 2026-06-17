#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include "person.pb.h"
#include "proto_init.h"
#include "ser1de.h"

#include <tuple>

static constexpr size_t kNofIterations = 10001;

void report_timings(std::vector<std::chrono::nanoseconds> perfs, std::string stat) {
    std::cout << stat << "(ns)";
    for (auto perf : perfs) {
        std::cout << ", " << perf.count();
    }
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

inline void benchmark_deserialize(std::vector<std::string>& ser_outs, std::vector<M>& deser_messages_out, std::vector<std::chrono::nanoseconds>& deserialization_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < ser_outs.size(); ++i) {
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

template<typename Ser1deImpl>
inline void benchmark_ser1de_serialize(Ser1deImpl& ser1de, std::vector<M>& messages, std::vector<std::string>& ser_outs, std::vector<std::chrono::nanoseconds>& serialization_durations, bool report_sizes) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < messages.size(); ++i) {
        begin = std::chrono::steady_clock::now();
        ser1de.SerializeToString(messages[i], &ser_outs[i]);
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        serialization_durations.push_back(duration);
    }

    if (report_sizes) {
        std::cout << "gather_out (bytes), " << ser1de.get_ser_gather_out_size() << "\n";
        std::cout << "compressed_out (bytes), " << ser1de.get_serComprOutputSize() << "\n";
    }
}

template<typename Ser1deImpl>
inline void benchmark_ser1de_deserialize(Ser1deImpl& ser1de, std::vector<std::string>& ser_outs, std::vector<M>& deser_messages_out, std::vector<std::chrono::nanoseconds>& deserialization_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < ser_outs.size(); ++i) {
        begin = std::chrono::steady_clock::now();
        ser1de.ParseFromString(ser_outs[i], &deser_messages_out[i]);
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        deserialization_durations.push_back(duration);
    }
}

bool verify_messages(const std::vector<M>& messages, const std::vector<M>& deser_messages_out, const std::string& label) {
    google::protobuf::util::MessageDifferencer differencer;
    std::string differences;
    google::protobuf::util::DefaultFieldComparator comparator;

    differencer.ReportDifferencesToString(&differences);
    differencer.set_field_comparator(&comparator);

    bool all_correct = true;
    for (size_t i = 0; i < messages.size() && all_correct; ++i) {
        all_correct = differencer.Compare(messages[i], deser_messages_out[i]);
        all_correct = all_correct && google::protobuf::util::MessageDifferencer::Equivalent(messages[i], deser_messages_out[i]);
    }

    std::cout << label << ": " << (all_correct ? "ALL CORRECT" : "ERROR: DATA MISSMATCH") << std::endl;
    if (!all_correct) {
        std::cout << "Differences: " << differences << std::endl;
    }
    assert(all_correct);
    return all_correct;
}

int benchmark() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Ser1de ser1de;
    Serial_Ser1de serial_ser1de_hw("Hardware");
    Serial_Ser1de serial_ser1de_sw("Software");

    std::vector<M> messages;
    std::vector<std::string> proto_ser_outs(kNofIterations);
    std::vector<std::string> ser1de_ser_outs(kNofIterations);
    std::vector<std::string> serial_ser1de_hw_ser_outs(kNofIterations);
    std::vector<std::string> serial_ser1de_sw_ser_outs(kNofIterations);

    std::vector<std::chrono::nanoseconds> proto_serialization_durations, proto_deserialization_durations;
    std::vector<std::chrono::nanoseconds> ser1de_serialization_durations, ser1de_deserialization_durations;
    std::vector<std::chrono::nanoseconds> serial_ser1de_hw_serialization_durations, serial_ser1de_hw_deserialization_durations;
    std::vector<std::chrono::nanoseconds> serial_ser1de_sw_serialization_durations, serial_ser1de_sw_deserialization_durations;

    for (size_t i = 0; i < kNofIterations; ++i) {
        M m;
        person_generated_setters(m, i);
        messages.push_back(m);
    }

    std::vector<M> proto_deser_messages_out(kNofIterations);
    std::vector<M> ser1de_deser_messages_out(kNofIterations);
    std::vector<M> serial_ser1de_hw_deser_messages_out(kNofIterations);
    std::vector<M> serial_ser1de_sw_deser_messages_out(kNofIterations);

    benchmark_serialize(messages, proto_ser_outs, proto_serialization_durations);
    benchmark_deserialize(proto_ser_outs, proto_deser_messages_out, proto_deserialization_durations);

    benchmark_ser1de_serialize(ser1de, messages, ser1de_ser_outs, ser1de_serialization_durations, true);
    benchmark_ser1de_deserialize(ser1de, ser1de_ser_outs, ser1de_deser_messages_out, ser1de_deserialization_durations);
    verify_messages(messages, ser1de_deser_messages_out, "Ser1de");

    benchmark_ser1de_serialize(serial_ser1de_hw, messages, serial_ser1de_hw_ser_outs, serial_ser1de_hw_serialization_durations, false);
    benchmark_ser1de_deserialize(serial_ser1de_hw, serial_ser1de_hw_ser_outs, serial_ser1de_hw_deser_messages_out, serial_ser1de_hw_deserialization_durations);
    verify_messages(messages, serial_ser1de_hw_deser_messages_out, "Serial_Ser1de HW");

    benchmark_ser1de_serialize(serial_ser1de_sw, messages, serial_ser1de_sw_ser_outs, serial_ser1de_sw_serialization_durations, false);
    benchmark_ser1de_deserialize(serial_ser1de_sw, serial_ser1de_sw_ser_outs, serial_ser1de_sw_deser_messages_out, serial_ser1de_sw_deserialization_durations);
    verify_messages(messages, serial_ser1de_sw_deser_messages_out, "Serial_Ser1de SW");

    std::cout << "Serialization Output Size (protobuf) " << proto_ser_outs[0].size() << "\n";
    std::cout << "Serialization Output Size (ser1de) " << ser1de_ser_outs[0].size() << "\n";

    report_timings(proto_serialization_durations, "proto_serialization");
    report_timings(proto_deserialization_durations, "proto_deserialization");
    report_timings(ser1de_serialization_durations, "ser1de_serialization");
    report_timings(ser1de_deserialization_durations, "ser1de_deserialization");
    report_timings(serial_ser1de_hw_serialization_durations, "serial_ser1de_hw_serialization");
    report_timings(serial_ser1de_hw_deserialization_durations, "serial_ser1de_hw_deserialization");
    report_timings(serial_ser1de_sw_serialization_durations, "serial_ser1de_sw_serialization");
    report_timings(serial_ser1de_sw_deserialization_durations, "serial_ser1de_sw_deserialization");

    return 0;
}

int main() {
    benchmark();
}
