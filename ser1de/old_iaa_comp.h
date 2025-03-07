#ifndef IAA_COMP_H
#define IAA_COMP_H

#include <chrono>
#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include "qpl/qpl.h"

class IAAComp {
private:
    struct Queue {
        std::vector<std::unique_ptr<uint8_t[]>> job_buffers;
        std::vector<qpl_job*> jobs;
    };

    std::vector<Queue> queues;
    int current_queue;
    int num_queues;
    int max_jobs_per_queue;

    std::unique_ptr<uint8_t[]> init_qpl(qpl_path_t e_path, qpl_job*& job) {
        uint32_t job_size = 0;
        qpl_status status = qpl_get_job_size(e_path, &job_size);
        if (status != QPL_STS_OK) {
            std::cerr << "An error acquired during job size getting." << std::endl;
            return nullptr;
        }

        std::unique_ptr<uint8_t[]> job_buffer = std::make_unique<uint8_t[]>(job_size);
        job = reinterpret_cast<qpl_job *>(job_buffer.get());
        status = qpl_init_job(e_path, job);
        if (status != QPL_STS_OK) {
            std::cerr << "An error acquired during compression job initializing." << std::endl;
            return nullptr;
        }

        return job_buffer;
    }

    int free_qpl(qpl_job *job) {
        qpl_status status = qpl_fini_job(job);
        if (status != QPL_STS_OK) {
            std::cerr << "An error acquired during job finalization." << std::endl;
            return -1;
        }
        return 0;
    }

    inline qpl_status submit_job(qpl_job *job) {
        qpl_status status = qpl_submit_job(job);
        if (status != QPL_STS_OK) {
            std::cerr << "An error acquired during job submission. Error code: " << status << std::endl;
        }
        return status;
    }

public:
    IAAComp(qpl_path_t path, int num_queues, int max_jobs_per_queue) 
        : current_queue(0), num_queues(num_queues), max_jobs_per_queue(max_jobs_per_queue) {
        queues.resize(num_queues);
        for (int i = 0; i < num_queues; ++i) {
            qpl_job* job = nullptr;
            auto job_buffer = init_qpl(path, job);
            if (!job_buffer) {
                throw std::runtime_error("Failed to initialize QPL job.");
            }
            queues[i].job_buffers.push_back(std::move(job_buffer));
            queues[i].jobs.push_back(job);
        }
    }

    ~IAAComp() {
        for (auto& queue : queues) {
            for (auto job : queue.jobs) {
                if (job) {
                    free_qpl(job);
                }
            }
        }
    }

    //std::pair<int, int> compress_blocking(uint8_t *source, uint32_t source_size, uint8_t *destination, uint32_t destination_size) {
    std::pair<int, int> compress_blocking(uint8_t *source, uint32_t source_size, uint8_t *destination, uint32_t destination_size, uint32_t* actualOutSize) {
        while (queues[current_queue].jobs.size() >= max_jobs_per_queue) {
            // Busy-wait loop
        }

        qpl_job* job = queues[current_queue].jobs.back();
        job->op = qpl_op_compress;
        job->level = qpl_default_level;
        job->next_in_ptr = source;
        job->available_in = source_size;
        job->next_out_ptr = destination;
        job->available_out = destination_size;
        job->flags = QPL_FLAG_FIRST | QPL_FLAG_LAST;

        qpl_status status = qpl_execute_job(job);
        if (status != QPL_STS_OK) {
            std::cerr << "Compression failed with error code: " << status << std::endl;
            exit(-1);
        }

        int queue_index = current_queue;
        int job_index = queues[current_queue].jobs.size() - 1;
        current_queue = (current_queue + 1) % num_queues;

        *actualOutSize = job->total_out;
        return {queue_index, job_index};
    }

    std::pair<int, int> compress_non_blocking(uint8_t *source, uint32_t source_size, uint8_t *destination, uint32_t destination_size) {
        while (queues[current_queue].jobs.size() >= max_jobs_per_queue) {
            // Busy-wait loop
        }

        qpl_job* job = queues[current_queue].jobs.back();
        job->op = qpl_op_compress;
        job->level = qpl_default_level;
        job->next_in_ptr = source;
        job->available_in = source_size;
        job->next_out_ptr = destination;
        job->available_out = destination_size;
        job->flags = QPL_FLAG_FIRST | QPL_FLAG_LAST;
        
        qpl_status status = submit_job(job);
        if (status != QPL_STS_OK) {
            std::cerr << "Compression job submission failed with error code: " << status << std::endl;
            exit(-1);
        }

        int queue_index = current_queue;
        int job_index = queues[current_queue].jobs.size() - 1;
        current_queue = (current_queue + 1) % num_queues;

        return {queue_index, job_index};
    }

    std::pair<int, int> decompress_blocking(uint8_t *source, uint32_t source_size, uint8_t *destination, uint32_t destination_size, uint32_t* actualOutSize) {
        //while (queues[current_queue].jobs.size() >= max_jobs_per_queue) {
        //    // Busy-wait loop
        //}

        qpl_job* job = queues[current_queue].jobs.back();
        job->op = qpl_op_decompress;
        job->level = qpl_default_level;
        job->next_in_ptr = source;
        job->available_in = source_size;
        job->next_out_ptr = destination;
        job->available_out = destination_size;
        job->flags = QPL_FLAG_FIRST | QPL_FLAG_LAST;

        qpl_status status = qpl_execute_job(job);
        if (status != QPL_STS_OK) {
            std::cerr << "Decompression failed." << std::endl;
        }

        int queue_index = current_queue;
        int job_index = queues[current_queue].jobs.size() - 1;
        current_queue = (current_queue + 1) % num_queues;

        *actualOutSize = job->total_out;
        return {queue_index, job_index};
    }

    std::pair<int, int> decompress_non_blocking(uint8_t *source, uint32_t source_size, uint8_t *destination, uint32_t destination_size) {
        //std::chrono::steady_clock::time_point de_begin = std::chrono::steady_clock::now();
        while (queues[current_queue].jobs.size() >= max_jobs_per_queue) {
            // Busy-wait loop
        }

        qpl_job* job = queues[current_queue].jobs.back();
        job->op = qpl_op_decompress;
        job->level = qpl_default_level;
        job->next_in_ptr = source;
        job->available_in = source_size;
        job->next_out_ptr = destination;
        job->available_out = destination_size;
        job->flags = QPL_FLAG_FIRST | QPL_FLAG_LAST;
        //std::chrono::steady_clock::time_point de_end = std::chrono::steady_clock::now();
        //std::chrono::nanoseconds de_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(de_end - de_begin);
        //std::cout << "Decompression job initialization time: " << de_duration.count() << "ns" << std::endl;

        //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        qpl_status status = submit_job(job);
        //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        //std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        //std::cout << "Decompression job submission time: " << duration.count() << "ns" << std::endl;
        if (status != QPL_STS_OK) {
            std::cerr << "Decompression job submission failed." << std::endl;
        }

        int queue_index = current_queue;
        int job_index = queues[current_queue].jobs.size() - 1;
        current_queue = (current_queue + 1) % num_queues;

        return {queue_index, job_index};
    }

    qpl_status poll_job(int queue_index, int job_index) {
        /*
        if (queue_index < 0 || queue_index >= num_queues) {
            std::cerr << "Invalid queue index." << std::endl;
            return QPL_STS_INVALID_PARAM_ERR;
        }

        if (job_index < 0 || job_index >= queues[queue_index].jobs.size()) {
            std::cerr << "Invalid job index." << std::endl;
            return QPL_STS_INVALID_PARAM_ERR;
        }
        */

        qpl_job* job = queues[queue_index].jobs[job_index];
        return qpl_check_job(job);
    }

    void add_job_to_queue(qpl_path_t path, int queue_index) {
        if (queue_index < 0 || queue_index >= num_queues) {
            std::cerr << "Invalid queue index." << std::endl;
            return;
        }

        qpl_job* job = nullptr;
        auto job_buffer = init_qpl(path, job);
        if (!job_buffer) {
            throw std::runtime_error("Failed to initialize QPL job.");
        }
        queues[queue_index].job_buffers.push_back(std::move(job_buffer));
        queues[queue_index].jobs.push_back(job);
    }

    size_t get_job_compr_size(int queue_index, int job_index) {
        //assert(qpl_check_job(queues[queue_index].jobs[job_index]) == QPL_STS_OK);
        return queues[queue_index].jobs[job_index]->total_out;
    }
};

#endif // IAA_COMP_H
