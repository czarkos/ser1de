#include <google/protobuf/util/message_differencer.h>

#include"person.pb.h"
#include"scatter_gather.h"

//static constexpr size_t kNofIterations = 100001;
static constexpr size_t kNofIterations = 31;

#define SCATTER_GATHER_BUFFER_SIZE 4096
#define ACTUAL_BUFFER_SIZE 4096
//<------------ BUFFER SIZE ------>

void report_timings(std::vector<std::chrono::nanoseconds> perfs, std::string stat) {
    std::cout << stat << "(ns)";
    for (auto perf: perfs) std::cout << ", " << perf.count();
    std::cout << "\n";
}

int main () {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::vector<M> messages;

    // vectors and time structs for holding performance numbers
    std::vector<std::chrono::nanoseconds> serialization_durations(kNofIterations, std::chrono::nanoseconds(0)), deserialization_durations(kNofIterations, std::chrono::nanoseconds(0));
    std::vector<std::chrono::nanoseconds> compression_durations(kNofIterations, std::chrono::nanoseconds(0)), decompression_durations(kNofIterations, std::chrono::nanoseconds(0));
    ///////////////////////////////////////////////////////////////////////////////////////////
    std::vector<std::chrono::nanoseconds> gather_durations, scatter_durations;
    std::vector<std::chrono::nanoseconds> gather_schema_durations, scatter_schema_durations;
    std::vector<std::chrono::nanoseconds> allocation_durations;
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < kNofIterations; ++i) {
        M m;

        // <------------ SETTERS ------>

        //messages[i] = m;
        messages.push_back(m);
        //ser_outs.push_back("");
    }

    // Gather schemas
    std::vector<ScatterGather::Schema> gather_schemas;
    std::vector<std::vector<size_t>> sizes_for_scatter;
    for (size_t i = 0; i < kNofIterations; ++i) {
        ScatterGather::Schema gather_schema;
        gather_schema.reserve(SCATTER_GATHER_BUFFER_SIZE);
        std::vector<size_t> sizes;
        sizes.reserve(SCATTER_GATHER_BUFFER_SIZE);

        begin = std::chrono::steady_clock::now();
        // <------------ GATHER SCHEMA ------>
        end = std::chrono::steady_clock::now();
        gather_schemas.push_back(gather_schema);
        sizes_for_scatter.push_back(sizes);

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        gather_schema_durations.push_back(duration);
    }

    // GATHER + COMPRESSION
    //ScatterGather scagatherer(DML_PATH_SW, 99);
    ScatterGather scagatherer(DML_PATH_SW);
    size_t out_size = 0;
    std::vector<std::vector<uint8_t>> gather_outs(kNofIterations, std::vector<uint8_t>(ACTUAL_BUFFER_SIZE, 0));

    // gather + compression
    std::chrono::steady_clock::time_point gather_begin, gather_end;
    std::chrono::nanoseconds gather_duration;
    gather_begin = std::chrono::steady_clock::now();
    for (size_t i = 0; i < kNofIterations; ++i) {
        // std::cout << "I AM HEREEEEEEE" << i << std::endl;
         begin = std::chrono::steady_clock::now();
         //auto outcome = scagatherer.GatherWithMemCpy(gather_schemas[i], gather_outs[i].data(), &out_size);
         //auto outcome = scagatherer.GatherWithMemCpy(gather_schemas[i], gather_outs[i].data(), &out_size);
         scagatherer.gather_data_blocking(gather_schemas[i], gather_outs[i].data(), &out_size);
         end = std::chrono::steady_clock::now();

        /*
        if (outcome) {
            std::cerr << "Failed to gather" << std::endl;
            return -1;
        }
        */

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        gather_durations.push_back(duration);
    }
    scagatherer.wait_for_all_dsa_jobs();
    gather_end = std::chrono::steady_clock::now();
    gather_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(gather_end - gather_begin);
    std::cout << "average gather time = " << (gather_duration.count() / kNofIterations) << " ns\n";
    std::cout << "I AM HEREEEEEEE" << std::endl;
    // second time for good measure
    gather_begin = std::chrono::steady_clock::now();
    for (size_t i = 0; i < kNofIterations; ++i) {
         //auto outcome = scagatherer.GatherWithMemCpy(gather_schemas[i], gather_outs[i].data(), &out_size);
         //auto outcome = scagatherer.GatherWithMemCpy(gather_schemas[i], gather_outs[i].data(), &out_size);
         scagatherer.gather_data_blocking(gather_schemas[i], gather_outs[i].data(), &out_size);
    }
    scagatherer.wait_for_all_dsa_jobs();
    gather_end = std::chrono::steady_clock::now();
    gather_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(gather_end - gather_begin);
    std::cout << "average gather time = " << (gather_duration.count() / kNofIterations) << " ns\n";
    std::cout << "gather_out(bytes), " << out_size << "\n";

    // create scatter schemas and output messages
    // output messages after decompression and scatter
    std::vector<M> out_messages;
    // Scatter schemas
    std::vector<ScatterGather::Schema> scatter_schemas;

    for (size_t i = 0; i < kNofIterations; ++i) {
        M m;
        out_messages.push_back(m);
    }

    begin = std::chrono::steady_clock::now();
    std::string dummy_str("a", sizes_for_scatter[0][1]);
    for (size_t i = 0; i < kNofIterations; ++i) {
        begin = std::chrono::steady_clock::now();
        // <------------ STRING SETTERS ------>
        end = std::chrono::steady_clock::now();

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        allocation_durations.push_back(duration);
    }

    for (size_t i = 0; i < kNofIterations; ++i) {
        ScatterGather::Schema scatter_schema;
        scatter_schema.reserve(SCATTER_GATHER_BUFFER_SIZE);

        begin = std::chrono::steady_clock::now();
        // <------------ SCATTER SCHEMA ------>
        //scagatherer.UpdateScatterSchema(scatter_schema, sizes_for_scatter[i]);
        end = std::chrono::steady_clock::now();
        scatter_schemas.push_back(scatter_schema);

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        scatter_schema_durations.push_back(duration);
    }

    // decompress+scatter
    for (size_t i = 0; i < messages.size(); ++i) {
        begin = std::chrono::steady_clock::now();
        auto outcome = scagatherer.ScatterWithMemCpy(gather_outs[i].data(), scatter_schemas[i]);
        end = std::chrono::steady_clock::now();

        if (outcome) {
            std::cout << "Failed to scatter" << std::endl;
            return -1;
        }

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        scatter_durations.push_back(duration);
    }

    // Verify correctness
    bool all_correct = true;
    for (size_t i = 0; i < kNofIterations && all_correct; ++i) {
        all_correct = google::protobuf::util::MessageDifferencer::Equivalent(messages[i], out_messages[i]);
    }
    std::cout << (all_correct ? "ALL CORRECT" : "ERROR: DATA MISSMATCH") << std::endl;
    assert(all_correct);

    report_timings(serialization_durations, "serialization");
    report_timings(deserialization_durations, "deserialization");
    report_timings(gather_schema_durations, "gather_schemas");
    report_timings(gather_durations, "gather");
    report_timings(compression_durations, "compression");
    report_timings(scatter_schema_durations, "scatter_schemas");
    report_timings(scatter_durations, "scatter");
    report_timings(decompression_durations, "decompression");
    report_timings(allocation_durations, "allocation");

    return 0;
}
