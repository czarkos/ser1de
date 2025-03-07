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
    std::vector<std::unique_ptr<uint8_t[]>> job_buffers;
    std::vector<qpl_job*> jobs;

    size_t max_jobs;
    size_t current_job;
    size_t active_jobs;

    // this variable was in a constructor used in early versions of ser1de and in motivational benchmarks
    // the idea behind this variable was to try and have some control over the iaa work_queues
    // in this version of ser1de, it is here for compatibility reasons
    size_t num_queues;

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

    inline qpl_job* get_next_iaa_job() {
        // get job
        qpl_job* job = jobs[current_job];

        // update current job index
        current_job = (current_job + 1) % max_jobs;

        return job;
    }

    inline size_t get_just_used_index() {
        if (current_job == 0)
            return max_jobs-1;
        else
            return current_job-1;
    }

public:
    IAAComp(qpl_path_t path, size_t num_queues, size_t max_jobs) : max_jobs(max_jobs), current_job(0), active_jobs(0), num_queues(num_queues) {
        // initialize num_max_jobs jobs
        for (size_t i = 0; i < max_jobs; ++i) {
            qpl_job* job = nullptr;
            auto job_buffer = init_qpl(path, job);
            if (!job_buffer) {
                throw std::runtime_error("Failed to initialize QPL job.");
            }
            job_buffers.push_back(std::move(job_buffer));
            jobs.push_back(job);
        }
    }

    IAAComp() : max_jobs(2), current_job(0), active_jobs(0), num_queues(0) {
        // default path is the hardware path with 2 pre-allocated jobs
        qpl_path_t path = qpl_path_hardware;
        // initialize num_max_jobs jobs
        for (size_t i = 0; i < max_jobs; ++i) {
            qpl_job* job = nullptr;
            auto job_buffer = init_qpl(path, job);
            if (!job_buffer) {
                throw std::runtime_error("Failed to initialize QPL job.");
            }
            job_buffers.push_back(std::move(job_buffer));
            jobs.push_back(job);
        }
    }

    ~IAAComp() {
        for (auto job : jobs) {
            if (job) {
                free_qpl(job);
            }
        }
    }

    size_t compress_blocking(uint8_t *source, uint32_t source_size, uint8_t *destination, uint32_t destination_size, uint32_t* actualOutSize) {
        qpl_job* job = get_next_iaa_job();

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

        *actualOutSize = job->total_out;
        return get_just_used_index();
    }

    size_t compress_non_blocking(uint8_t *source, uint32_t source_size, uint8_t *destination, uint32_t destination_size) {
        qpl_job* job = get_next_iaa_job();

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

        return get_just_used_index();
    }

    size_t decompress_blocking(uint8_t *source, uint32_t source_size, uint8_t *destination, uint32_t destination_size, uint32_t* actualOutSize) {
        qpl_job* job = get_next_iaa_job();

        job->op = qpl_op_decompress;
        job->level = qpl_default_level;
        job->next_in_ptr = source;
        job->available_in = source_size;
        job->next_out_ptr = destination;
        job->available_out = destination_size;
        job->flags = QPL_FLAG_FIRST | QPL_FLAG_LAST;

        qpl_status status = qpl_execute_job(job);
        if (status != QPL_STS_OK) {
            std::cerr << "Decompression failed with error code: " << status << std::endl;
        }

        *actualOutSize = job->total_out;
        return get_just_used_index();
    }

    size_t decompress_non_blocking(uint8_t *source, uint32_t source_size, uint8_t *destination, uint32_t destination_size) {
        qpl_job* job = get_next_iaa_job();

        job->op = qpl_op_decompress;
        job->level = qpl_default_level;
        job->next_in_ptr = source;
        job->available_in = source_size;
        job->next_out_ptr = destination;
        job->available_out = destination_size;
        job->flags = QPL_FLAG_FIRST | QPL_FLAG_LAST;

        qpl_status status = submit_job(job);
        if (status != QPL_STS_OK) {
            std::cerr << "Decompression job submission failed with error code: " << status << std::endl;
        }

        return get_just_used_index();
    }

    qpl_status poll_job(size_t job_index) {
        qpl_job* job = jobs[job_index];
        return qpl_check_job(job);
    }

    size_t get_job_compr_size(size_t job_index) {
        return jobs[job_index]->total_out;
    }
};

#endif // IAA_COMP_H
