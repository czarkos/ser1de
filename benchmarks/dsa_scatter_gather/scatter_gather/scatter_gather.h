#ifndef SCATTER_GATHER_H
#define SCATTER_GATHER_H

#include <dml/dml.h>
#include <vector>
#include <tuple>
#include <cstddef>

class ScatterGather {
public:
    typedef std::vector<std::tuple<uint8_t*, size_t>> Schema;
    ScatterGather();
    ~ScatterGather();
    void gather_data(Schema& schema, void *out, size_t* out_size);

private:
    dml_job_t* job;
    dml_status_t initialize_dml_job();
    void cleanup_dml_job();
};

#endif // SCATTER_GATHER_H