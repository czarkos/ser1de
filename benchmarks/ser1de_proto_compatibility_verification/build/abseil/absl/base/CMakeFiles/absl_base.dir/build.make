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
include abseil/absl/base/CMakeFiles/absl_base.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include abseil/absl/base/CMakeFiles/absl_base.dir/compiler_depend.make

# Include the progress variables for this target.
include abseil/absl/base/CMakeFiles/absl_base.dir/progress.make

# Include the compile flags for this target's objects.
include abseil/absl/base/CMakeFiles/absl_base.dir/flags.make

abseil/absl/base/CMakeFiles/absl_base.dir/internal/cycleclock.cc.o: abseil/absl/base/CMakeFiles/absl_base.dir/flags.make
abseil/absl/base/CMakeFiles/absl_base.dir/internal/cycleclock.cc.o: /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/cycleclock.cc
abseil/absl/base/CMakeFiles/absl_base.dir/internal/cycleclock.cc.o: abseil/absl/base/CMakeFiles/absl_base.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object abseil/absl/base/CMakeFiles/absl_base.dir/internal/cycleclock.cc.o"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT abseil/absl/base/CMakeFiles/absl_base.dir/internal/cycleclock.cc.o -MF CMakeFiles/absl_base.dir/internal/cycleclock.cc.o.d -o CMakeFiles/absl_base.dir/internal/cycleclock.cc.o -c /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/cycleclock.cc

abseil/absl/base/CMakeFiles/absl_base.dir/internal/cycleclock.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/absl_base.dir/internal/cycleclock.cc.i"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/cycleclock.cc > CMakeFiles/absl_base.dir/internal/cycleclock.cc.i

abseil/absl/base/CMakeFiles/absl_base.dir/internal/cycleclock.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/absl_base.dir/internal/cycleclock.cc.s"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/cycleclock.cc -o CMakeFiles/absl_base.dir/internal/cycleclock.cc.s

abseil/absl/base/CMakeFiles/absl_base.dir/internal/spinlock.cc.o: abseil/absl/base/CMakeFiles/absl_base.dir/flags.make
abseil/absl/base/CMakeFiles/absl_base.dir/internal/spinlock.cc.o: /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/spinlock.cc
abseil/absl/base/CMakeFiles/absl_base.dir/internal/spinlock.cc.o: abseil/absl/base/CMakeFiles/absl_base.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object abseil/absl/base/CMakeFiles/absl_base.dir/internal/spinlock.cc.o"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT abseil/absl/base/CMakeFiles/absl_base.dir/internal/spinlock.cc.o -MF CMakeFiles/absl_base.dir/internal/spinlock.cc.o.d -o CMakeFiles/absl_base.dir/internal/spinlock.cc.o -c /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/spinlock.cc

abseil/absl/base/CMakeFiles/absl_base.dir/internal/spinlock.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/absl_base.dir/internal/spinlock.cc.i"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/spinlock.cc > CMakeFiles/absl_base.dir/internal/spinlock.cc.i

abseil/absl/base/CMakeFiles/absl_base.dir/internal/spinlock.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/absl_base.dir/internal/spinlock.cc.s"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/spinlock.cc -o CMakeFiles/absl_base.dir/internal/spinlock.cc.s

abseil/absl/base/CMakeFiles/absl_base.dir/internal/sysinfo.cc.o: abseil/absl/base/CMakeFiles/absl_base.dir/flags.make
abseil/absl/base/CMakeFiles/absl_base.dir/internal/sysinfo.cc.o: /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/sysinfo.cc
abseil/absl/base/CMakeFiles/absl_base.dir/internal/sysinfo.cc.o: abseil/absl/base/CMakeFiles/absl_base.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object abseil/absl/base/CMakeFiles/absl_base.dir/internal/sysinfo.cc.o"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT abseil/absl/base/CMakeFiles/absl_base.dir/internal/sysinfo.cc.o -MF CMakeFiles/absl_base.dir/internal/sysinfo.cc.o.d -o CMakeFiles/absl_base.dir/internal/sysinfo.cc.o -c /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/sysinfo.cc

abseil/absl/base/CMakeFiles/absl_base.dir/internal/sysinfo.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/absl_base.dir/internal/sysinfo.cc.i"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/sysinfo.cc > CMakeFiles/absl_base.dir/internal/sysinfo.cc.i

abseil/absl/base/CMakeFiles/absl_base.dir/internal/sysinfo.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/absl_base.dir/internal/sysinfo.cc.s"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/sysinfo.cc -o CMakeFiles/absl_base.dir/internal/sysinfo.cc.s

abseil/absl/base/CMakeFiles/absl_base.dir/internal/thread_identity.cc.o: abseil/absl/base/CMakeFiles/absl_base.dir/flags.make
abseil/absl/base/CMakeFiles/absl_base.dir/internal/thread_identity.cc.o: /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/thread_identity.cc
abseil/absl/base/CMakeFiles/absl_base.dir/internal/thread_identity.cc.o: abseil/absl/base/CMakeFiles/absl_base.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object abseil/absl/base/CMakeFiles/absl_base.dir/internal/thread_identity.cc.o"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT abseil/absl/base/CMakeFiles/absl_base.dir/internal/thread_identity.cc.o -MF CMakeFiles/absl_base.dir/internal/thread_identity.cc.o.d -o CMakeFiles/absl_base.dir/internal/thread_identity.cc.o -c /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/thread_identity.cc

abseil/absl/base/CMakeFiles/absl_base.dir/internal/thread_identity.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/absl_base.dir/internal/thread_identity.cc.i"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/thread_identity.cc > CMakeFiles/absl_base.dir/internal/thread_identity.cc.i

abseil/absl/base/CMakeFiles/absl_base.dir/internal/thread_identity.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/absl_base.dir/internal/thread_identity.cc.s"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/thread_identity.cc -o CMakeFiles/absl_base.dir/internal/thread_identity.cc.s

abseil/absl/base/CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.o: abseil/absl/base/CMakeFiles/absl_base.dir/flags.make
abseil/absl/base/CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.o: /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/unscaledcycleclock.cc
abseil/absl/base/CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.o: abseil/absl/base/CMakeFiles/absl_base.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object abseil/absl/base/CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.o"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT abseil/absl/base/CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.o -MF CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.o.d -o CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.o -c /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/unscaledcycleclock.cc

abseil/absl/base/CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.i"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/unscaledcycleclock.cc > CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.i

abseil/absl/base/CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.s"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base/internal/unscaledcycleclock.cc -o CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.s

# Object files for target absl_base
absl_base_OBJECTS = \
"CMakeFiles/absl_base.dir/internal/cycleclock.cc.o" \
"CMakeFiles/absl_base.dir/internal/spinlock.cc.o" \
"CMakeFiles/absl_base.dir/internal/sysinfo.cc.o" \
"CMakeFiles/absl_base.dir/internal/thread_identity.cc.o" \
"CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.o"

# External object files for target absl_base
absl_base_EXTERNAL_OBJECTS =

abseil/absl/base/libabsl_base.a: abseil/absl/base/CMakeFiles/absl_base.dir/internal/cycleclock.cc.o
abseil/absl/base/libabsl_base.a: abseil/absl/base/CMakeFiles/absl_base.dir/internal/spinlock.cc.o
abseil/absl/base/libabsl_base.a: abseil/absl/base/CMakeFiles/absl_base.dir/internal/sysinfo.cc.o
abseil/absl/base/libabsl_base.a: abseil/absl/base/CMakeFiles/absl_base.dir/internal/thread_identity.cc.o
abseil/absl/base/libabsl_base.a: abseil/absl/base/CMakeFiles/absl_base.dir/internal/unscaledcycleclock.cc.o
abseil/absl/base/libabsl_base.a: abseil/absl/base/CMakeFiles/absl_base.dir/build.make
abseil/absl/base/libabsl_base.a: abseil/absl/base/CMakeFiles/absl_base.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX static library libabsl_base.a"
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && $(CMAKE_COMMAND) -P CMakeFiles/absl_base.dir/cmake_clean_target.cmake
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/absl_base.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
abseil/absl/base/CMakeFiles/absl_base.dir/build: abseil/absl/base/libabsl_base.a
.PHONY : abseil/absl/base/CMakeFiles/absl_base.dir/build

abseil/absl/base/CMakeFiles/absl_base.dir/clean:
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base && $(CMAKE_COMMAND) -P CMakeFiles/absl_base.dir/cmake_clean.cmake
.PHONY : abseil/absl/base/CMakeFiles/absl_base.dir/clean

abseil/absl/base/CMakeFiles/absl_base.dir/depend:
	cd /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification /home/christos/dev/protobuf/third_party/abseil-cpp/absl/base /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base /home/christos/dev/ser1de/benchmarks/ser1de_proto_compatibility_verification/build/abseil/absl/base/CMakeFiles/absl_base.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : abseil/absl/base/CMakeFiles/absl_base.dir/depend

