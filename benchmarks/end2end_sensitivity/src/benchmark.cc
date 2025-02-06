#include <google/protobuf/util/message_differencer.h>

#include"person.pb.h"
#include"scatter_gather/scatter_gather.h"
#include"proto_init.h"
#include"iaa_comp.h"

static constexpr size_t kNofIterations = 1001;

#define BUFFER_SIZE 2*4096

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

inline void benchmark_create_gather_schema(std::vector<M>& messages, std::vector<ScatterGather::Schema>& gather_schemas, std::vector<std::vector<size_t>>& sizes_for_scatter, std::vector<std::chrono::nanoseconds>& gather_schema_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < kNofIterations; ++i) {
        ScatterGather::Schema gather_schema;
        gather_schema.reserve(BUFFER_SIZE/16);
        std::vector<size_t> sizes;
        sizes.reserve(BUFFER_SIZE/16);

        begin = std::chrono::steady_clock::now();
        // <------------ GATHER SCHEMA ------>
		messages[i].generate_schema(gather_schema);
        end = std::chrono::steady_clock::now();
		messages[i].generate_scatter_sizes(sizes);
        gather_schemas.push_back(gather_schema);
        sizes_for_scatter.push_back(sizes);

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        gather_schema_durations.push_back(duration);
    }
}

inline void benchmark_create_scatter_schema(std::vector<M>& out_messages, std::vector<ScatterGather::Schema>& scatter_schemas, std::vector<std::chrono::nanoseconds>& scatter_schema_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < kNofIterations; ++i) {
        ScatterGather::Schema scatter_schema;
        scatter_schema.reserve(BUFFER_SIZE/16);

        begin = std::chrono::steady_clock::now();
        // <------------ SCATTER SCHEMA ------>
		out_messages[i].generate_schema(scatter_schema);
        //scagatherer.UpdateScatterSchema(scatter_schema, sizes_for_scatter[i]);
        end = std::chrono::steady_clock::now();
        scatter_schemas.push_back(scatter_schema);

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        scatter_schema_durations.push_back(duration);
    }
}

inline void benchmark_gather(ScatterGather& scagatherer, std::vector<ScatterGather::Schema>& gather_schemas, std::vector<std::vector<uint8_t>>& gather_outs, bool dsa, std::vector<std::chrono::nanoseconds>& gather_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;
    size_t out_size = 0;

    begin = std::chrono::steady_clock::now();
    for (size_t i = 0; i < kNofIterations; ++i) {
        if (dsa) {
            scagatherer.dsa_gather_blocking_batching(gather_schemas[i], gather_outs[i].data(), &out_size);
        }
        else {
            scagatherer.GatherWithMemCpy(gather_schemas[i], gather_outs[i].data(), &out_size);
        }
        //scagatherer.dsa_gather_blocking(gather_schemas[i], gather_outs[0].data(), &out_size);
    }
    //if (dsa)
        //scagatherer.wait_for_all_jobs();
    end = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin) / kNofIterations;
    gather_durations.push_back(duration);

    std::cout << "gather_out(bytes), " << out_size << "\n";
}

inline void benchmark_scatter(ScatterGather& scagatherer, std::vector<ScatterGather::Schema>& scatter_schemas, std::vector<std::vector<uint8_t>>& gather_outs, bool dsa, std::vector<std::chrono::nanoseconds>& scatter_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    begin = std::chrono::steady_clock::now();
    for (size_t i = 0; i < kNofIterations; ++i) {
         if (dsa) {
            //scagatherer.dsa_scatter_non_blocking(gather_outs[i].data(), scatter_schemas[i]);
            scagatherer.dsa_scatter_blocking_batching(gather_outs[i].data(), scatter_schemas[i]);
         }
         else {
            scagatherer.ScatterWithMemCpy(gather_outs[i].data(), scatter_schemas[i]);
         }
         //scagatherer.ScatterWithMemCpy(decompressed[0], scatter_schemas[i]);
         //scagatherer.dsa_scatter_non_blocking(decompressed[0], scatter_schemas[i]);
    }
    //if (dsa)
        //scagatherer.wait_for_all_jobs();
    end = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin) / kNofIterations;
    scatter_durations.push_back(duration);
}

inline void benchmark_compress(IAAComp& iaa_comp, std::vector<std::vector<uint8_t>>& gather_outs, std::vector<std::unique_ptr<uint8_t[]>>& compressed, std::vector<uint32_t>& comprOutputSize, std::vector<std::chrono::nanoseconds>& compression_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < kNofIterations; ++i) {
        begin = std::chrono::steady_clock::now();
        iaa_comp.compress_blocking(gather_outs[i].data(), gather_outs[i].size(), compressed[i].get(), BUFFER_SIZE, &comprOutputSize[i]);
        //iaa_comp.compress_blocking(gather_outs[0].data(), gather_outs[0].size(), compressed[i].get(), gather_outs[0].size()+1024, &comprOutputSize[i]);
        end = std::chrono::steady_clock::now();

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        compression_durations.push_back(duration);
    }

    std::cout << "compress_out(bytes), " << comprOutputSize[0] << "\n";
}

inline void benchmark_decompress(IAAComp& iaa_comp, std::vector<std::unique_ptr<uint8_t[]>>& compressed, std::vector<uint32_t>& comprOutputSize, std::vector<std::vector<uint8_t>>& gather_outs, std::vector<uint32_t>& decomprOutputSize, std::vector<std::chrono::nanoseconds>& decompression_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < kNofIterations; ++i) {
        begin = std::chrono::steady_clock::now();
        iaa_comp.decompress_blocking(compressed[i].get(), comprOutputSize[i], gather_outs[i].data(), gather_outs[i].size(), &decomprOutputSize[i]);
        //iaa_comp.decompress_blocking(compressed[i].get(), comprOutputSize[i], gather_outs[0].data(), gather_outs[0].size()+1024, &decomprOutputSize[i]);
        end = std::chrono::steady_clock::now();

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        decompression_durations.push_back(duration);
    }

    std::cout << "decompress_out(bytes), " << decomprOutputSize[0] << "\n";
}

int benchmark (bool dsa) {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // initialize iaa jobs
    qpl_path_t path = qpl_path_hardware; // or QPL_PATH_SOFTWARE
    size_t iaa_num_queues = 1;
    size_t iaa_max_jobs_per_queue = 16;
    IAAComp iaa_comp(path, iaa_num_queues, iaa_max_jobs_per_queue);
    // Initialize DSA
    dml_path_t dml_path = DML_PATH_HW;
    size_t dsa_num_jobs = 350;
    ScatterGather scagatherer(dml_path, dsa_num_jobs);

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

    /*
    */
    // IAA utility arrays
    std::vector<std::unique_ptr<uint8_t[]>> compressed(kNofIterations);
    std::vector<std::unique_ptr<uint8_t[]>> decompressed(kNofIterations);
    for (size_t i = 0; i < kNofIterations; ++i) {
        compressed[i] = std::make_unique<uint8_t[]>(BUFFER_SIZE);
        decompressed[i] = std::make_unique<uint8_t[]>(BUFFER_SIZE);
    }

    // arrays for keeping output size feedback from IAA
    std::vector<uint32_t> comprOutputSize(kNofIterations);
    std::vector<uint32_t> decomprOutputSize(kNofIterations);

    for (size_t i = 0; i < kNofIterations; ++i) {
        M m;

        // populate the message
        person_generated_setters(m, i);

        messages.push_back(m);
        ser_outs.push_back("");
    }

    /*
    */
    //
    // Benchmark Serialize.
    //
    benchmark_serialize(messages, ser_outs, serialization_durations);

    //
    // Benchmark Deserialize.
    //
    std::vector<M> deser_messages_out;
    for (size_t i = 0; i < messages.size(); ++i) deser_messages_out.push_back(M());
    benchmark_deserialize(messages, ser_outs, deser_messages_out, deserialization_durations);

    //
    // Benchmark Gather Schemas
    //
    std::vector<ScatterGather::Schema> gather_schemas;
    gather_schemas.reserve(kNofIterations); 
    std::vector<std::vector<size_t>> sizes_for_scatter;
    sizes_for_scatter.reserve(kNofIterations);
    benchmark_create_gather_schema(messages, gather_schemas, sizes_for_scatter, gather_schema_durations);


    //
    // Benchmark Gather.
    //
    std::vector<std::vector<uint8_t>> gather_outs(kNofIterations, std::vector<uint8_t>(BUFFER_SIZE, 0));
    benchmark_gather(scagatherer, gather_schemas, gather_outs, dsa, gather_durations);

    //
    // Benchmark Compress.
    //
    benchmark_compress(iaa_comp, gather_outs, compressed, comprOutputSize, compression_durations);

    // DECOMPRESSION + SCATTER

    // create scatter schemas and output messages
    // output messages after decompression and scatter
    std::vector<M> out_messages;
    for (size_t i = 0; i < kNofIterations; ++i) {
        M m;
        out_messages.push_back(m);
    }

    // allocate space for the strings
    begin = std::chrono::steady_clock::now();
    std::string dummy_str("a", sizes_for_scatter[0][1]);
    for (size_t i = 0; i < kNofIterations; ++i) {
        begin = std::chrono::steady_clock::now();
        // <------------ STRING SETTERS ------>
        string_generated_setters(out_messages[i], dummy_str);
        end = std::chrono::steady_clock::now();

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        allocation_durations.push_back(duration);
    }

    //
    // Benchmark Create Scatter Schema.
    //
    std::vector<ScatterGather::Schema> scatter_schemas;
    benchmark_create_scatter_schema(out_messages, scatter_schemas, scatter_schema_durations);

    // decompress+scatter
    //
    // Benchmark Decompress.
    //
    benchmark_decompress(iaa_comp, compressed, comprOutputSize, gather_outs, decomprOutputSize, decompression_durations);

    //
    // Benchmark Scatter.
    //
    benchmark_scatter(scagatherer, scatter_schemas, gather_outs, dsa, scatter_durations);

    // Verify correctness
    bool all_correct = true;
    for (size_t i = 0; i < kNofIterations && all_correct; ++i) {
        all_correct = google::protobuf::util::MessageDifferencer::Equivalent(messages[i], out_messages[i]);
    }
    std::cout << (all_correct ? "ALL CORRECT" : "ERROR: DATA MISSMATCH") << std::endl;
    //assert(all_correct);

    if (dsa) {
        report_timings(serialization_durations, "serialization");
        report_timings(deserialization_durations, "deserialization");
        report_timings(compression_durations, "compression");
        report_timings(decompression_durations, "decompression");
        report_timings(gather_schema_durations, "gather_schemas");
        report_timings(gather_durations, "dsa_gather");
        report_timings(scatter_schema_durations, "scatter_schemas");
        report_timings(scatter_durations, "dsa_scatter");
        report_timings(allocation_durations, "allocation");
    }
    else {
        report_timings(serialization_durations, "serialization");
        report_timings(deserialization_durations, "deserialization");
        report_timings(compression_durations, "compression");
        report_timings(decompression_durations, "decompression");
        report_timings(gather_schema_durations, "gather_schemas");
        report_timings(gather_durations, "dsa_gather");
        report_timings(scatter_schema_durations, "scatter_schemas");
        report_timings(scatter_durations, "dsa_scatter");
        report_timings(allocation_durations, "allocation");
        //report_timings(gather_durations, "memcpy_gather");
        //report_timings(scatter_durations, "memcpy_scatter");
    }

    //std::cout << "Average serialization time: " << std::accumulate(serialization_durations.begin(), serialization_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average deserialization time: " << std::accumulate(deserialization_durations.begin(), deserialization_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average gather schema time: " << std::accumulate(gather_schema_durations.begin(), gather_schema_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average gather time: " << std::accumulate(gather_durations.begin(), gather_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average compression time: " << std::accumulate(compression_durations.begin(), compression_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average scatter schema time: " << std::accumulate(scatter_schema_durations.begin(), scatter_schema_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average scatter time: " << std::accumulate(scatter_durations.begin(), scatter_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average decompression time: " << std::accumulate(decompression_durations.begin(), decompression_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average allocation time: " << std::accumulate(allocation_durations.begin(), allocation_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;



    // Optional:  Delete all global objects allocated by libprotobuf.
    //google::protobuf::ShutdownProtobufLibrary();

    /*
    for (size_t i = 0; i < kNofIterations; ++i) {
        delete[] compressed[i];
        delete[] decompressed[i];
    }
    delete[] compressed;
    delete[] decompressed;
    */

    return 0;
}

int main () {
    //benchmark(true);
    benchmark(false);
}