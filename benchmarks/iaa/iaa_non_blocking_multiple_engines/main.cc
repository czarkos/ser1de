#include "iaa_comp.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <string>

std::string generate_random_string(size_t length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        result += charset[dist(rng)];
    }
    return result;
}

void benchmark_blocking(int num_queues, int num_jobs, int max_jobs_per_queue = 16) {
    qpl_path_t path = qpl_path_hardware; // or QPL_PATH_SOFTWARE
    IAAComp iaa_comp(path, num_queues, max_jobs_per_queue);

    std::vector<std::pair<int, int>> job_indices;
    std::vector<std::string> sources;
    std::vector<std::vector<uint8_t>> destinations(num_jobs, std::vector<uint8_t>(256)); // Assuming max destination size is 256 bytes

    // Generate random strings for the jobs
    for (int i = 0; i < num_jobs; ++i) {
        size_t length = 50 + (std::rand() % 151); // Random length between 50 and 200
        sources.push_back(generate_random_string(length));
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    // Submit jobs using non-blocking functions
    for (int i = 0; i < num_jobs; ++i) {
        uint8_t* source = reinterpret_cast<uint8_t*>(const_cast<char*>(sources[i].c_str()));
        uint32_t source_size = sources[i].size();
        uint8_t* destination = destinations[i].data();
        uint32_t destination_size = destinations[i].size();

        auto indices = iaa_comp.compress_blocking(source, source_size, destination, destination_size);
        job_indices.push_back(indices);
    }

    // Poll for job completion
    for (const auto& indices : job_indices) {
        int queue_index = indices.first;
        int job_index = indices.second;

        while (iaa_comp.poll_job(queue_index, job_index) == QPL_STS_BEING_PROCESSED) {
            // Optionally perform other tasks while waiting for job completion
        }

        // Handle job completion
        // ...
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "Number of queues: " << num_queues << ", Latency: " << elapsed.count() << " seconds" << std::endl;
}

void benchmark_non_blocking(int num_queues, int num_jobs, int max_jobs_per_queue = 16) {
    qpl_path_t path = qpl_path_hardware; // or QPL_PATH_SOFTWARE
    IAAComp iaa_comp(path, num_queues, max_jobs_per_queue);

    std::vector<std::pair<int, int>> job_indices;
    std::vector<std::string> sources;
    std::vector<std::vector<uint8_t>> destinations(num_jobs, std::vector<uint8_t>(256)); // Assuming max destination size is 256 bytes

    // Generate random strings for the jobs
    for (int i = 0; i < num_jobs; ++i) {
        size_t length = 50 + (std::rand() % 151); // Random length between 50 and 200
        sources.push_back(generate_random_string(length));
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    // Submit jobs using non-blocking functions
    for (int i = 0; i < num_jobs; ++i) {
        uint8_t* source = reinterpret_cast<uint8_t*>(const_cast<char*>(sources[i].c_str()));
        uint32_t source_size = sources[i].size();
        uint8_t* destination = destinations[i].data();
        uint32_t destination_size = destinations[i].size();

        auto indices = iaa_comp.compress_non_blocking(source, source_size, destination, destination_size);
        job_indices.push_back(indices);
    }

    // Poll for job completion
    for (const auto& indices : job_indices) {
        int queue_index = indices.first;
        int job_index = indices.second;

        while (iaa_comp.poll_job(queue_index, job_index) == QPL_STS_BEING_PROCESSED) {
            // Optionally perform other tasks while waiting for job completion
        }

        // Handle job completion
        // ...
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "Number of queues: " << num_queues << ", Latency: " << elapsed.count() << " seconds" << std::endl;
}

int main() {
    const int num_jobs = 1000000;
    const int max_jobs_per_queue = 16;

    // Benchmark with 1, 2, 4, and 8 engines for blocking compression
    std::cout << "Blocking Compression Benchmark:" << std::endl;
    benchmark_blocking(1, num_jobs, max_jobs_per_queue);
    benchmark_blocking(2, num_jobs, max_jobs_per_queue);
    benchmark_blocking(4, num_jobs, max_jobs_per_queue);
    benchmark_blocking(8, num_jobs, max_jobs_per_queue);

    std::cout << "Non-Blocking Compression Benchmark:" << std::endl;
    // Benchmark with 1, 2, 4, and 8 engines for non-blocking compression
    benchmark_non_blocking(1, num_jobs, max_jobs_per_queue);
    benchmark_non_blocking(2, num_jobs, max_jobs_per_queue);
    benchmark_non_blocking(4, num_jobs, max_jobs_per_queue);
    benchmark_non_blocking(8, num_jobs, max_jobs_per_queue);

    return 0;
}
