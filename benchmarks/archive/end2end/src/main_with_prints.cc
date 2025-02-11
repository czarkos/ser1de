#include <google/protobuf/util/message_differencer.h>

#include"person.pb.h"
#include"scatter_gather.h"
#include"iaa_comp.h"

static constexpr size_t kNofIterations = 2;
static constexpr size_t kNofWarmUpIterations = 1;

#define BUFFER_SIZE 4096

int main () {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // initialize iaa jobs
	//iaa_init_jobs(qpl_path_hardware);
	iaa_init_jobs(qpl_path_software);

    std::vector<M> messages;
    //messages.reserve(kNofIterations);
    std::vector<std::string> ser_outs;
    // IAA utility arrays
    uint8_t** compressed   = new uint8_t*[kNofIterations];
    uint8_t** decompressed = new uint8_t*[kNofIterations];
    for (size_t i = 0; i < kNofIterations; ++i) {
        compressed[i]   = new uint8_t[BUFFER_SIZE];
        decompressed[i] = new uint8_t[BUFFER_SIZE];
    }
    // arrays for keeping output size feedback from IAA
    uint32_t* comprOutputSize   = new uint32_t[kNofIterations];
    uint32_t* decomprOutputSize = new uint32_t[kNofIterations];

    for (size_t i = 0; i < kNofIterations; ++i) {
        M m;

        // <------------ SETTERS ------>
		(&m)->set_f1(10956065 + (i * 1) % 8);
		(&m)->set_f2(128 + (i * 1) % 8);
		(&m)->set_f3("JRM2Q90FF3");
		(&m)->set_f4("VBVWLJ31BO");

        //messages[i] = m;
        messages.push_back(m);
        ser_outs.push_back("");
    }

    // Gather schemas
    std::vector<ScatterGather::Schema> gather_schemas;
    std::vector<std::vector<size_t>> sizes_for_scatter;
    for (size_t i = 0; i < kNofIterations; ++i) {
        ScatterGather::Schema gather_schema;
        std::vector<size_t> sizes;

        // <------------ GATHER SCHEMA ------>
	messages[i].generate_schema(gather_schema);
    std::cout << "printing gather_schema:\n";
        std::cout << static_cast<void*>(std::get<0>(gather_schema[0])) << ": ";
        std::cout << *((int*)std::get<0>(gather_schema[0])) << ", ";
        std::cout << *((int*)(std::get<0>(gather_schema[0])+4)) << ", ";
        std::cout <<  std::get<1>(gather_schema[0]) << "\n";
        std::cout << static_cast<void*>(std::get<0>(gather_schema[1])) << " " << std::get<1>(gather_schema[1]) << "\n";
        std::cout << static_cast<void*>(std::get<0>(gather_schema[2])) << " " << std::get<1>(gather_schema[2]) << "\n";
        std::cout << "-----------------\n";

    messages[i].generate_scatter_sizes(sizes);
        gather_schemas.push_back(gather_schema);
        sizes_for_scatter.push_back(sizes);
    }

    //
    // Benchmark serialize.
    //
    // Warm-up.
    for (size_t i = 0; i < kNofWarmUpIterations; ++i) {
        if (!messages[i].SerializeToString(&ser_outs[i])) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (size_t i = kNofWarmUpIterations; i < messages.size(); ++i) {
        if (!messages[i].SerializeToString(&ser_outs[i])) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto took_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
    std::cout << "Serialization took = " << took_ns / (kNofIterations - kNofWarmUpIterations) << " [ns], size = " << ser_outs[0].size() << " Bytes" << std::endl;

    //
    // Benchmark deserialize.
    //
    std::vector<M> deser_messages_out;
    for (size_t i = 0; i < messages.size(); ++i) deser_messages_out.push_back(M());

    // Warm-up.
    for (size_t i = 0; i < kNofWarmUpIterations; ++i) {
        if (!deser_messages_out[i].ParseFromString(ser_outs[i])) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }
    }

    begin = std::chrono::steady_clock::now();
    for (size_t i = kNofWarmUpIterations; i < messages.size(); ++i) {
        if (!deser_messages_out[i].ParseFromString(ser_outs[i])) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }
    }
    end = std::chrono::steady_clock::now();
    took_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
    std::cout << "Deserialization took = " << took_ns / (messages.size() - kNofWarmUpIterations) << " [ns]" << std::endl;

    // GATHER + COMPRESSION

    ScatterGather scagatherer;
    size_t out_size = 0;
    std::vector<std::vector<uint8_t>> gather_outs(kNofIterations, std::vector<uint8_t>(BUFFER_SIZE, 0));

    // warmup for both
    for (size_t i = 0; i < kNofWarmUpIterations; ++i) {
        if (scagatherer.GatherWithMemCpy(gather_schemas[i], gather_outs[i].data(), &out_size) ) {
            std::cerr << "Failed to gather" << std::endl;
            return -1;
        }
        if (compress_with_IAA(gather_outs[i].data(), out_size, compressed[i], BUFFER_SIZE, &comprOutputSize[i])) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }
    }

    // gather
    begin = std::chrono::steady_clock::now();
    for (size_t i = kNofWarmUpIterations; i < kNofIterations; ++i) {
        if (scagatherer.GatherWithMemCpy(gather_schemas[i], gather_outs[i].data(), &out_size) ) {
            std::cerr << "Failed to gather" << std::endl;
            return -1;
        }
    /*
    }
    end = std::chrono::steady_clock::now();
    took_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
    std::cout << "Gathering took : " << took_ns / (kNofIterations - kNofWarmUpIterations) << " nanoseconds" << std::endl;

    // compress
    begin = std::chrono::steady_clock::now();
    for (size_t i = kNofWarmUpIterations; i < kNofIterations; ++i) {
    */
        if (compress_with_IAA(gather_outs[i].data(), out_size, compressed[i], BUFFER_SIZE, &comprOutputSize[i])) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }
    }
    end = std::chrono::steady_clock::now();
    took_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
    std::cout << "Gather+Compressing took : " << took_ns / (kNofIterations - kNofWarmUpIterations) << " nanoseconds, ";

    std::cout << "Gathered size : " << out_size << " bytes, ";
    std::cout << "Compressed size : " << comprOutputSize[0] << " bytes" << std::endl;

    // DECOMPRESSION + SCATTER

    // create scatter schemas and output messages
    // output messages after decompression and scatter
    std::vector<M> out_messages;
    //out_messages.reserve(kNofIterations);
    // Scatter schemas
    std::vector<ScatterGather::Schema> scatter_schemas;
    for (size_t i = 0; i < kNofIterations; ++i) {
        M m;
        out_messages.push_back(m);
    }

    for (size_t i = 0; i < kNofIterations; ++i) {
        ScatterGather::Schema scatter_schema;

        // <------------ SCATTER SCHEMA ------>
        out_messages[i].set_f3("alskdjfhsalkdjhf");
	    out_messages[i].generate_scatter_schema(scatter_schema, sizes_for_scatter[i]);
        //scagatherer.UpdateScatterSchema(scatter_schema, sizes_for_scatter[i]);
        std::cout << static_cast<void*>(std::get<0>(scatter_schema[0])) << ": ";
        std::cout << *(std::get<0>(scatter_schema[0])) << ", ";
        std::cout <<  std::get<1>(scatter_schema[0]) << "\n";
        std::cout << static_cast<void*>(std::get<0>(scatter_schema[1])) << " " << std::get<1>(scatter_schema[1]) << "\n";
        std::cout << static_cast<void*>(std::get<0>(scatter_schema[2])) << " " << std::get<1>(scatter_schema[2]) << "\n";
        std::cout << "-----------------\n";
        scatter_schemas.push_back(scatter_schema);
    }
    //exit(1);

    // warmup decompress+scatter
    for (size_t i = 0; i < kNofWarmUpIterations; ++i) {
        if (decompress_with_IAA(compressed[i], comprOutputSize[i], decompressed[i], BUFFER_SIZE, &decomprOutputSize[i])) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }
        if (scagatherer.ScatterWithMemCpy(decompressed[i], scatter_schemas[i])) {
            std::cout << "Failed to scatter" << std::endl;
            return -1;
        }
    }

    begin = std::chrono::steady_clock::now();
    for (size_t i = kNofWarmUpIterations; i < messages.size(); ++i) {
        if (decompress_with_IAA(compressed[i], comprOutputSize[i], decompressed[i], BUFFER_SIZE, &decomprOutputSize[i])) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }
        if (scagatherer.ScatterWithMemCpy(decompressed[i], scatter_schemas[i])) {
            std::cout << "Failed to scatter" << std::endl;
            return -1;
        }
    }
    end = std::chrono::steady_clock::now();
    took_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
    std::cout << "Decompression+Scatter took = " << took_ns / (kNofIterations - kNofWarmUpIterations) << " [ns], size = " << decomprOutputSize[0] << " Bytes" << std::endl;

    // Verify correctness
    /*
    */
    bool all_correct = true;

    for (size_t i = 0; i < kNofIterations && all_correct; ++i) {
        all_correct = google::protobuf::util::MessageDifferencer::Equivalent(messages[i], out_messages[i]);
        std::cout << messages[0].f3() << "  " << out_messages[0].f3() << "\n";
        std::cout << messages[0].f4() << "  " << out_messages[0].f4() << "\n";
    }

    std::cout << (all_correct ? "ALL CORRECT" : "ERROR: DATA MISSMATCH") << std::endl;

    /*
    */
    // Optional:  Delete all global objects allocated by libprotobuf.
    //google::protobuf::ShutdownProtobufLibrary();

    for (size_t i = 0; i < kNofIterations; ++i) {
        delete[] compressed[i];
        delete[] decompressed[i];
    }
    delete[] compressed;
    delete[] decompressed;

    // free existing iaa jobs
	iaa_fini_jobs();

    return 0;
}
