#include "scatter_gather.h"
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include <cassert>

ScatterGather::ScatterGather(dml_path_t execution_path) : last_job_index(0), total_jobs(250) {
    jobs.resize(250);
    initialize_dml_jobs(DML_PATH_HW, 250);
    for (dml_job_t* job : jobs) {
        assert(job != nullptr);
    }
}

ScatterGather::ScatterGather(dml_path_t execution_path, size_t num_jobs) : last_job_index(0), total_jobs(num_jobs) {
    jobs.resize(num_jobs);
    initialize_dml_jobs(execution_path, num_jobs);
    for (dml_job_t* job : jobs) {
        assert(job != nullptr);
    }
}

ScatterGather::~ScatterGather() {
    for (dml_job_t* job : jobs) {
        cleanup_dml_job(job);
    }
    // Clean up the batch job
    dml_status_t status = dml_finalize_job(batch_job);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job finalization.\n", status);
        free(batch_job);
    }
    free(batch_job);


}

dml_status_t ScatterGather::initialize_dml_jobs(dml_path_t execution_path, size_t num_jobs) {
    dml_status_t status;
    dml_job_t* job;

    for (size_t i = 0; i < num_jobs; ++i) {
        uint32_t size = 0u;
        status = dml_get_job_size(execution_path, &size);
        if (DML_STATUS_OK != status) {
            fprintf(stderr, "An error (%u) occurred during getting job size.\n", status);
            break;
        }

        job = (dml_job_t *)malloc(size);
        status = dml_init_job(execution_path, job);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "Failed to create DML job with code %d\n", status);
            break;
        }
        if (i == num_jobs) {
            batch_job = job;
        } else {
            jobs[i] = job;
        }
    }

    std::cerr << "I INITIALIZED" << std::endl;
    
    return status;
}

void ScatterGather::cleanup_dml_job(dml_job_t* job) {
    dml_status_t status = dml_finalize_job(job);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during batch job finalization.\n", status);
        free(job);
    }
    free(job);
}

void ScatterGather::gather_data_blocking(Schema& schema, void *out, size_t* out_size) {
    dml_status_t status;            // DML operation status
    size_t offset = 0;              // Offset in the destination buffer
    size_t num_tuples = schema.size(); // Number of tuples to gather
    std::cout << "There are " << num_tuples << " tuples\n";

    for (size_t i = 0; i < num_tuples; ++i) {
        // Select the next job in a round-robin fashion
        dml_job_t* job = jobs[last_job_index];
        last_job_index = (last_job_index + 1) % total_jobs;
        
        // Configure the memory move operation
        job->operation = DML_OP_MEM_MOVE;
        job->source_first_ptr = std::get<0>(schema[i]); // Pointer from the tuple
        //job->source_first_ptr = tuples[i].ptr; // Pointer from the tuple
        job->destination_first_ptr = static_cast<uint8_t*>(out) + offset;
        job->source_length = std::get<1>(schema[i]); // Size from the tuple
        job->flags = DML_FLAG_PREFETCH_CACHE; // Use prefetch cache flag
        //job->source_length = tuples[i].size;

        // Execute the operation
        status = dml_execute_job(job, DML_WAIT_MODE_BUSY_POLL);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu\n", i);
            break;
        }

        offset += std::get<1>(schema[i]); // Advance the destination offset
        //offset += tuples[i].size; // Advance the destination offset
    }
    *out_size = offset;
}

void ScatterGather::gather_data_non_blocking(Schema& schema, void *out, size_t* out_size) {
    dml_status_t status;            // DML operation status
    size_t offset = 0;              // Offset in the destination buffer
    size_t num_tuples = schema.size(); // Number of tuples to gather

    for (size_t i = 0; i < num_tuples; ++i) {
        // Select the next job in a round-robin fashion
        dml_job_t* job = jobs[last_job_index];
        last_job_index = (last_job_index + 1) % total_jobs;

        // Configure the memory move operation
        job->operation = DML_OP_MEM_MOVE;
        job->source_first_ptr = std::get<0>(schema[i]); // Pointer from the tuple
        job->destination_first_ptr = static_cast<uint8_t*>(out) + offset;
        job->source_length = std::get<1>(schema[i]);
        job->flags = DML_FLAG_PREFETCH_CACHE; // Use prefetch cache flag

        // Submit the operation
        status = dml_submit_job(job);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu\n", i);
            break;
        }

        offset += std::get<1>(schema[i]); // Advance the destination offset
    }

    *out_size = offset;
}

void ScatterGather::gather_data_batch(Schema& schema, void *out, size_t* out_size) {
    dml_status_t status;            // DML operation status
    size_t offset = 0;              // Offset in the destination buffer
    size_t num_tuples = schema.size(); // Number of tuples to gather

    assert(num_tuples <= 32);

    // Set up batch operations
    for (size_t i = 0; i < num_tuples; ++i) {
        // Select the next job in a round-robin fashion
        dml_job_t* job = jobs[last_job_index];
        last_job_index = (last_job_index + 1) % total_jobs;

        // Configure the memory move operation
        job->operation = DML_OP_MEM_MOVE;
        job->source_first_ptr = std::get<0>(schema[i]); // Pointer from the tuple
        job->destination_first_ptr = static_cast<uint8_t*>(out) + offset;
        job->source_length = std::get<1>(schema[i]); // Size from the tuple
        //job->flags = DML_FLAG_PREFETCH_CACHE; // Use prefetch cache flag

        // Add the job to the batch
        status = dml_batch_set_mem_move_by_index(batch_job, i, job->source_first_ptr, job->destination_first_ptr, job->source_length, DML_FLAG_PREFETCH_CACHE);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "Failed to set batch operation at index %zu with code %d\n", i, status);
            break;
        }

        offset += std::get<1>(schema[i]); // Advance the destination offset
    }

    // Execute the batch job
    status = dml_execute_job(batch_job, DML_WAIT_MODE_BUSY_POLL);
    if (status != DML_STATUS_OK) {
        fprintf(stderr, "DML batch operation failed with code %d\n", status);
    }

    *out_size = offset;
}

void ScatterGather::wait_for_all_dsa_jobs() {
    dml_wait_job(batch_job, DML_WAIT_MODE_BUSY_POLL);
    for (dml_job_t* job : jobs) {
        dml_wait_job(job, DML_WAIT_MODE_BUSY_POLL);
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
      memcpy(const_cast<uint8_t*>(std::get<0>(el)), in + in_offset,
             std::get<1>(el));
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
    // TODO: Change cerr to fprintf
    if (sizes.size() != schema.size()) {
        std::cerr << "discrepency in schema in sizes\n";
        for (auto size: sizes) std::cerr << size << " ";
        std::cerr << std::endl;
        exit(1);
    }

    for (size_t i = 0; i < schema.size(); i++) {
        auto &elem_size = std::get<1>(schema[i]);
        //std::cout << "updating scatter schema from size " << std::get<1>(schema[i]);
        elem_size = sizes[i];
        //std::cout << " to size " << std::get<1>(schema[i]) << "\n";
    }
}