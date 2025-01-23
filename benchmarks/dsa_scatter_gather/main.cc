#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include <cstring>
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

    long long total_duration_gather = 0;
    long long total_duration_scatter = 0;
    long long total_duration_schema_creation = 0;
    size_t total_data_gathered = 0;

    // Create ScatterGather object
    ScatterGather scatterGather(DML_PATH_HW);

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // Create gather schema
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
        auto end_schema_creation = std::chrono::high_resolution_clock::now();
        auto duration_schema_creation = std::chrono::duration_cast<std::chrono::nanoseconds>(end_schema_creation - start_schema_creation).count();
        total_duration_schema_creation += duration_schema_creation;

        // Measure the time taken to gather data
        auto start_gather = std::chrono::high_resolution_clock::now();
        size_t out_size = 0;
        scatterGather.dsa_gather_blocking(gather_schema, dest_buffers[i], &out_size);
        //scatterGather.GatherWithMemCpy(gather_schema, dest_buffers[i], &out_size);
        auto end_gather = std::chrono::high_resolution_clock::now();
        auto duration_gather = std::chrono::duration_cast<std::chrono::nanoseconds>(end_gather - start_gather).count();
        total_duration_gather += duration_gather;
        total_data_gathered += out_size;

        // Create scatter schema
        Schema scatter_schema;
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(int_data1_scatter[i]), sizeof(int_data1_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(int_data2_scatter[i]), sizeof(int_data2_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(int_data3_scatter[i]), sizeof(int_data3_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(int_data4_scatter[i]), sizeof(int_data4_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(str_data1_scatter[i]), sizeof(str_data1_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(str_data2_scatter[i]), sizeof(str_data2_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(str_data3_scatter[i]), sizeof(str_data3_scatter[i])));
        scatter_schema.push_back(std::make_tuple(reinterpret_cast<uint8_t*>(str_data4_scatter[i]), sizeof(str_data4_scatter[i])));

        // Measure the time taken to scatter data
        auto start_scatter = std::chrono::high_resolution_clock::now();
        //scatterGather.ScatterWithMemCpy(dest_buffers[i], scatter_schema);
        scatterGather.dsa_scatter_blocking(dest_buffers[i], scatter_schema);
        auto end_scatter = std::chrono::high_resolution_clock::now();
        auto duration_scatter = std::chrono::duration_cast<std::chrono::nanoseconds>(end_scatter - start_scatter).count();
        total_duration_scatter += duration_scatter;

        // Verify the scattered data
        assert(memcmp(int_data1[i], int_data1_scatter[i], sizeof(int_data1[i])) == 0);
        assert(memcmp(int_data2[i], int_data2_scatter[i], sizeof(int_data2[i])) == 0);
        assert(memcmp(int_data3[i], int_data3_scatter[i], sizeof(int_data3[i])) == 0);
        assert(memcmp(int_data4[i], int_data4_scatter[i], sizeof(int_data4[i])) == 0);
        assert(memcmp(str_data1[i], str_data1_scatter[i], sizeof(str_data1[i])) == 0);
        assert(memcmp(str_data2[i], str_data2_scatter[i], sizeof(str_data2[i])) == 0);
        assert(memcmp(str_data3[i], str_data3_scatter[i], sizeof(str_data3[i])) == 0);
        assert(memcmp(str_data4[i], str_data4_scatter[i], sizeof(str_data4[i])) == 0);
    }

    // Clean up
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        delete[] dest_buffers[i];
    }

    long long average_duration_gather = total_duration_gather / NUM_ITERATIONS;
    long long average_duration_scatter = total_duration_scatter / NUM_ITERATIONS;
    long long average_duration_schema_creation = total_duration_schema_creation / NUM_ITERATIONS;

    std::cout << "Average time taken to create schema: " << average_duration_schema_creation << " nanoseconds" << std::endl;
    std::cout << "Average time taken to gather data: " << average_duration_gather << " nanoseconds" << std::endl;
    std::cout << "Average time taken to scatter data: " << average_duration_scatter << " nanoseconds" << std::endl;
    std::cout << "Data gathered per iteration: " << total_data_gathered / NUM_ITERATIONS << " bytes" << std::endl;

    std::cout << "Data gathered and scattered successfully!" << std::endl;

    return 0;
}