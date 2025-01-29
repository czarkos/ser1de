# if you want to compile with the new or old lib, you need to (un)comment the corresponding line in the add_executable command
cmake_minimum_required (VERSION 3.16)

project(IAABenchmark)

# gcc setup
#add_definitions(-std=c++17 -O3 -march=native)
add_definitions(-std=c++14 -O0 -march=native)
add_definitions(-Wall -Wextra -Wformat -Wformat-security)
# add_definitions(-pedantic)
add_definitions(-fstack-protector -fPIE -fPIC)
add_definitions(-D_FORTIFY_SOURCE=2)
add_definitions(-g)

# Add the include directories
include_directories($ENV{SER1DE_LIB_PATH})
include_directories($ENV{QPL_PATH}/include/)

# Add the QPL library
link_directories($ENV{QPL_PATH}/lib)

# Add the executable
add_executable(benchmark main.cc)

# Link the QPL library
target_link_libraries(benchmark qpl)

# Set C++ standard
# set_target_properties(benchmark PROPERTIES CXX_STANDARD 11)