#ifndef SCATTER_GATHER_H
#define SCATTER_GATHER_H

#include <dml/dml.h>
#include <vector>
#include <tuple>
#include <cstddef>

class ScatterGather {
public:
    typedef std::vector<std::tuple<uint8_t*, size_t>> Schema;
    ScatterGather(dml_path_t execution_path);
    ScatterGather(dml_path_t execution_path, size_t num_jobs);
    ~ScatterGather();
    void wait_for_all_dsa_jobs();
    void gather_data_blocking(Schema& schema, void *out, size_t* out_size);
    void gather_data_non_blocking(Schema& schema, void *out, size_t* out_size);
    void gather_data_batch(Schema& schema, void *out, size_t* out_size);
    int GatherWithMemCpy(const Schema& schema, uint8_t* out, size_t* out_size);
    int ScatterWithMemCpy(const uint8_t* in, const Schema& schema);
    void UpdateScatterSchema(Schema& schema, std::vector<size_t> sizes);

private:
    std::vector<dml_job_t*> jobs;
    dml_job_t* batch_job;
    size_t last_job_index;
    size_t total_jobs;
    dml_status_t initialize_dml_jobs(dml_path_t execution_path, size_t num_jobs);
    void cleanup_dml_job(dml_job_t* job);
};

#endif // SCATTER_GATHER_H