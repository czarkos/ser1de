# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build

# Include any dependencies generated for this target.
include abseil/absl/log/CMakeFiles/absl_die_if_null.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include abseil/absl/log/CMakeFiles/absl_die_if_null.dir/compiler_depend.make

# Include the progress variables for this target.
include abseil/absl/log/CMakeFiles/absl_die_if_null.dir/progress.make

# Include the compile flags for this target's objects.
include abseil/absl/log/CMakeFiles/absl_die_if_null.dir/flags.make

abseil/absl/log/CMakeFiles/absl_die_if_null.dir/die_if_null.cc.o: abseil/absl/log/CMakeFiles/absl_die_if_null.dir/flags.make
abseil/absl/log/CMakeFiles/absl_die_if_null.dir/die_if_null.cc.o: /home/christos/dev/protobuf/third_party/abseil-cpp/absl/log/die_if_null.cc
abseil/absl/log/CMakeFiles/absl_die_if_null.dir/die_if_null.cc.o: abseil/absl/log/CMakeFiles/absl_die_if_null.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object abseil/absl/log/CMakeFiles/absl_die_if_null.dir/die_if_null.cc.o"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/log && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT abseil/absl/log/CMakeFiles/absl_die_if_null.dir/die_if_null.cc.o -MF CMakeFiles/absl_die_if_null.dir/die_if_null.cc.o.d -o CMakeFiles/absl_die_if_null.dir/die_if_null.cc.o -c /home/christos/dev/protobuf/third_party/abseil-cpp/absl/log/die_if_null.cc

abseil/absl/log/CMakeFiles/absl_die_if_null.dir/die_if_null.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/absl_die_if_null.dir/die_if_null.cc.i"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/log && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/christos/dev/protobuf/third_party/abseil-cpp/absl/log/die_if_null.cc > CMakeFiles/absl_die_if_null.dir/die_if_null.cc.i

abseil/absl/log/CMakeFiles/absl_die_if_null.dir/die_if_null.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/absl_die_if_null.dir/die_if_null.cc.s"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/log && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/christos/dev/protobuf/third_party/abseil-cpp/absl/log/die_if_null.cc -o CMakeFiles/absl_die_if_null.dir/die_if_null.cc.s

# Object files for target absl_die_if_null
absl_die_if_null_OBJECTS = \
"CMakeFiles/absl_die_if_null.dir/die_if_null.cc.o"

# External object files for target absl_die_if_null
absl_die_if_null_EXTERNAL_OBJECTS =

abseil/absl/log/libabsl_die_if_null.a: abseil/absl/log/CMakeFiles/absl_die_if_null.dir/die_if_null.cc.o
abseil/absl/log/libabsl_die_if_null.a: abseil/absl/log/CMakeFiles/absl_die_if_null.dir/build.make
abseil/absl/log/libabsl_die_if_null.a: abseil/absl/log/CMakeFiles/absl_die_if_null.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libabsl_die_if_null.a"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/log && $(CMAKE_COMMAND) -P CMakeFiles/absl_die_if_null.dir/cmake_clean_target.cmake
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/log && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/absl_die_if_null.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
abseil/absl/log/CMakeFiles/absl_die_if_null.dir/build: abseil/absl/log/libabsl_die_if_null.a
.PHONY : abseil/absl/log/CMakeFiles/absl_die_if_null.dir/build

abseil/absl/log/CMakeFiles/absl_die_if_null.dir/clean:
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/log && $(CMAKE_COMMAND) -P CMakeFiles/absl_die_if_null.dir/cmake_clean.cmake
.PHONY : abseil/absl/log/CMakeFiles/absl_die_if_null.dir/clean

abseil/absl/log/CMakeFiles/absl_die_if_null.dir/depend:
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification /home/christos/dev/protobuf/third_party/abseil-cpp/absl/log /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/log /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/log/CMakeFiles/absl_die_if_null.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : abseil/absl/log/CMakeFiles/absl_die_if_null.dir/depend

