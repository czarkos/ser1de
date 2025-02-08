#ifndef SCATTER_GATHER_H
#define SCATTER_GATHER_H

#include <dml/dml.h>
#include <vector>
#include <tuple>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <assert.h>

class ScatterGather {
public:
    typedef std::vector<std::tuple<uint8_t*, size_t>> Schema;
    ScatterGather(dml_path_t execution_path, size_t num_jobs);
    ~ScatterGather();
    // Gather
    void dsa_gather_blocking(Schema& schema, void *out, size_t* out_size);
    void dsa_gather_non_blocking(Schema& schema, void *out, size_t* out_size);
    void dsa_gather_blocking_batching(Schema& schema, void *out, size_t* out_size);
    void dsa_gather_non_blocking_batching(Schema& schema, void *out, size_t* out_size);
    // Scatter
    void dsa_scatter_blocking(uint8_t* in, Schema& schema);
    void dsa_scatter_non_blocking(uint8_t* in, Schema& schema);
    void dsa_scatter_blocking_batching(uint8_t* in, Schema& schema);
    void dsa_scatter_non_blocking_batching(uint8_t* in, Schema& schema);
    // Wait
    void wait_for_all_jobs();
    void wait_for_all_batch_jobs();
    // Memcpy versions
    int GatherWithMemCpy(const Schema& schema, uint8_t* out, size_t* out_size);
    int ScatterWithMemCpy(const uint8_t* in, const Schema& schema);
    void UpdateScatterSchema(Schema& schema, std::vector<size_t> sizes);

private:
    std::vector<dml_job_t*> batch_jobs;
    size_t num_batch_jobs;
    size_t current_batch_job_index;
    size_t active_batch_jobs;  // Track number of active jobs
    size_t batch_size;
    std::vector<uint8_t*> batch_buffers;
    std::vector<uint32_t> batch_buffer_lengths;
    std::vector<dml_job_t*> jobs;
    size_t num_jobs;
    size_t current_job_index;
    dml_status_t initialize_dml_jobs(dml_path_t execution_path);
    dml_status_t initialize_dml_batch_jobs(dml_path_t execution_path);
    void cleanup_dml_jobs();
    void cleanup_dml_batch_jobs();
    dml_job_t* get_next_job();
    dml_job_t* get_next_batch_job();
};

ScatterGather::ScatterGather(dml_path_t execution_path, size_t num_jobs) : 
    num_jobs(num_jobs), 
    current_job_index(0),
    current_batch_job_index(0),
    active_batch_jobs(0) {
    batch_size = 32;
    num_batch_jobs = 150;
    initialize_dml_jobs(execution_path);
    initialize_dml_batch_jobs(execution_path);
}

ScatterGather::~ScatterGather() {
    cleanup_dml_jobs();
    cleanup_dml_batch_jobs();
}

dml_status_t ScatterGather::initialize_dml_batch_jobs(dml_path_t execution_path) {
    uint32_t size = 0u;
    dml_status_t status;

    for (size_t i = 0; i < num_batch_jobs; ++i) {
        uint8_t* batch_buffer;
        uint32_t batch_buffer_length;

        // initialize batch job
        status = dml_get_job_size(execution_path, &size);
        if (DML_STATUS_OK != status) {
            fprintf(stderr, "An error (%u) occurred during getting job size.\n", status);
            break;
        }

        dml_job_t* batch_job = (dml_job_t *)malloc(size);
        status = dml_init_job(execution_path, batch_job);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "Failed to create DML job with code %d\n", status);
            break;
        }
        
        status = dml_get_batch_size(batch_job, batch_size, &batch_buffer_length);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "An error occured during getting batch size with code %d\n", status);
            break;
        }

        batch_buffer = (uint8_t *)malloc(batch_buffer_length);

        //fprintf(stderr, "Batch buffer length: %u\n", batch_buffer_length);
        //fprintf(stderr, "Batch size: %lu\n", batch_size);

        batch_job->operation = DML_OP_BATCH;
        batch_job->destination_first_ptr = batch_buffer;
        batch_job->destination_length = batch_buffer_length;

        batch_jobs.push_back(batch_job);
        batch_buffers.push_back(batch_buffer);
        batch_buffer_lengths.push_back(batch_buffer_length);
    }

    return status;
}

dml_status_t ScatterGather::initialize_dml_jobs(dml_path_t execution_path) {
    uint32_t size = 0u;
    dml_status_t status;

    // initialize simple jobs
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

void ScatterGather::cleanup_dml_batch_jobs() {
    for (auto batch_job : batch_jobs) {
        dml_status_t status = dml_finalize_job(batch_job);
        if (DML_STATUS_OK != status) {
            printf("An error (%u) occurred during job finalization.\n", status);
        }
        free(batch_job);
    }
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

dml_job_t* ScatterGather::get_next_batch_job() {
    dml_job_t* job = batch_jobs[current_batch_job_index];
    current_batch_job_index = (current_batch_job_index + 1) % num_batch_jobs;
    active_batch_jobs++;
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


void ScatterGather::dsa_gather_blocking_batching(Schema& schema, void *out, size_t* out_size) {
    dml_status_t status;
    size_t offset = 0;
    size_t num_tuples = schema.size();

    size_t num_batches = num_tuples / batch_size;
    size_t last_batch_size = num_tuples % batch_size;

    for (size_t i = 0; i < num_batches; ++i) {
        dml_job_t* batch_job = get_next_batch_job();
        batch_job->operation = DML_OP_BATCH;
        batch_job->destination_first_ptr = batch_buffers[current_batch_job_index];
        batch_job->destination_length = batch_buffer_lengths[current_batch_job_index];
        for (size_t j = 0; j < batch_size; ++j) {
            size_t index = i * batch_size + j;
            dml_batch_set_mem_move_by_index(batch_job, j, std::get<0>(schema[index]), static_cast<uint8_t*>(out) + offset, std::get<1>(schema[index]), DML_FLAG_PREFETCH_CACHE);
            offset += std::get<1>(schema[index]);
        }
        // make only the last job be blocking
        //if (i == num_batches - 1 && last_batch_size == 0)
        //    status = dml_execute_job(batch_job, DML_WAIT_MODE_BUSY_POLL);
        //else
        status = dml_submit_job(batch_job);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu with error code %u\n", i, status);
            break;
        }
    }
    if (last_batch_size > 0) {
        dml_job_t* batch_job = get_next_batch_job();
        batch_job->operation = DML_OP_BATCH;
        batch_job->destination_first_ptr = batch_buffers[current_batch_job_index];
        batch_job->destination_length = batch_buffer_lengths[current_batch_job_index];
        for (size_t j = 0; j < last_batch_size; ++j) {
            size_t index = num_batches * batch_size + j;
            dml_batch_set_mem_move_by_index(batch_job, j, std::get<0>(schema[index]), static_cast<uint8_t*>(out) + offset, std::get<1>(schema[index]), DML_FLAG_PREFETCH_CACHE);
            offset += std::get<1>(schema[index]);
        }
        // make only the last job be blocking
        //status = dml_execute_job(batch_job, DML_WAIT_MODE_BUSY_POLL);
        status = dml_submit_job(batch_job);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu with error code %u\n", num_batches * batch_size + last_batch_size, status);
        }
    }
    *out_size = offset;
}

void ScatterGather::dsa_gather_non_blocking_batching(Schema& schema, void *out, size_t* out_size) {
    dml_status_t status;
    size_t offset = 0;
    size_t num_tuples = schema.size();

    size_t num_batches = num_tuples / batch_size;

    for (size_t i = 0; i < num_batches; ++i) {
        dml_job_t* batch_job = get_next_batch_job();
        batch_job->operation = DML_OP_BATCH;
        batch_job->destination_first_ptr = batch_buffers[current_batch_job_index];
        batch_job->destination_length = batch_buffer_lengths[current_batch_job_index];
        for (size_t j = 0; j < batch_size; ++j) {
            size_t index = i * batch_size + j;
            dml_batch_set_mem_move_by_index(batch_job, j, std::get<0>(schema[index]), static_cast<uint8_t*>(out) + offset, std::get<1>(schema[index]), DML_FLAG_PREFETCH_CACHE);
            offset += std::get<1>(schema[index]);
        }
        // make only the last job be blocking
        //if (i == num_batches -1)
        //    status = dml_execute_job(batch_job, DML_WAIT_MODE_BUSY_POLL);
        //else
        status = dml_submit_job(batch_job);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu with error code %u\n", i, status);
            break;
        }
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

void ScatterGather::dsa_scatter_blocking_batching(uint8_t* in, Schema& schema) {
    dml_status_t status;
    size_t in_offset = 0;
    size_t num_tuples = schema.size();

    size_t num_batches = num_tuples / batch_size;
    size_t last_batch_size = num_tuples % batch_size;
    for (size_t i = 0; i < num_batches; ++i) {
        dml_job_t* batch_job = get_next_batch_job();
        batch_job->operation = DML_OP_BATCH;
        batch_job->destination_first_ptr = batch_buffers[current_batch_job_index];
        batch_job->destination_length = batch_buffer_lengths[current_batch_job_index];
        for (size_t j = 0; j < batch_size; ++j) {
            size_t index = i * batch_size + j;
            dml_batch_set_mem_move_by_index(batch_job, j, static_cast<uint8_t*>(in) + in_offset, std::get<0>(schema[index]), std::get<1>(schema[index]), DML_FLAG_PREFETCH_CACHE);
            in_offset += std::get<1>(schema[index]);
        }
        // make only the last job be blocking
        //if (i == num_batches -1 && last_batch_size == 0)
        //    status = dml_execute_job(batch_job, DML_WAIT_MODE_BUSY_POLL);
        //else
        status = dml_submit_job(batch_job);

        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu with error code %u\n", i, status);
            break;
        }
    }
    if (last_batch_size > 0) {
        dml_job_t* batch_job = get_next_batch_job();
        batch_job->operation = DML_OP_BATCH;
        batch_job->destination_first_ptr = batch_buffers[current_batch_job_index];
        batch_job->destination_length = batch_buffer_lengths[current_batch_job_index];
        for (size_t j = 0; j < last_batch_size; ++j) {
            size_t index = num_batches * batch_size + j;
            dml_batch_set_mem_move_by_index(batch_job, j, static_cast<uint8_t*>(in) + in_offset, std::get<0>(schema[index]), std::get<1>(schema[index]), DML_FLAG_PREFETCH_CACHE);
            in_offset += std::get<1>(schema[index]);
        }
        // make only the last job be blocking
        //status = dml_execute_job(batch_job, DML_WAIT_MODE_BUSY_POLL);
        status = dml_submit_job(batch_job);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu with error code %u\n", num_batches * batch_size + last_batch_size, status);
        }
    }
}

void ScatterGather::dsa_scatter_non_blocking_batching(uint8_t* in, Schema& schema) {
    dml_status_t status;
    size_t in_offset = 0;
    size_t num_tuples = schema.size();

    size_t num_batches = num_tuples / batch_size;

    // Reset counters at the start of operation
    current_batch_job_index = 0;
    active_batch_jobs = 0;

    for (size_t i = 0; i < num_batches; ++i) {
        dml_job_t* batch_job = get_next_batch_job();
        batch_job->operation = DML_OP_BATCH;
        batch_job->destination_first_ptr = batch_buffers[current_batch_job_index];
        batch_job->destination_length = batch_buffer_lengths[current_batch_job_index];
        for (size_t j = 0; j < batch_size; ++j) {
            size_t index = i * batch_size + j;
            dml_batch_set_mem_move_by_index(batch_job, j, static_cast<uint8_t*>(in) + in_offset, std::get<0>(schema[index]), std::get<1>(schema[index]), DML_FLAG_PREFETCH_CACHE);
            in_offset += std::get<1>(schema[index]);
        }
        //// make only the last job be blocking
        //if (i == num_batches -1)
        //    status = dml_execute_job(batch_job, DML_WAIT_MODE_BUSY_POLL);
        //else
        status = dml_submit_job(batch_job);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu with error code %u\n", i, status);
            break;
        }
    }
}

void ScatterGather::wait_for_all_batch_jobs() {
    // Wait for all active jobs
    for (size_t i = 0; i < active_batch_jobs; ++i) {
        size_t job_index = (num_batch_jobs + current_batch_job_index - active_batch_jobs + i) % num_batch_jobs;
        dml_status_t status = dml_wait_job(batch_jobs[job_index], DML_WAIT_MODE_BUSY_POLL);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML wait operation failed with error code %u\n", status);
        }
    }
    // Reset active jobs counter
    active_batch_jobs = 0;
}

void ScatterGather::wait_for_all_jobs() {
    for (auto job : jobs) {
        dml_status_t status = dml_wait_job(job, DML_WAIT_MODE_BUSY_POLL);
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

#endif // SCATTER_GATHER_H