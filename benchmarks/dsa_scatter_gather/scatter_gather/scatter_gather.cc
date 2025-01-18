#include "scatter_gather.h"
#include <cstdio>
#include <stdlib.h>

ScatterGather::ScatterGather() {
    initialize_dml_job();
}

ScatterGather::~ScatterGather() {
    cleanup_dml_job();
}

dml_status_t ScatterGather::initialize_dml_job() {
    uint32_t size = 0u;
    dml_status_t status = dml_get_job_size(DML_PATH_HW, &size);
    if (DML_STATUS_OK != status) {
        fprintf(stderr, "An error (%u) occurred during getting job size.\n", status);
    }

    job = (dml_job_t *)malloc(size);
    status = dml_init_job(DML_PATH_HW, job);
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

void ScatterGather::gather_data(Schema& schema, void *out, size_t* out_size) {
    dml_status_t status;            // DML operation status
    size_t offset = 0;              // Offset in the destination buffer
    size_t num_tuples = schema.size(); // Number of tuples to gather

    for (size_t i = 0; i < num_tuples; ++i) {
        // Configure the memory move operation
        job->operation = DML_OP_MEM_MOVE;
        job->source_first_ptr = std::get<0>(schema[i]); // Pointer from the tuple
        //job->source_first_ptr = tuples[i].ptr; // Pointer from the tuple
        job->destination_first_ptr = static_cast<uint8_t*>(out) + offset;
        job->source_length = std::get<1>(schema[i]); // Size from the tuple
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