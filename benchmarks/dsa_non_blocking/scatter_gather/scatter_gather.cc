#include "scatter_gather.h"
#include <cstdio>
#include <cstring>
#include <stdlib.h>

ScatterGather::ScatterGather(dml_path_t execution_path, size_t num_jobs) : num_jobs(num_jobs), current_job_index(0) {
    //jobs.resize(num_jobs);
    initialize_dml_jobs(execution_path);
}

ScatterGather::~ScatterGather() {
    cleanup_dml_jobs();
}

dml_status_t ScatterGather::initialize_dml_jobs(dml_path_t execution_path) {
    uint32_t size = 0u;
    dml_status_t status;
    for (size_t i = 0; i < num_jobs; ++i) {
        status = dml_get_job_size(execution_path, &size);
        if (DML_STATUS_OK != status) {
            fprintf(stderr, "An error (%u) occurred during getting job size.\n", status);
            break;
        }

        dml_job_t* job = (dml_job_t *)malloc(size);
        status = dml_init_job(execution_path, job);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "Failed to create DML job with code %d\n", status);
            break;
        }
        jobs.push_back(job);
    }
    return status;
}

void ScatterGather::cleanup_dml_jobs() {
    for (auto job : jobs) {
        dml_status_t status = dml_finalize_job(job);
        if (DML_STATUS_OK != status) {
            printf("An error (%u) occurred during job finalization.\n", status);
        }
        free(job);
    }
}

dml_job_t* ScatterGather::get_next_job() {
    dml_job_t* job = jobs[current_job_index];
    current_job_index = (current_job_index + 1) % num_jobs;
    return job;
}

void ScatterGather::dsa_gather_blocking(Schema& schema, void *out, size_t* out_size) {
    dml_status_t status;
    size_t offset = 0;
    size_t num_tuples = schema.size();

    for (size_t i = 0; i < num_tuples; ++i) {
        dml_job_t* job = get_next_job();
        job->operation = DML_OP_MEM_MOVE;
        job->source_first_ptr = std::get<0>(schema[i]);
        job->destination_first_ptr = static_cast<uint8_t*>(out) + offset;
        job->source_length = std::get<1>(schema[i]);
        job->flags = DML_FLAG_PREFETCH_CACHE;

        status = dml_execute_job(job, DML_WAIT_MODE_BUSY_POLL);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu with error code %u\n", i, status);
            break;
        }

        offset += std::get<1>(schema[i]);
    }
    *out_size = offset;
}

void ScatterGather::dsa_scatter_blocking(uint8_t* in, Schema& schema) {
    dml_status_t status;
    size_t in_offset = 0;
    size_t num_tuples = schema.size();

    for (size_t i = 0; i < num_tuples; ++i) {
        dml_job_t* job = get_next_job();
        job->operation = DML_OP_MEM_MOVE;
        job->source_first_ptr = static_cast<uint8_t*>(in) + in_offset;
        job->destination_first_ptr = std::get<0>(schema[i]);
        job->source_length = std::get<1>(schema[i]);
        job->flags = DML_FLAG_PREFETCH_CACHE;

        status = dml_execute_job(job, DML_WAIT_MODE_BUSY_POLL);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu with error code %u\n", i, status);
            break;
        }

        in_offset += std::get<1>(schema[i]);
    }
}

void ScatterGather::dsa_gather_non_blocking(Schema& schema, void *out, size_t* out_size) {
    dml_status_t status;
    size_t offset = 0;
    size_t num_tuples = schema.size();

    for (size_t i = 0; i < num_tuples; ++i) {
        dml_job_t* job = get_next_job();
        job->operation = DML_OP_MEM_MOVE;
        job->source_first_ptr = std::get<0>(schema[i]);
        job->destination_first_ptr = static_cast<uint8_t*>(out) + offset;
        job->source_length = std::get<1>(schema[i]);
        job->flags = DML_FLAG_PREFETCH_CACHE;

        status = dml_submit_job(job);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu with error code %u\n", i, status);
            break;
        }

        offset += std::get<1>(schema[i]);
    }
    *out_size = offset;
}

void ScatterGather::dsa_scatter_non_blocking(uint8_t* in, Schema& schema) {
    dml_status_t status;
    size_t in_offset = 0;
    size_t num_tuples = schema.size();

    for (size_t i = 0; i < num_tuples; ++i) {
        dml_job_t* job = get_next_job();
        job->operation = DML_OP_MEM_MOVE;
        job->source_first_ptr = static_cast<uint8_t*>(in) + in_offset;
        job->destination_first_ptr = std::get<0>(schema[i]);
        job->source_length = std::get<1>(schema[i]);
        job->flags = DML_FLAG_PREFETCH_CACHE;

        status = dml_submit_job(job);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu with error code %u\n", i, status);
            break;
        }

        in_offset += std::get<1>(schema[i]);
    }
}

void ScatterGather::wait_for_all_jobs() {
    for (auto job : jobs) {
        dml_status_t status = dml_wait_job(job, DML_WAIT_MODE_UMWAIT);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML wait operation failed with error code %u\n", status);
        }
    }
}

int ScatterGather::GatherWithMemCpy(const Schema& schema, uint8_t* out, size_t* out_size) {
    size_t out_offset = 0;
    for (const auto& el : schema) {
        memcpy(out + out_offset, std::get<0>(el), std::get<1>(el));
        out_offset += std::get<1>(el);
    }
    *out_size = out_offset;

    /*
    std::cout << "printing contents of gather output buffer:\n";
    std::cout << *((int*)out) << " ";
    std::cout << *((int*)(out+4)) << " ";
    for (int i = 0; i < 10; i++) std::cout << out[8+i];
    std::cout << " ";
    for (int i = 0; i < 10; i++) std::cout << out[10+8+i];
    std::cout << "\n------------------\n";
    */
    return 0;
}

int ScatterGather::ScatterWithMemCpy(const uint8_t* in, const Schema& schema) {
    size_t in_offset = 0;
    for (const auto& el : schema) {
        //std::cout << "ScatterWithMemCpy from source " << in + in_offset << " to destination " << const_cast<uint8_t*>(std::get<0>(el));
        //std::cout << " with size " << std::get<1>(el) << "\n";
        memcpy(const_cast<uint8_t*>(std::get<0>(el)), in + in_offset, std::get<1>(el));
        in_offset += std::get<1>(el);
    }
    /*
    std::cout << "printing contents of scatter input buffer:\n";
    std::cout << *((int*)in) << " ";
    std::cout << *((int*)(in+4)) << " ";
    for (int i = 0; i < 10; i++) std::cout << in[8+i];
    std::cout << " ";
    for (int i = 0; i < 10; i++) std::cout << in[10+8+i];
    std::cout << "\n------------------\n";
    */
    return 0;
}

void ScatterGather::UpdateScatterSchema(Schema& schema, std::vector<size_t> sizes) {
    if (sizes.size() != schema.size()) {
        fprintf(stderr, "discrepency in schema in sizes\n");
        for (auto size: sizes) fprintf(stderr, "%zu ", size);
        fprintf(stderr, "\n");
        /*
        std::cout << "discrepency in schema in sizes\n";
        for (auto size: sizes) std::cout << size << " ";
        std::cout << std::endl;
        */
        exit(1);
    }

    for (size_t i = 0; i < schema.size(); i++) {
        auto &elem_size = std::get<1>(schema[i]);
        //std::cout << "updating scatter schema from size " << std::get<1>(schema[i]);
        elem_size = sizes[i];
        //std::cout << " to size " << std::get<1>(schema[i]) << "\n";
    }
}