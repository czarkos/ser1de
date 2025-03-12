import subprocess
import os
import sys
import re
import matplotlib.pyplot as plt

ser1de_path = '/home/christos/dev/ser1de/'
protobuf_path = '/home/christos/dev/protobuf/'
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ser1de_path + 'scripts'))

from proto_generator import *

benchmark_path = ser1de_path + 'evaluation/fleetbench_compatibility/'
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
    if not os.path.exists('exp/stubs'):
        os.makedirs('exp/stubs')
        print("Created exp/stubs/ directory")
    # Compile proto
    res = subprocess.run(f'{protobuf_path}/protoc exp/Message0.proto --cpp_out=exp/stubs', shell=True, text=True, capture_output=True)
    res = subprocess.run(f'{protobuf_path}/protoc exp/Message1.proto --cpp_out=exp/stubs', shell=True, text=True, capture_output=True)
    res = subprocess.run(f'{protobuf_path}/protoc exp/Message2.proto --cpp_out=exp/stubs', shell=True, text=True, capture_output=True)
    res = subprocess.run(f'{protobuf_path}/protoc exp/Message3.proto --cpp_out=exp/stubs', shell=True, text=True, capture_output=True)
    res = subprocess.run(f'{protobuf_path}/protoc exp/Message4.proto --cpp_out=exp/stubs', shell=True, text=True, capture_output=True)
    res = subprocess.run(f'{protobuf_path}/protoc exp/Message5.proto --cpp_out=exp/stubs', shell=True, text=True, capture_output=True)
    res = subprocess.run(f'{protobuf_path}/protoc exp/Message6.proto --cpp_out=exp/stubs', shell=True, text=True, capture_output=True)
    res = subprocess.run(f'{protobuf_path}/protoc exp/Message7.proto --cpp_out=exp/stubs', shell=True, text=True, capture_output=True)
    res = subprocess.run(f'{protobuf_path}/protoc exp/Message8.proto --cpp_out=exp/stubs', shell=True, text=True, capture_output=True)
    res = subprocess.run(f'{protobuf_path}/protoc exp/Message9.proto --cpp_out=exp/stubs', shell=True, text=True, capture_output=True)
    if res.returncode != 0:
        print("Failed to compile proto: ", res.stderr)

    print("Generated person.pb.{cc,h} via protoc")

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
    res = subprocess.run(f'sudo taskset -c 0 ./test', cwd=benchmark_path + 'build/', shell=True, text=True, capture_output=True)
    result = res.stdout
    print(result)
    if res.returncode != 0:
        print("Problem running benchmark: ", res.stderr)
