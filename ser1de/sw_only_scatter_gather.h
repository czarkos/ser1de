#ifndef SCATTER_GATHER_H
#define SCATTER_GATHER_H

#include <qpl/qpl.h>
#include <vector>
#include <tuple>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <assert.h>

class ScatterGather {
public:
    ScatterGather(){}
    ~ScatterGather(){};
    int GatherWithMemCpy(const std::vector<uint8_t*>& ptrs_list, const std::vector<size_t> sizes_list, uint8_t* out, size_t* out_size);
    int ScatterWithMemCpy(const uint8_t* in, const std::vector<uint8_t*>& ptrs_list, const std::vector<size_t> sizes_list); 
private:
};

int ScatterGather::GatherWithMemCpy(const std::vector<uint8_t*>& ptrs_list, const std::vector<size_t> sizes_list, uint8_t* out, size_t* out_size) {
    size_t out_offset = 0;
    size_t num_ptrs = ptrs_list.size();
    size_t num_sizes = sizes_list.size();
    assert(num_ptrs == num_sizes);

    for (size_t i = 0; i < num_ptrs; i++) {
        if (ptrs_list[i] == nullptr) {
            continue;
        }
        memcpy(out + out_offset, ptrs_list[i], sizes_list[i]);
        out_offset += sizes_list[i];
    }
    *out_size = out_offset;

    return 0;
}

int ScatterGather::ScatterWithMemCpy(const uint8_t* in, const std::vector<uint8_t*>& ptrs_list, const std::vector<size_t> sizes_list) {
    size_t in_offset = 0;
    size_t num_ptrs = ptrs_list.size();
    size_t num_sizes = sizes_list.size();

    //std::cout << "num_ptrs " << num_ptrs << "\n";
    //std::cout << "num_sizes " << num_sizes << "\n";
    assert(num_ptrs == num_sizes);

    for (size_t i = 0; i < num_ptrs; i++) {
        if (ptrs_list[i] == nullptr) {
            continue;
        }
        memcpy(const_cast<uint8_t*>(ptrs_list[i]), in + in_offset, sizes_list[i]);
        in_offset += sizes_list[i];
    }

    return 0;
}

#endif // SCATTER_GATHER_H