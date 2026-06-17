cmake_minimum_required(VERSION 3.16)
project(cpu_util_icache CXX)

# Match the build flags used by the sibling fleetbench_throughput benchmark.
add_definitions(-std=c++14 -O3 -march=native)
add_definitions(-Wall -Wextra -Wformat -Wformat-security)
add_definitions(-fstack-protector -fPIE -fPIC)
add_definitions(-D_FORTIFY_SOURCE=2)
add_definitions(-g)

# Sibling benchmark that owns the generated protobuf code and access headers.
# These are referenced READ-ONLY; nothing outside this directory is modified.
set(FB ${CMAKE_CURRENT_SOURCE_DIR}/../fleetbench_throughput)

find_package(GTest CONFIG REQUIRED)

# Abseil (shipped with the protobuf checkout).
add_subdirectory($ENV{PROTO_PATH}/third_party/abseil-cpp abseil)

# Protobuf.
include_directories($ENV{PROTO_PATH}/src/)
link_directories($ENV{PROTO_PATH})
link_directories($ENV{PROTO_PATH}/third_party/utf8_range)
link_directories($ENV{PROTO_PATH}/third_party/abseil-cpp)

# QPL (hardware-offloaded compression used by ser1de).
link_directories($ENV{QPL_PATH}/lib)
include_directories($ENV{QPL_PATH}/include/)

# ser1de library headers.
include_directories($ENV{SER1DE_LIB_PATH})

# Generated protobuf code + access helpers from the sibling benchmark.
# The generated .pb.cc files include "protos/MessageN.pb.h", so both the
# parent (${FB}/protos) and inner (${FB}/protos/protos) dirs are needed.
include_directories(${FB}/protos)
include_directories(${FB}/protos/protos)
include_directories(${FB}/src)

# This benchmark's own sources.
include_directories(src)

set(protobuf_ABSL_USED_TARGETS
    absl::absl_check
    absl::absl_log
    absl::algorithm
    absl::base
    absl::bind_front
    absl::bits
    absl::btree
    absl::cleanup
    absl::cord
    absl::core_headers
    absl::debugging
    absl::die_if_null
    absl::dynamic_annotations
    absl::flags
    absl::flat_hash_map
    absl::flat_hash_set
    absl::function_ref
    absl::hash
    absl::layout
    absl::log_initialize
    absl::log_severity
    absl::memory
    absl::node_hash_map
    absl::node_hash_set
    absl::optional
    absl::span
    absl::status
    absl::statusor
    absl::strings
    absl::synchronization
    absl::time
    absl::type_traits
    absl::utility
    absl::variant
)

add_executable(bench
    src/benchmark_cpu_icache.cc
    ${FB}/protos/protos/Message0.pb.cc
    ${FB}/protos/protos/Message1.pb.cc
    ${FB}/protos/protos/Message2.pb.cc
    ${FB}/protos/protos/Message3.pb.cc
    ${FB}/protos/protos/Message4.pb.cc
    ${FB}/protos/protos/Message5.pb.cc
    ${FB}/protos/protos/Message6.pb.cc
    ${FB}/protos/protos/Message7.pb.cc
    ${FB}/protos/protos/Message8.pb.cc
    ${FB}/protos/protos/Message9.pb.cc
)
target_link_libraries(bench protobuf ${protobuf_ABSL_USED_TARGETS} utf8_validity qpl)
