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
    void dsa_gather_blocking(Schema& schema, void *out, size_t* out_size);
    void dsa_scatter_blocking(uint8_t* in, Schema& schema);
    void dsa_gather_non_blocking(Schema& schema, void *out, size_t* out_size);
    void dsa_scatter_non_blocking(uint8_t* in, Schema& schema);
    void wait_for_all_jobs();
    int GatherWithMemCpy(const Schema& schema, uint8_t* out, size_t* out_size);
    int ScatterWithMemCpy(const uint8_t* in, const Schema& schema);
    void UpdateScatterSchema(Schema& schema, std::vector<size_t> sizes);

private:
    std::vector<dml_job_t*> jobs;
    size_t num_jobs;
    size_t current_job_index;
    dml_status_t initialize_dml_jobs(dml_path_t execution_path);
    void cleanup_dml_jobs();
    dml_job_t* get_next_job();
};

#endif // SCATTER_GATHER_H