#ifndef SCATTER_GATHER_H
#define SCATTER_GATHER_H

#include <dml/dml.h>
#include <vector>
#include <tuple>
#include <cstddef>

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
    //dml_job_t* batch_job;
    size_t batch_size;
    std::vector<uint8_t*> batch_buffers;
    std::vector<uint32_t> batch_buffer_lengths;
    //uint8_t* batch_buffer;
    //uint32_t batch_buffer_length;
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

#endif // SCATTER_GATHER_H