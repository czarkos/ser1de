import subprocess
import os
import sys
import re
import matplotlib.pyplot as plt

ser1de_path = '/home/christos/dev/ser1de/'
protobuf_path = '/home/christos/dev/protobuf/'
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ser1de_path + 'scripts'))

from proto_generator import *

benchmark_path = ser1de_path + 'benchmarks/allocation_dev/'
benchmark_tmpl_path = benchmark_path + "src/proto_init.cc"
##benchmark_tmpl_path = benchmark_path + "src/main.cc"
#benchmark_tmpl_path = benchmark_path + "src/one_buffer.cc"
#benchmark_tmpl_path = benchmark_path + "src/one_optimal_buffer.cc"
#benchmark_tmpl_path = benchmark_path + "src/optimal_buffer.cc"
#benchmark_source_path = benchmark_path + "src/main_src.cc"
benchmark_source_path = benchmark_path + "src/proto_init_src.cc"

import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-p", "--proto", action="store_true", help="Build with protoc")
parser.add_argument("-ha", "--hacky", action="store_true", help="Generate setters/getters/schemas with hacky script")
parser.add_argument("-b", "--build", action="store_true", help="Build protoc output with make")
parser.add_argument("-r", "--run", action="store_true", help="Run tests")
parser.add_argument("-s", "--string", action="store_true", help="Include strings in .proto")

parser.add_argument('-w', '--width', type=int, default=1, help='Number of nested messages within a parent message.')
parser.add_argument('-d', '--depth', type=int, default=1, help='Depth of nested messages.')
parser.add_argument('-nv', '--num-varints', type=int, default=4, help='Number of varints in each message.')
parser.add_argument('-ns', '--num-strings', type=int, default=1, help='Number of strings in each message.')
args = parser.parse_args()

print("Width : ", args.width)
print("Depth : ", args.depth)
print("#varints: ", args.num_varints)
print("#strings: ", args.num_strings)
print("******************************************************")

if args.proto:
    # Generate .proto
    if not os.path.exists('exp'):
        os.makedirs('exp')
        print("Created exp/ directory")
    with open(f'exp/person.proto', 'w') as f:
        f.write(NestedMessageGenerator.generate_message(args.num_varints, args.num_strings, args.depth, args.width, args.string))
    print("Generated person.proto")

    if not os.path.exists('exp/stubs'):
        os.makedirs('exp/stubs')
        print("Created exp/stubs/ directory")
    # Compile proto
    res = subprocess.run(f'{protobuf_path}/protoc exp/person.proto --cpp_out=exp/stubs', shell=True, text=True, capture_output=True)
    if res.returncode != 0:
        print("Failed to compile proto: ", res.stderr)

    print("Generated person.pb.{cc,h} via protoc")

if args.hacky:
    # Generate setters.
    setters = NestedMessageGenerator.generate_setters(args.num_varints, args.num_strings, args.depth, args.width, args.string)
    gather_schema = NestedMessageGenerator.generate_gather_schema(args.num_varints, args.depth, args.width)
    scatter_schema = NestedMessageGenerator.generate_scatter_schema(args.num_varints, args.depth, args.width)
    with open(benchmark_tmpl_path, 'r') as f:
            lines = f.readlines()

    for j, line in enumerate(lines):
        if "<------------ SETTERS ------>" in line:
            lines.insert(j + 1, setters)
            break
    for j, line in enumerate(lines):
        if "<------------ GATHER SCHEMA ------>" in line:
            lines.insert(j + 1, gather_schema)
            break
    for j, line in enumerate(lines):
        if "<------------ SCATTER SCHEMA ------>" in line:
            lines.insert(j + 1, scatter_schema)
            break

    with open(benchmark_source_path, 'w') as f:
        f.writelines(lines)

    print("Finished hacky setter/schema generation")
else:
    # Generate setters.
    setters = NestedMessageGenerator.generate_setters(args.num_varints, args.num_strings, args.depth, args.width, args.string)
    string_setters = NestedMessageGenerator.generate_only_string_setters(args.num_varints, args.num_strings, args.depth, args.width, args.string)
    with open(benchmark_tmpl_path, 'r') as f:
            lines = f.readlines()

    for j, line in enumerate(lines):
        if "<------------ SETTERS ------>" in line:
            lines.insert(j + 1, setters)
            break
    for j, line in enumerate(lines):
        if "<------------ NUM STRINGS ------>" in line:
            #lines.insert(j + 1, f'\tnum_strings = {NUM_STRINGS};\n')
            lines.insert(j + 1, f'\tnum_strings = {args.num_strings};\n')
            break
    for j, line in enumerate(lines):
        if "<------------ STRING SETTERS ------>" in line:
            lines.insert(j + 1, string_setters)
            break
    for j, line in enumerate(lines):
        if "<------------ GATHER SCHEMA ------>" in line:
            lines.insert(j + 1, "\t\tmessages[i].generate_schema(gather_schema);\n")
            lines.insert(j + 2, "\t\tmessages[i].generate_scatter_sizes(sizes);\n")
            break
    for j, line in enumerate(lines):
        if "<------------ SCATTER SCHEMA ------>" in line:
            lines.insert(j + 1, "\t\tout_messages[i].generate_schema(scatter_schema);\n")
            break

    with open(benchmark_source_path, 'w') as f:
        f.writelines(lines)

    print("Finished setter/schema generation via protoc (our changes)")


if args.build:
    # Build benchmark
    if not os.path.exists('build'):
        os.makedirs('build')
        res = subprocess.run(f'cmake ..', cwd=benchmark_path + 'build/', shell=True, text=True, capture_output=True)
        print("Created build/ directory and built benchmark using CMake")
    res = subprocess.run(f'make -j', cwd=benchmark_path + 'build/', shell=True, text=True, capture_output=True)
    if res.returncode != 0:
        print("Failed to build benchmark: ", res.stderr)
    else:
        print("Build successful.")


if args.run:
    # Run benchmark
    res = subprocess.run(f'sudo ./test', cwd=benchmark_path + 'build/', shell=True, text=True, capture_output=True)
    result = res.stdout
    print(result)
    if res.returncode != 0:
        print("Problem running benchmark: ", res.stderr)
