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
    ~ScatterGather();
    void gather_data(Schema& schema, void *out, size_t* out_size);
    // compatibility with older versions
    int GatherWithMemCpy(const Schema& schema, uint8_t* out, size_t* out_size);
    int ScatterWithMemCpy(const uint8_t* in, const Schema& schema);
    void UpdateScatterSchema(Schema& schema, std::vector<size_t> sizes);

private:
    dml_job_t* job;
    dml_status_t initialize_dml_job(dml_path_t execution_path);
    void cleanup_dml_job();
};

#endif // SCATTER_GATHER_H