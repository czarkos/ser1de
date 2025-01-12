#ifndef _SCATTER_GATHER_H_
#define _SCATTER_GATHER_H_

#include <cassert>
#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

class ScatterGather {
 public:
  // [<addr, size>]
  typedef std::vector<std::tuple<uint8_t*, size_t>> Schema;

  int GatherWithMemCpy(const Schema& schema, uint8_t* out, size_t* out_size) {
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

  int ScatterWithMemCpy(const uint8_t* in, const Schema& schema) {
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

  void UpdateScatterSchema(Schema& schema, std::vector<size_t> sizes) {
      if (sizes.size() != schema.size()) {
          std::cout << "discrepency in schema in sizes\n";
          for (auto size: sizes) std::cout << size << " ";
          std::cout << std::endl;
          exit(1);
      }

      for (size_t i = 0; i < schema.size(); i++) {
          auto &elem_size = std::get<1>(schema[i]);
          //std::cout << "updating scatter schema from size " << std::get<1>(schema[i]);
          elem_size = sizes[i];
          //std::cout << " to size " << std::get<1>(schema[i]) << "\n";
      }
  }

};

#endif
