cmake_minimum_required (VERSION 3.16)

# gcc setup
add_definitions(-std=c++17 -O3 -march=native)
add_definitions(-Wall -Wextra -Wformat -Wformat-security)
# add_definitions(-pedantic)
add_definitions(-fstack-protector -fPIE -fPIC)
add_definitions(-D_FORTIFY_SOURCE=2)


include_directories($ENV{IDX_SDK})

#
add_executable(scatter_gather_test scatter_gather_test.cc)
target_link_libraries(scatter_gather_test LINK_PUBLIC accel-config)