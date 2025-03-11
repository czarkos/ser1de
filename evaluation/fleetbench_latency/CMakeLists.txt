cmake_minimum_required (VERSION 3.16)

# gcc setup
#add_definitions(-std=c++17 -O3 -march=native)
add_definitions(-std=c++14 -O0 -march=native)
add_definitions(-Wall -Wextra -Wformat -Wformat-security)
# add_definitions(-pedantic)
add_definitions(-fstack-protector -fPIE -fPIC)
add_definitions(-D_FORTIFY_SOURCE=2)
add_definitions(-g)

#
find_package(GTest CONFIG REQUIRED)

# Add abseil
add_subdirectory($ENV{PROTO_PATH}/third_party/abseil-cpp abseil)

# Add Protobuf
include_directories($ENV{PROTO_PATH}/src/)
link_directories($ENV{PROTO_PATH})
link_directories($ENV{PROTO_PATH}/third_party/utf8_range)
link_directories($ENV{PROTO_PATH}/third_party/abseil-cpp)

# add qpl link directory
link_directories($ENV{QPL_PATH}/lib)
# link_directories($ENV{DML_PATH}/lib)

include_directories($ENV{SER1DE_LIB_PATH})
include_directories(exp/stubs)
include_directories(exp/stubs/exp)

# add qpl include directory
include_directories($ENV{QPL_PATH}/include/)
# include_directories($ENV{DML_PATH}/include/)

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

add_executable(test src/benchmark_ser1de.cc exp/stubs/exp/Message0.pb.cc exp/stubs/exp/Message1.pb.cc exp/stubs/exp/Message2.pb.cc exp/stubs/exp/Message3.pb.cc exp/stubs/exp/Message4.pb.cc exp/stubs/exp/Message5.pb.cc exp/stubs/exp/Message6.pb.cc exp/stubs/exp/Message7.pb.cc exp/stubs/exp/Message8.pb.cc exp/stubs/exp/Message9.pb.cc)
#add_executable(test src/proto_init_src.cc src/benchmark_ser1de.cc exp/stubs/exp/person.pb.cc src/scatter_gather/scatter_gather.cc)
target_link_libraries(test protobuf ${protobuf_ABSL_USED_TARGETS} utf8_validity qpl)# dml)
