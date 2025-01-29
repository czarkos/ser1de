#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include <cstring>
#include <numeric>
#include <immintrin.h> // Include for _mm_cldemote
#include "scatter_gather/scatter_gather.h"

const int NUM_ITERATIONS = 1000;
const size_t BUFFER_SIZE = 2048;

int main() {
    using Schema = std::vector<std::tuple<uint8_t*, size_t>>;

    // Initialize data
    int int_data1[NUM_ITERATIONS][50];
    int int_data2[NUM_ITERATIONS][50];
    int int_data3[NUM_ITERATIONS][50];
    int int_data4[NUM_ITERATIONS][50];
    char str_data1[NUM_ITERATIONS][200];
    char str_data2[NUM_ITERATIONS][200];
    char str_data3[NUM_ITERATIONS][200];
    char str_data4[NUM_ITERATIONS][200];

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        for (int j = 0; j < 50; ++j) {
            int_data1[i][j] = j + 1;
            int_data2[i][j] = j + 51;
            int_data3[i][j] = j + 101;
            int_data4[i][j] = j + 151;
        }
        std::strcpy(str_data1[i], "This is a test string with more than fifty characters! This is a test string with more than fifty characters! This is a test string with more than fifty characters! This is a test string with more than fifty characters!");
        std::strcpy(str_data2[i], "Another test string that is also quite long and descriptive. Another test string that is also quite long and descriptive. Another test string that is also quite long and descriptive. Another test string that is also quite long and descriptive.");
        std::strcpy(str_data3[i], "Short string. Short string. Short string. Short string. Short string. Short string. Short string. Short string. Short string. Short string.");
        std::strcpy(str_data4[i], "Yet another string that is long enough to test properly. Yet another string that is long enough to test properly. Yet another string that is long enough to test properly. Yet another string that is long enough to test properly.");
    }

    // Pre-allocate the destination buffers
    uint8_t* dest_buffers[NUM_ITERATIONS];
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        dest_buffers[i] = new uint8_t[BUFFER_SIZE];
    }

    // Create new empty arrays for scattering
    int int_data1_scatter[NUM_ITERATIONS][50];
    int int_data2_scatter[NUM_ITERATIONS][50];
    int int_data3_scatter[NUM_ITERATIONS][50];
    int int_data4_scatter[NUM_ITERATIONS][50];
    char str_data1_scatter[NUM_ITERATIONS][200];
    char str_data2_scatter[NUM_ITERATIONS][200];
    char str_data3_scatter[NUM_ITERATIONS][200];
    char str_data4_scatter[NUM_ITERATIONS][200];

    std::vector<long long> durations_gather;
    std::vector<long long> durations_scatter;
    std::vector<long long> durations_schema_creation;

    // Create ScatterGather object
    ScatterGather scatterGather(DML_PATH_HW, 350);

    // Loop for gather schema creation
    std::vector<Schema> gather_schemas(NUM_ITERATIONS);
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        auto start_schema_creation = std::chrono::high_resolution_clock::now();
        Schema gather_schema;
        gather_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(int_data1[i]), sizeof(int_data1[i])));
        gather_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(int_data2[i]), sizeof(int_data2[i])));
        gather_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(int_data3[i]), sizeof(int_data3[i])));
        gather_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(int_data4[i]), sizeof(int_data4[i])));
        gather_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(str_data1[i]), sizeof(str_data1[i])));
        gather_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(str_data2[i]), sizeof(str_data2[i])));
        gather_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(str_data3[i]), sizeof(str_data3[i])));
        gather_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(str_data4[i]), sizeof(str_data4[i])));
        gather_schemas[i] = gather_schema;
        auto end_schema_creation = std::chrono::high_resolution_clock::now();
        auto duration_schema_creation = std::chrono::duration_cast<std::chrono::nanoseconds>(end_schema_creation - start_schema_creation).count();
        durations_schema_creation.push_back(duration_schema_creation);
    }

    // Loop for gathering data
    size_t out_size = 0;
    auto total_start_gather = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        auto start_gather = std::chrono::high_resolution_clock::now();
        //scatterGather.dsa_gather_blocking(gather_schemas[i], dest_buffers[i], &out_size);
        //scatterGather.dsa_gather_blocking_batching(gather_schemas[i], dest_buffers[i], &out_size);
        //scatterGather.dsa_gather_non_blocking(gather_schemas[i], dest_buffers[i], &out_size);
        //scatterGather.dsa_gather_non_blocking_batching(gather_schemas[i], dest_buffers[i], &out_size);
        scatterGather.GatherWithMemCpy(gather_schemas[i], dest_buffers[i], &out_size);
        auto end_gather = std::chrono::high_resolution_clock::now();
        auto duration_gather = std::chrono::duration_cast<std::chrono::nanoseconds>(end_gather - start_gather).count();
        durations_gather.push_back(duration_gather);
    }
    //scatterGather.wait_for_all_jobs();
    //scatterGather.wait_for_all_batch_jobs();
    auto total_end_gather = std::chrono::high_resolution_clock::now();
    auto total_duration_gather = std::chrono::duration_cast<std::chrono::nanoseconds>(total_end_gather - total_start_gather).count();

    // Loop for scatter schema creation
    std::vector<Schema> scatter_schemas(NUM_ITERATIONS);
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        auto start_schema_creation = std::chrono::high_resolution_clock::now();
        Schema scatter_schema;
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(int_data1_scatter[i]), sizeof(int_data1_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(int_data2_scatter[i]), sizeof(int_data2_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(int_data3_scatter[i]), sizeof(int_data3_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(int_data4_scatter[i]), sizeof(int_data4_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(str_data1_scatter[i]), sizeof(str_data1_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(str_data2_scatter[i]), sizeof(str_data2_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(str_data3_scatter[i]), sizeof(str_data3_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(str_data4_scatter[i]), sizeof(str_data4_scatter[i])));
        scatter_schemas[i] = scatter_schema;
        auto end_schema_creation = std::chrono::high_resolution_clock::now();
        auto duration_schema_creation = std::chrono::duration_cast<std::chrono::nanoseconds>(end_schema_creation - start_schema_creation).count();
        durations_schema_creation.push_back(duration_schema_creation);
    }

    // Loop for scattering data
    auto total_start_scatter = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        auto start_scatter = std::chrono::high_resolution_clock::now();
        scatterGather.dsa_scatter_blocking(dest_buffers[i], scatter_schemas[i]);
        //scatterGather.dsa_scatter_blocking_batching(dest_buffers[i], scatter_schemas[i]);
        //scatterGather.dsa_scatter_non_blocking(dest_buffers[i], scatter_schemas[i]);
        //scatterGather.dsa_scatter_non_blocking_batching(dest_buffers[i], scatter_schemas[i]);
        //scatterGather.ScatterWithMemCpy(dest_buffers[i], scatter_schemas[i]);
        auto end_scatter = std::chrono::high_resolution_clock::now();
        auto duration_scatter = std::chrono::duration_cast<std::chrono::nanoseconds>(end_scatter - start_scatter).count();
        durations_scatter.push_back(duration_scatter);
    }
    //scatterGather.wait_for_all_jobs();
    //scatterGather.wait_for_all_batch_jobs();
    auto total_end_scatter = std::chrono::high_resolution_clock::now();
    auto total_duration_scatter = std::chrono::duration_cast<std::chrono::nanoseconds>(total_end_scatter - total_start_scatter).count();

    // Verify the scattered data
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        assert(memcmp(int_data1[i], int_data1_scatter[i], sizeof(int_data1[i])) == 0);
        assert(memcmp(int_data2[i], int_data2_scatter[i], sizeof(int_data2[i])) == 0);
        assert(memcmp(int_data3[i], int_data3_scatter[i], sizeof(int_data3[i])) == 0);
        assert(memcmp(int_data4[i], int_data4_scatter[i], sizeof(int_data4[i])) == 0);
        assert(memcmp(str_data1[i], str_data1_scatter[i], sizeof(str_data1[i])) == 0);
        assert(memcmp(str_data2[i], str_data2_scatter[i], sizeof(str_data2[i])) == 0);
        assert(memcmp(str_data3[i], str_data3_scatter[i], sizeof(str_data3_scatter[i])) == 0);
        assert(memcmp(str_data4[i], str_data4_scatter[i], sizeof(str_data4_scatter[i])) == 0);
    }

    // Clean up
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        delete[] dest_buffers[i];
    }

    // Calculate averages
    long long average_duration_gather = std::accumulate(durations_gather.begin(), durations_gather.end(), 0LL) / NUM_ITERATIONS;
    long long average_duration_scatter = std::accumulate(durations_scatter.begin(), durations_scatter.end(), 0LL) / NUM_ITERATIONS;
    long long average_duration_schema_creation = std::accumulate(durations_schema_creation.begin(), durations_schema_creation.end(), 0LL) / (2 * NUM_ITERATIONS); // Dividing by 2 * NUM_ITERATIONS because schema creation is measured twice

    std::cout << "Average time taken to create schema: " << average_duration_schema_creation << " nanoseconds" << std::endl;
    std::cout << "Average time taken to gather data: " << average_duration_gather << " nanoseconds" << std::endl;
    std::cout << "Total Average time taken to gather data: " << total_duration_gather / NUM_ITERATIONS << " nanoseconds" << std::endl;
    std::cout << "Average time taken to scatter data: " << average_duration_scatter << " nanoseconds" << std::endl;
    std::cout << "Total Average time taken to scatter data: " << total_duration_scatter / NUM_ITERATIONS << " nanoseconds" << std::endl;
    std::cout << "Data gathered per iteration: " << out_size << " bytes" << std::endl;

    std::cout << "Data gathered and scattered successfully!" << std::endl;

    return 0;
}