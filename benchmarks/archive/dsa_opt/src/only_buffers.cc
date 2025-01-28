#include <random>
#include <google/protobuf/util/message_differencer.h>

#include"person.pb.h"
#include"scatter_gather.h"
#include"no_class_iaa_comp.h"

static constexpr size_t kNofIterations = 100001;

#define ACTUAL_BUFFER_SIZE 512
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

    // initialize iaa jobs
	iaa_init_jobs(qpl_path_hardware);
	//iaa_init_jobs(qpl_path_software);

    // vectors and time structs for holding performance numbers
    std::vector<std::chrono::nanoseconds> serialization_durations, deserialization_durations;
    std::vector<std::chrono::nanoseconds> gather_durations, scatter_durations;
    std::vector<std::chrono::nanoseconds> gather_schema_durations, scatter_schema_durations;
    std::vector<std::chrono::nanoseconds> compression_durations, decompression_durations;
    std::vector<std::chrono::nanoseconds> allocation_durations;
    std::chrono::steady_clock::time_point begin, end;
    std::chrono::nanoseconds duration;

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

    // Random number generation setup
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> length_dist(1, ACTUAL_BUFFER_SIZE/2);
    std::uniform_int_distribution<> char_dist(32, 126); // Printable ASCII characters

    std::vector<size_t> sizes;
    std::vector<std::vector<uint8_t>> gather_outs(kNofIterations);
    for (size_t i = 0; i < kNofIterations; ++i) {
        size_t random_length = length_dist(gen);
        sizes.push_back(random_length);
        gather_outs[i].resize(random_length);
        for (size_t j = 0; j < random_length; ++j) {
            gather_outs[i][j] = static_cast<uint8_t>(char_dist(gen));
        }
    }

    // COMPRESSION
    for (size_t i = 0; i < kNofIterations; ++i) {
        serialization_durations.push_back(std::chrono::nanoseconds(0));
        deserialization_durations.push_back(std::chrono::nanoseconds(0));
        gather_durations.push_back(std::chrono::nanoseconds(0));

        begin = std::chrono::steady_clock::now();
        auto outcome = compress_with_IAA(gather_outs[i].data(), sizes[i], compressed[i], BUFFER_SIZE, &comprOutputSize[i]);
        end = std::chrono::steady_clock::now();

        if (outcome) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        compression_durations.push_back(duration);
    }
    std::cout << "gather_out(bytes), " << sizes[0] << "\n";
    std::cout << "compress_out(bytes), " << comprOutputSize[0] << "\n";

    // DECOMPRESSION
    for (size_t i = 0; i < kNofIterations; ++i) {
        begin = std::chrono::steady_clock::now();
        auto outcome = decompress_with_IAA(compressed[i], comprOutputSize[i], decompressed[i], BUFFER_SIZE, &decomprOutputSize[i]);
        end = std::chrono::steady_clock::now();

        if (outcome) {
            std::cerr << "Benchmark error." << std::endl;
            return -1;
        }

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        decompression_durations.push_back(duration);

        allocation_durations.push_back(std::chrono::nanoseconds(0));
        scatter_schema_durations.push_back(std::chrono::nanoseconds(0));
        scatter_durations.push_back(std::chrono::nanoseconds(0));
    }
    std::cout << "decompress_out(bytes), " << decomprOutputSize[0] << "\n";

    // Verify correctness
    bool all_correct = true;
    for (size_t i = 0; i < kNofIterations && all_correct; ++i) {
        for (size_t j = 0; j < sizes[i]; ++j) {
            if (gather_outs[i][j] != decompressed[i][j]) {
                std::cerr << "Mismatch at index " << j << " of iteration " << i << std::endl;
                all_correct = false;
                break;
            }
        }
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
