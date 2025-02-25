#include <google/protobuf/util/message_differencer.h>

#include"person.pb.h"
#include"proto_init.h"
#include"iaa_comp.h"
#include"scatter_gather.h"
#include"custom_alloc.h"

#include <tuple>

static constexpr size_t kNofIterations = 1001;

#define BUFFER_SIZE 4*4096

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

inline void benchmark_ser1de_serialize(ScatterGather& scagatherer, IAAComp& iaa_comp, std::vector<M>& messages, std::vector<std::unique_ptr<uint8_t[]>>& compressed, std::vector<uint32_t>& comprOutputSize, std::vector<std::chrono::nanoseconds>& gather_schema_durations, std::vector<std::chrono::nanoseconds>& gather_durations, std::vector<std::chrono::nanoseconds>& compression_durations, std::vector<std::vector<size_t>>& sizes_for_scatter) {
    size_t BATCHING = 1;
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    // Initialize output size
    size_t out_size = 0;

    size_t queue_index = 0, job_index = 0;

    // Initialize intermediate variables
    // gather schema
    //ScatterGather::Schema gather_schema;
    //gather_schema.reserve(BUFFER_SIZE/16);
    std::vector<uint8_t*> ptrs;
    ptrs.reserve(BUFFER_SIZE/16);
    std::vector<size_t> sizes;
    sizes.reserve(BUFFER_SIZE/16);

    // gather
    std::vector<uint8_t> gather_out(BUFFER_SIZE, 0);
    for (size_t i = 0; i < kNofIterations; ++i) {
        // <------------ GATHER SCHEMA ------>
		//messages[i].generate_schema(gather_schema);
        begin = std::chrono::steady_clock::now();
		//messages[i].generate_schema(gather_schema);
        messages[i].generate_seperated_schema(ptrs, sizes);
        end = std::chrono::steady_clock::now();
        //messages[i].generate_seperated_schema(ptrs, sizes);
		//messages[i].generate_scatter_sizes(sizes);
        sizes_for_scatter.push_back(sizes);
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        gather_schema_durations.push_back(duration);
        // <------------ GATHER ------>
        begin = std::chrono::steady_clock::now();
        //scagatherer.dsa_gather_blocking_batching(gather_schema, gather_out.data(), &out_size);
        //scagatherer.GatherWithMemCpy(gather_schema, gather_out.data(), &out_size);
        scagatherer.GatherWithMemCpy(ptrs, sizes, gather_out.data(), &out_size);
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        gather_durations.push_back(duration);
        // <------------ COMPRESS ------>
        begin = std::chrono::steady_clock::now();
        /*
            if(i != 0) {
                while(iaa_comp.poll_job(queue_index, job_index) != QPL_STS_OK) {
                    // Busy-wait loop
                };
                comprOutputSize[i-1] = iaa_comp.get_job_compr_size(queue_index, job_index);
                //std::cout << "gather out " << out_size << "\n";
                //std::cout << "compressed size: " << comprOutputSize[i-1] << "\n";
                //exit(0);
            }
            if (i == kNofIterations-1) {
                //std::cout << "I AM HERE" << std::endl;
                iaa_comp.compress_blocking(gather_out.data(), out_size, compressed[i].get(), out_size+512, &comprOutputSize[i]);
            }
            else {
                std::tie(queue_index, job_index) = iaa_comp.compress_non_blocking(gather_out.data(), out_size, compressed[i].get(), out_size+512);
            }
        */
        /*
        if (BATCHING == 1) {
        }
        else {
            iaa_comp.compress_blocking(gather_out.data(), out_size, compressed[i].get(), out_size+512, &comprOutputSize[i]);
        }
        */
        iaa_comp.compress_blocking(gather_out.data(), out_size, compressed[i].get(), out_size+512, &comprOutputSize[i]);
        //iaa_comp.compress_blocking(gather_out.data(), out_size, compressed[i].get(), out_size+512, &comprOutputSize[i]);
        //std::cout << "gather out " << out_size << "\n";
        //std::cout << "compressed size: " << comprOutputSize[i] << "\n";
        //exit(0);
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        compression_durations.push_back(duration);
        ptrs.clear();
        sizes.clear();
    }
    std::cout << "gather_out(bytes), " << out_size << "\n";
}

inline void benchmark_ser1de_deserialize(ScatterGather& scagatherer, IAAComp& iaa_comp, std::vector<M>& out_messages, std::vector<std::unique_ptr<uint8_t[]>>& compressed, std::vector<uint32_t>& comprOutputSize, std::vector<uint32_t>& decomprOutputSize, std::vector<std::vector<size_t>>& sizes_for_scatter, std::vector<std::chrono::nanoseconds>& decompression_durations, std::vector<std::chrono::nanoseconds>& scatter_schema_durations, std::vector<std::chrono::nanoseconds>& scatter_durations, std::vector<std::chrono::nanoseconds>& allocation_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    // scatter
    std::vector<uint8_t> gather_out(BUFFER_SIZE, 0);
    for (size_t i = 0; i < kNofIterations; ++i) {
        // scatter schema
        ScatterGather::Schema scatter_schema;
        scatter_schema.reserve(BUFFER_SIZE/16);
        std::vector<uint8_t*> ptrs;
        ptrs.reserve(BUFFER_SIZE/16);

        // <------------ DECOMPRESS ------>
        begin = std::chrono::steady_clock::now();
        //iaa_comp.decompress_blocking(compressed[i].get(), comprOutputSize[i], gather_out.data(), gather_out.size(), &decomprOutputSize[i]);
        iaa_comp.decompress_non_blocking(compressed[i].get(), comprOutputSize[i], gather_out.data(), gather_out.size());
        //iaa_comp.decompress_blocking(compressed[i].get(), comprOutputSize[i], gather_outs[0].data(), gather_outs[0].size()+1024, &decomprOutputSize[i]);
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        decompression_durations.push_back(duration);
        // <------------ ALLOCATE ------>
        begin = std::chrono::steady_clock::now();
        //std::string dummy_str("a", sizes_for_scatter[i][1]);
        //string_generated_setters(out_messages[i], dummy_str);
        //allocate_with_sizes_hacky(out_messages[i], sizes_for_scatter[i], dummy_str);
        //out_messages[i].allocate_from_sizes(sizes_for_scatter[i]);
        out_messages[i].generate_scatter_ptrs_and_allocate_from_sizes(ptrs, sizes_for_scatter[i]);
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        allocation_durations.push_back(duration);
        // <------------ SCATTER SCHEMA ------>
        begin = std::chrono::steady_clock::now();
        //out_messages[i].generate_schema(scatter_schema);
        //out_messages[i].generate_scatter_ptrs(ptrs);
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        scatter_schema_durations.push_back(duration);
        // <------------ SCATTER ------>
        begin = std::chrono::steady_clock::now();
        //scagatherer.dsa_scatter_blocking_batching(gather_out.data(), scatter_schema);
        //scagatherer.ScatterWithMemCpy(gather_out.data(), scatter_schema);
        scagatherer.ScatterWithMemCpy(gather_out.data(), ptrs, sizes_for_scatter[i]);
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        scatter_durations.push_back(duration);
    }

}

inline void benchmark_custom_alloc(std::vector<M>& out_messages, std::vector<std::vector<size_t>>& sizes_for_scatter, std::vector<std::chrono::nanoseconds>& allocation_durations) {
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

    for (size_t i = 0; i < kNofIterations; ++i) {
        begin = std::chrono::steady_clock::now();
        // <------------ STRING SETTERS ------>
        //std::string dummy_str("a", sizes_for_scatter[i][1]);
        //string_generated_setters(out_messages[i], dummy_str);
        //allocate_with_sizes_hacky(out_messages[i], sizes_for_scatter[i], dummy_str);
        out_messages[i].allocate_from_sizes(sizes_for_scatter[i]);
        //allocateStringField(&out_messages[i], sizes_for_scatter[i][1]);
        end = std::chrono::steady_clock::now();

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        allocation_durations.push_back(duration);
    }
}

int benchmark (bool dsa) {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // initialize iaa jobs
    qpl_path_t path = qpl_path_hardware; // or QPL_PATH_SOFTWARE
    //qpl_path_t path = qpl_path_software; // or QPL_PATH_SOFTWARE
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

    //
    // Benchmark Serialize.
    //
    benchmark_serialize(messages, ser_outs, serialization_durations);
    //std::cout << "ser out size " << ser_outs[0].size() << "\n"; 

    //
    // Benchmark Deserialize.
    //
    std::vector<M> deser_messages_out;
    for (size_t i = 0; i < messages.size(); ++i) deser_messages_out.push_back(M());
    benchmark_deserialize(messages, ser_outs, deser_messages_out, deserialization_durations);

    //
    // Benchmark Ser1de Serialize.
    //
    std::vector<std::vector<size_t>> sizes_for_scatter;
    benchmark_ser1de_serialize(scagatherer, iaa_comp, messages, compressed, comprOutputSize, gather_schema_durations, gather_durations, compression_durations, sizes_for_scatter);

    //
    // Benchmark Ser1de Deserialize.
    //
    // output messages after decompression and scatter
    std::vector<M> out_messages;
    for (size_t i = 0; i < kNofIterations; ++i) {
        M m;
        out_messages.push_back(m);
    }

    // allocate space for the strings
    //std::string dummy_str("a", sizes_for_scatter[0][1]);
    /*
    for (size_t i = 0; i < kNofIterations; ++i) {
        begin = std::chrono::steady_clock::now();
        // <------------ STRING SETTERS ------>
        std::string dummy_str("a", sizes_for_scatter[i][1]);
        //string_generated_setters(out_messages[i], dummy_str);
        allocate_with_sizes_hacky(out_messages[i], sizes_for_scatter[i], dummy_str);
        end = std::chrono::steady_clock::now();

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        allocation_durations.push_back(duration);
    }
    */
    //benchmark_custom_alloc(out_messages, sizes_for_scatter, allocation_durations);
    benchmark_ser1de_deserialize(scagatherer, iaa_comp, out_messages, compressed, comprOutputSize, decomprOutputSize, sizes_for_scatter, decompression_durations, scatter_schema_durations, scatter_durations, allocation_durations);

    // Verify correctness
    bool all_correct = true;
    for (size_t i = 0; i < kNofIterations && all_correct; ++i) {
        all_correct = google::protobuf::util::MessageDifferencer::Equivalent(messages[i], out_messages[i]);
    }
    std::cout << (all_correct ? "ALL CORRECT" : "ERROR: DATA MISSMATCH") << std::endl;
    /*
    std::cout << "First message contents:" << std::endl;
    std::cout << messages[0].DebugString() << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
    std::cout << out_messages[0].DebugString() << std::endl;
    
    // Print integer fields
    std::cout << "Integer fields for first message:" << std::endl;
    std::cout << "f1: " << messages[0].f1() << " -> " << out_messages[0].f1() << std::endl;
    std::cout << "f2: " << messages[0].f2() << " -> " << out_messages[0].f2() << std::endl;
    std::cout << "f3: " << messages[0].f3() << " -> " << out_messages[0].f3() << std::endl;
    std::cout << "f4: " << messages[0].f4() << " -> " << out_messages[0].f4() << std::endl;
    
    // Print nested integer fields for first message
    if (messages[0].has_f6() && out_messages[0].has_f6()) {
        std::cout << "Nested integer fields for first message:" << std::endl;
        std::cout << "f6.f1: " << messages[0].f6().f1() << " -> " << out_messages[0].f6().f1() << std::endl;
        std::cout << "f6.f2: " << messages[0].f6().f2() << " -> " << out_messages[0].f6().f2() << std::endl;
        std::cout << "f6.f3: " << messages[0].f6().f3() << " -> " << out_messages[0].f6().f3() << std::endl;
        std::cout << "f6.f4: " << messages[0].f6().f4() << " -> " << out_messages[0].f6().f4() << std::endl;
    }
    */
    assert(all_correct);

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
    /*
    */

    //std::cout << "Average serialization time: " << std::accumulate(serialization_durations.begin(), serialization_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average deserialization time: " << std::accumulate(deserialization_durations.begin(), deserialization_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average gather schema time: " << std::accumulate(gather_schema_durations.begin(), gather_schema_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average gather time: " << std::accumulate(gather_durations.begin(), gather_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average compression time: " << std::accumulate(compression_durations.begin(), compression_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average scatter schema time: " << std::accumulate(scatter_schema_durations.begin(), scatter_schema_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average scatter time: " << std::accumulate(scatter_durations.begin(), scatter_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average decompression time: " << std::accumulate(decompression_durations.begin(), decompression_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;
    //std::cout << "Average allocation time: " << std::accumulate(allocation_durations.begin(), allocation_durations.end(), std::chrono::nanoseconds(0)).count() / kNofIterations << "ns" << std::endl;

    return 0;
}

int main () {
    //benchmark(true);
    benchmark(false);
}