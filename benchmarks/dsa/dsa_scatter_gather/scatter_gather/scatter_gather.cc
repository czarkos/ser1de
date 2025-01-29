#include "scatter_gather.h"
#include <cstdio>
#include <cstring>
#include <stdlib.h>

ScatterGather::ScatterGather(dml_path_t execution_path) {
    initialize_dml_job(execution_path);
}

ScatterGather::~ScatterGather() {
    cleanup_dml_job();
}

dml_status_t ScatterGather::initialize_dml_job(dml_path_t execution_path) {
    uint32_t size = 0u;
    dml_status_t status = dml_get_job_size(execution_path, &size);
    if (DML_STATUS_OK != status) {
        fprintf(stderr, "An error (%u) occurred during getting job size.\n", status);
    }

    job = (dml_job_t *)malloc(size);
    status = dml_init_job(execution_path, job);
    if (status != DML_STATUS_OK) {
        fprintf(stderr, "Failed to create DML job with code %d\n", status);
    }
    return status;
}

void ScatterGather::cleanup_dml_job() {
    dml_status_t status = dml_finalize_job(job);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job finalization.\n", status);
        free(job);
    }
    free(job);
}

void ScatterGather::dsa_gather_blocking(Schema& schema, void *out, size_t* out_size) {
    dml_status_t status;            // DML operation status
    size_t offset = 0;              // Offset in the destination buffer
    size_t num_tuples = schema.size(); // Number of tuples to gather

    for (size_t i = 0; i < num_tuples; ++i) {
        // Configure the memory move operation
        job->operation = DML_OP_MEM_MOVE;
        job->source_first_ptr = std::get<0>(schema[i]); // Pointer from the tuple
        job->destination_first_ptr = static_cast<uint8_t*>(out) + offset;
        job->source_length = std::get<1>(schema[i]); // Size from the tuple
        job->flags = DML_FLAG_PREFETCH_CACHE; // Use prefetch cache flag

        // Execute the operation
        status = dml_execute_job(job, DML_WAIT_MODE_BUSY_POLL);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu with error code %u\n", i, status);
            break;
        }

        offset += std::get<1>(schema[i]); // Advance the destination offset
        //offset += tuples[i].size; // Advance the destination offset
    }
    *out_size = offset;
}

void ScatterGather::dsa_scatter_blocking(uint8_t* in, Schema& schema) {
    dml_status_t status;            // DML operation status
    size_t in_offset = 0;              // Offset in the destination buffer
    size_t num_tuples = schema.size(); // Number of tuples to gather

    for (size_t i = 0; i < num_tuples; ++i) {
        // Configure the memory move operation
        job->operation = DML_OP_MEM_MOVE;
        job->source_first_ptr = static_cast<uint8_t*>(in) + in_offset;
        job->destination_first_ptr = std::get<0>(schema[i]); // Pointer from the tuple
        job->source_length = std::get<1>(schema[i]); // Size from the tuple
        job->flags = DML_FLAG_PREFETCH_CACHE; // Use prefetch cache flag

        // Execute the operation
        status = dml_execute_job(job, DML_WAIT_MODE_BUSY_POLL);
        if (status != DML_STATUS_OK) {
            fprintf(stderr, "DML operation failed at tuple %zu with error code %u\n", i, status);
            break;
        }

        in_offset += std::get<1>(schema[i]); // Advance the destination offset
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