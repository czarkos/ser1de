#include <google/protobuf/util/message_differencer.h>

#include"person.pb.h"
#include"scatter_gather.h"
#include"no_class_iaa_comp.h"

static constexpr size_t kNofIterations = 101;

//#define BUFFER_SIZE 4*4096
#define SCHEMA_BUFFER_SIZE 4096/4

void report_timings(std::vector<std::chrono::nanoseconds> perfs, std::string stat) {
    std::cout << stat << "(ns)";
    for (auto perf: perfs) std::cout << ", " << perf.count();
    std::cout << "\n";
}

int main () {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // initialize iaa jobs
	iaa_init_jobs(qpl_path_hardware);
	//iaa_init_jobs(qpl_path_software);

    std::vector<M> messages;
    std::vector<std::string> ser_outs;


    // vectors and time structs for holding performance numbers
    std::vector<std::chrono::nanoseconds> serialization_durations, deserialization_durations;
    std::vector<std::chrono::nanoseconds> gather_durations, scatter_durations;
    std::vector<std::chrono::nanoseconds> gather_schema_durations, scatter_schema_durations;
    std::vector<std::chrono::nanoseconds> compression_durations, decompression_durations;
    std::vector<std::chrono::nanoseconds> allocation_durations;
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < kNofIterations; ++i) {
        M m;

        // <------------ SETTERS ------>

        //messages[i] = m;
        messages.push_back(m);
        ser_outs.push_back("");
    }

    // Gather schemas
    std::vector<ScatterGather::Schema> gather_schemas;
    std::vector<std::vector<size_t>> sizes_for_scatter;
    for (size_t i = 0; i < kNofIterations; ++i) {
        ScatterGather::Schema gather_schema;
        gather_schema.reserve(SCHEMA_BUFFER_SIZE);
        std::vector<size_t> sizes;
        sizes.reserve(SCHEMA_BUFFER_SIZE);

        begin = std::chrono::steady_clock::now();
		messages[i].generate_schema(gather_schema);
        end = std::chrono::steady_clock::now();
		messages[i].generate_scatter_sizes(sizes);
        gather_schemas.push_back(gather_schema);
        sizes_for_scatter.push_back(sizes);

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        gather_schema_durations.push_back(duration);
    }

    //
    // Benchmark serialize.
    //
    for (size_t i = 0; i < messages.size(); ++i) {
        begin = std::chrono::steady_clock::now();
        auto outcome = messages[i].SerializeToString(&ser_outs[i]);
        end = std::chrono::steady_clock::now();

        if (!outcome) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        serialization_durations.push_back(duration);
    }

    //
    // Benchmark deserialize.
    //
    std::vector<M> deser_messages_out;
    for (size_t i = 0; i < messages.size(); ++i) deser_messages_out.push_back(M());

    for (size_t i = 0; i < messages.size(); ++i) {
        begin = std::chrono::steady_clock::now();
        auto outcome = deser_messages_out[i].ParseFromString(ser_outs[i]);
        end = std::chrono::steady_clock::now();

        if (!outcome) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        deserialization_durations.push_back(duration);
    }

    // IAA utility arrays
    size_t log_buffer_size = ceil(log2(ser_outs[0].size()))+1;
    size_t BUFFER_SIZE = 1 << log_buffer_size;
    uint8_t* compressed   = new uint8_t[1 << log_buffer_size];
    uint8_t* decompressed = new uint8_t[1 << log_buffer_size];
    // arrays for keeping output size feedback from IAA
    uint32_t comprOutputSize;
    uint32_t decomprOutputSize;



    // GATHER + COMPRESSION
    ScatterGather scagatherer;
    size_t out_size = 0;
    //std::vector<std::vector<uint8_t>> gather_outs(kNofIterations, std::vector<uint8_t>(BUFFER_SIZE, 0));
    std::vector<uint8_t> gather_outs(1 << log_buffer_size, 0);

    // gather + compression
    for (size_t i = 0; i < kNofIterations; ++i) {
         begin = std::chrono::steady_clock::now();
         //auto outcome = scagatherer.GatherWithMemCpy(gather_schemas[i], gather_outs[i].data(), &out_size);
         auto outcome = scagatherer.GatherWithMemCpy(gather_schemas[i], gather_outs.data(), &out_size);
         end = std::chrono::steady_clock::now();

        if (outcome) {
            std::cerr << "Failed to gather" << std::endl;
            return -1;
        }

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        gather_durations.push_back(duration);

        begin = std::chrono::steady_clock::now();
        //outcome = compress_with_IAA(gather_outs[i].data(), out_size, compressed[i], BUFFER_SIZE, &comprOutputSize[i]);
        outcome = compress_with_IAA(gather_outs.data(), out_size, compressed, BUFFER_SIZE, &comprOutputSize);
        end = std::chrono::steady_clock::now();

        if (outcome) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        compression_durations.push_back(duration);
    }
    std::cout << "gather_out(bytes), " << out_size << "\n";
    std::cout << "compress_out(bytes), " << comprOutputSize << "\n";

    // DECOMPRESSION + SCATTER

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
        scatter_schema.reserve(SCHEMA_BUFFER_SIZE);

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
        //auto outcome = decompress_with_IAA(compressed[i], comprOutputSize[i], decompressed[i], BUFFER_SIZE, &decomprOutputSize[i]);
        auto outcome = decompress_with_IAA(compressed, comprOutputSize, decompressed, BUFFER_SIZE, &decomprOutputSize);
        end = std::chrono::steady_clock::now();

        if (outcome) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        decompression_durations.push_back(duration);

        begin = std::chrono::steady_clock::now();
        //outcome = scagatherer.ScatterWithMemCpy(decompressed[i], scatter_schemas[i]);
        outcome = scagatherer.ScatterWithMemCpy(decompressed, scatter_schemas[i]);
        end = std::chrono::steady_clock::now();

        if (outcome) {
            std::cout << "Failed to scatter" << std::endl;
            return -1;
        }

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        scatter_durations.push_back(duration);
    }
    std::cout << "decompress_out(bytes), " << decomprOutputSize << "\n";
    /*
    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        assert(decompressed[i] == gather_outs[i]);
    }
    */

    // Verify correctness
    bool all_correct = true;
    for (size_t i = 0; i < kNofIterations && all_correct; ++i) {
        all_correct = google::protobuf::util::MessageDifferencer::Equivalent(messages[i], out_messages[i]);
    }
    std::cout << (all_correct ? "ALL CORRECT" : "ERROR: DATA MISSMATCH") << std::endl;

    report_timings(serialization_durations, "serialization");
    report_timings(deserialization_durations, "deserialization");
    report_timings(gather_schema_durations, "gather_schemas");
    report_timings(gather_durations, "gather");
    report_timings(compression_durations, "compression");
    report_timings(scatter_schema_durations, "scatter_schemas");
    report_timings(scatter_durations, "scatter");
    report_timings(decompression_durations, "decompression");
    report_timings(allocation_durations, "allocation");

    // Optional:  Delete all global objects allocated by libprotobuf.
    //google::protobuf::ShutdownProtobufLibrary();

    delete[] compressed;
    delete[] decompressed;

    // free existing iaa jobs
	iaa_fini_jobs();

    return 0;
}
