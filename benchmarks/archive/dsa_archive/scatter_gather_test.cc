#include "scatter_gather.h"

#include <chrono>
#include <iostream>
#include <vector>

static constexpr size_t nOfIterations = 10000;

// static constexpr size_t T = 1 * 1024 * 1024;
static constexpr size_t N = 128;
static constexpr size_t M = 128;

int main() {
  // Data.
  typedef std::vector<std::vector<uint8_t>> ScatteredObj;
  typedef std::vector<uint8_t> GatheredStream;

  std::vector<ScatteredObj> scattered_batch;
  for (size_t it = 0; it < nOfIterations; ++it) {
    ScatteredObj scattered_obj;
    for (size_t i = 0; i < N; ++i) {
      scattered_obj.push_back(std::vector<uint8_t>());
      scattered_obj.back().reserve(M);
      for (size_t j = 0; j < M; ++j) scattered_obj.back().push_back(it + i + j);
    }
    scattered_batch.push_back(scattered_obj);
  }

  // ScatterGather'er.
  ScatterGather stuff(Dsa::Shared, N);
  if (stuff.Init()) {
    std::cout << "Error" << std::endl;
    return -1;
  }
  std::cout << "ScatterGather is initialized and ready" << std::endl;

  //
  // Do Gather.
  //
  std::vector<GatheredStream> out(nOfIterations, GatheredStream(N * M, 0));
  // <-------------- time start
  auto begin = std::chrono::steady_clock::now();
  for (size_t it = 0; it < scattered_batch.size(); ++it) {
    ScatterGather::Schema schema;
    schema.reserve(N);
    for (const auto& el : scattered_batch[it]) {
      schema.push_back(std::make_tuple(el.data(), el.size()));
    }
    size_t out_size = 0;
    if (stuff.Gather(schema, out[it].data(), &out_size)) {
      std::cout << "Failed to gather" << std::endl;
      return -1;
    }
    assert(out[it].size() == out_size);
  }
  auto end = std::chrono::steady_clock::now();
  auto took_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
  std::cout << "gather took = " << took_ns / scattered_batch.size()
            << std::endl;
  // -----------------> time stop

  std::cout << "Gather done" << std::endl;

  //
  // Do Scatter.
  //
  std::vector<ScatteredObj> scattered_out_batch;
  for (size_t it = 0; it < nOfIterations; ++it) {
    ScatteredObj scattered_obj;
    for (size_t i = 0; i < N; ++i)
      scattered_obj.push_back(std::vector<uint8_t>(M, 1));
    scattered_out_batch.push_back(scattered_obj);
  }

  // <-------------- time start
  begin = std::chrono::steady_clock::now();
  for (size_t it = 0; it < scattered_out_batch.size(); ++it) {
    ScatterGather::Schema schema;
    schema.reserve(N);
    for (const auto& el : scattered_out_batch[it]) {
      schema.push_back(std::make_tuple(el.data(), el.size()));
    }
    if (stuff.Scatter(out[it].data(), schema)) {
      std::cout << "Failed to scatter" << std::endl;
      return -1;
    }
  }
  end = std::chrono::steady_clock::now();
  took_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
  std::cout << "scatter took = " << took_ns / scattered_out_batch.size()
            << std::endl;
  // -----------------> time stop

  std::cout << "Scatter done" << std::endl;

  //
  // Verify.
  //
  bool correct = true;
  for (size_t it = 0; it < nOfIterations && correct; ++it) {
    for (size_t i = 0; i < N && correct; ++i) {
      for (size_t j = 0; j < M && correct; ++j) {
        if (scattered_out_batch[it][i][j] != scattered_batch[it][i][j])
          correct = false;
      }
    }
  }
  std::cout << (correct ? "ALL GOOD" : "DATA MISSMATCH") << std::endl;

  return 0;
}