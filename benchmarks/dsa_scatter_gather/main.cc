#include "scatter_gather/scatter_gather.h"
#include <iostream>
#include <chrono>
#include <cstring>
#include <stdlib.h>
#include <assert.h>

int main() {
    // Initialize 5 strings
    const char* strings[5] = {
        "Hello, ",
        "this is ",
        "a simple ",
        "benchmark ",
        "test."
    };

    // Initialize 5 small integer arrays
    int int_arrays[5][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9},
        {10, 11, 12},
        {13, 14, 15}
    };

    ScatterGather::Schema gather_schema;
    // Create tuples for strings
    for (int i = 0; i < 5; ++i) {
        std::tuple<uint8_t*, size_t> t((uint8_t*)strings[i], strlen(strings[i]));
        gather_schema.push_back(t);
    }

    // Create tuples for integer arrays
    for (int i = 0; i < 5; ++i) {
        std::tuple<uint8_t*, size_t> t((uint8_t*)int_arrays[i], sizeof(int_arrays[i]));
        gather_schema.push_back(t);
    }

    // Calculate the total size needed for the destination buffer
    size_t total_size = 0;
    for (const auto& t : gather_schema) {
        total_size += std::get<1>(t);
    }

    // Allocate the destination buffer
    uint8_t* dest_buffer = new uint8_t[total_size];
    size_t out_size = 0;

    // Create ScatterGather object
    ScatterGather scatterGather;

    // Measure the time taken to gather data
    auto start = std::chrono::high_resolution_clock::now();
    scatterGather.gather_data(gather_schema, dest_buffer, &out_size);
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken to gather data: " << duration.count() << " seconds" << std::endl;

    // Clean up
    delete[] dest_buffer;

    std::cout << "Total size gathered: " << out_size << " bytes" << std::endl;
    assert(out_size == total_size);
    std::cout << "Data gathered successfully!" << std::endl;

    return 0;
}