#ifndef IAA_COMP_H
#define IAA_COMP_H

#include <iostream>
#include <cassert>
#include <memory>
#include "qpl/qpl.h"

class IAAComp {
private:
    std::unique_ptr<uint8_t[]> global_job_buffer;
    qpl_job* job;

    std::unique_ptr<uint8_t[]> init_qpl(qpl_path_t e_path) {
        uint32_t job_size = 0;
        qpl_status status = qpl_get_job_size(e_path, &job_size);
        if (status != QPL_STS_OK) {
            std::cerr << "An error acquired during job size getting." << std::endl;
            return nullptr;
        }

        std::unique_ptr<uint8_t[]> job_buffer;
        job_buffer = std::make_unique<uint8_t[]>(job_size);
        auto job = reinterpret_cast<qpl_job *>(job_buffer.get());
        status = qpl_init_job(e_path, job);
        if (status != QPL_STS_OK) {
            std::cerr << "An error acquired during compression job initializing." << std::endl;
            return nullptr;
        }

        //return std::move(job_buffer);
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
public:
    IAAComp(qpl_path_t path) {
        global_job_buffer = init_qpl(path);
        if (global_job_buffer == nullptr) {
            std::cerr << "Failed to init qpl." << std::endl;
            throw std::runtime_error("Failed to init qpl");
        }
        job = reinterpret_cast<qpl_job *>(global_job_buffer.get());
        assert(job != nullptr);
        std::cerr << "IAAComp initialized successfully." << std::endl;
    }

    ~IAAComp() {
        if (free_qpl(job)) {
            std::cerr << "Failed to free resources." << std::endl;
        }
        std::cerr << "IAAComp destroyed successfully." << std::endl;
    }

    int compress(uint8_t* src, uint32_t srcSize, uint8_t* outBStream, uint32_t outBufferSize, uint32_t* actualOutSize) {
        job->op             = qpl_op_compress;
        job->level          = qpl_default_level;
        job->next_in_ptr    = src;
        job->available_in   = srcSize;
        job->next_out_ptr   = outBStream;
        job->available_out  = outBufferSize;
        job->flags          = QPL_FLAG_FIRST | QPL_FLAG_LAST | QPL_FLAG_OMIT_VERIFY;

        qpl_status status = qpl_execute_job(job);
        if (status != QPL_STS_OK) {
            std::cerr << "An error " << status << " acquired during compression." << std::endl;
            return -1;
        }

        *actualOutSize = job->total_out;
        return 0;
    }

    int decompress(uint8_t* src, uint32_t srcSize, uint8_t* outBStream, uint32_t outBufferSize, uint32_t* actualOutSize) {
        job->op             = qpl_op_decompress;
        job->next_in_ptr    = src;
        job->available_in   = srcSize;
        job->next_out_ptr   = outBStream;
        job->available_out  = outBufferSize;
        job->flags          = QPL_FLAG_FIRST | QPL_FLAG_LAST;

        qpl_status status = qpl_execute_job(job);
        if (status != QPL_STS_OK) {
            std::cerr << "An error " << status << " acquired during decompression." << std::endl;
            return -1;
        }

        *actualOutSize = job->total_out;
        return 0;
    }
};

#endif // IAA_COMP_H