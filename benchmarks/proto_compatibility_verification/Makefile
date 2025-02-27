# Define variables
PROTOC = /home/christos/dev/protobuf/protoc
PROTO_SRC = test.proto
PROTO_GEN_DIR = ./protos
PROTO_CPP_OUT = $(PROTO_GEN_DIR)
CMAKE_BUILD_DIR = build
EXECUTABLE = test

# Default target
#all: run run_ri run_rs run_rm run_rr
all: run run_rr

# Generate C++ code from .proto file
proto: $(PROTO_GEN_DIR)/test.pb.cc

$(PROTO_GEN_DIR)/test.pb.cc: $(PROTO_SRC)
	mkdir -p $(PROTO_GEN_DIR)
	$(PROTOC) --cpp_out=$(PROTO_CPP_OUT) $(PROTO_SRC)

# Build the project using CMake
$(CMAKE_BUILD_DIR)/Makefile: $(PROTO_GEN_DIR)/test.pb.cc src/new_lib_main.cc
	mkdir -p $(CMAKE_BUILD_DIR)
	cd $(CMAKE_BUILD_DIR) && cmake ..

build/$(EXECUTABLE): $(CMAKE_BUILD_DIR)/Makefile
	cd $(CMAKE_BUILD_DIR) && make -j

# Compile the project
build: $(CMAKE_BUILD_DIR)/Makefile
	cd $(CMAKE_BUILD_DIR) && make -j

# Run the executable
run: build/$(EXECUTABLE)
	@echo "Running Singular Message Compatibility Test"
	@sudo taskset -c 0 ./$(CMAKE_BUILD_DIR)/$(EXECUTABLE)

# run with repeated message
run_rr: build/$(EXECUTABLE)
	@echo "Running Repeated Message Compatibility Test"
	@sudo taskset -c 0 ./$(CMAKE_BUILD_DIR)/$(EXECUTABLE) rr

## run with repeated ints
#run_ri: build/$(EXECUTABLE)
#	@echo "Running Repeated Primitive Field Compatibility Test"
#	@sudo taskset -c 0 ./$(CMAKE_BUILD_DIR)/$(EXECUTABLE) ri
#
## run with repeated strings
#run_rs: build/$(EXECUTABLE)
#	@echo "Running Repeated String Field Compatibility Test"
#	@sudo taskset -c 0 ./$(CMAKE_BUILD_DIR)/$(EXECUTABLE) rs
#
## run with repeated message fields
#run_rm: build/$(EXECUTABLE)
#	@echo "Running Repeated Message Field Compatibility Test"
#	@sudo taskset -c 0 ./$(CMAKE_BUILD_DIR)/$(EXECUTABLE) rm

# Clean the build directory
clean:
	rm -rf $(CMAKE_BUILD_DIR) $(PROTO_GEN_DIR)
# rm -rf $(CMAKE_BUILD_DIR) $(PROTO_GEN_DIR)/test.pb.cc $(PROTO_GEN_DIR)/test.pb.h

.PHONY: all proto build run clean