import matplotlib.pyplot as plt
import re
import numpy as np
import argparse
import subprocess
import os
import sys
from pathlib import Path
from collections import defaultdict

def run_benchmark(num_runs, output_file):
    """Run the benchmark for the specified number of runs and save results to output_file"""
    print(f"Running benchmark for {num_runs} runs...")
    
    # Get the directory of this script
    script_dir = Path(__file__).parent
    benchmark_dir = script_dir.parent
    benchmark_executable = benchmark_dir / "build" / "test"
    
    if not benchmark_executable.exists():
        print(f"Error: Benchmark executable not found at {benchmark_executable}")
        print("Please make sure the benchmark is compiled by running 'cmake --build build' in the benchmark directory")
        sys.exit(1)
    
    with open(output_file, 'w') as f:
        for run_num in range(1, num_runs + 1):
            print(f"Running benchmark iteration {run_num}/{num_runs}...")
            f.write(f"=== RUN {run_num} ===\n")
            
            try:
                # Run the benchmark and capture output
                result = subprocess.run([str(benchmark_executable)], 
                                     capture_output=True, 
                                     text=True, 
                                     cwd=benchmark_dir,
                                     timeout=300)  # 5 minute timeout
                
                if result.returncode != 0:
                    print(f"Warning: Benchmark run {run_num} failed with return code {result.returncode}")
                    if result.stderr:
                        print(f"Error output: {result.stderr}")
                    continue
                
                f.write(result.stdout)
                f.write("\n\n")
                
            except subprocess.TimeoutExpired:
                print(f"Warning: Benchmark run {run_num} timed out")
                continue
            except Exception as e:
                print(f"Error running benchmark: {e}")
                sys.exit(1)
    
    print(f"Benchmark completed. Results saved to {output_file}")

def parse_and_plot(input_file, max_size_bytes=None):
    """Parse the benchmark results and generate plots"""
    print(f"Parsing results from {input_file} and generating plots...")
    
    # Use dictionaries to collect all data points by flat object size
    protobuf_data = defaultdict(lambda: {'times': [], 'packets': []})
    ser1de_data = defaultdict(lambda: {'times': [], 'packets': []})

    # Read and parse the results file
    try:
        with open(input_file, 'r') as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"Error: Input file {input_file} not found")
        sys.exit(1)
        
    current_flat_size = None
    parsed_count = 0
    
    for i, line in enumerate(lines):
        line = line.strip()
        
        # Parse flat object size from lines like "		Message:0, Setter:1, Flat Object Size: 3881bytes"
        if 'Flat Object Size:' in line:
            size_match = re.search(r'Flat Object Size:\s*(\d+)bytes', line)
            if size_match:
                current_flat_size = int(size_match.group(1))
                
                # Skip this message if it exceeds the maximum size (if specified)
                if max_size_bytes is not None and current_flat_size > max_size_bytes:
                    current_flat_size = None  # Mark as skipped
                
        elif line.startswith('Protobuf:') and current_flat_size is not None:
            # Parse Protobuf line: "Protobuf:	12365ns, 85885ns, 98250ns, 2170bytes"
            try:
                # Remove the "Protobuf:" prefix and any leading/trailing whitespace
                data_part = line.split(':', 1)[1].strip()
                parts = [p.strip() for p in data_part.split(',')]
                
                if len(parts) >= 4:
                    time_ns = float(parts[2].replace('ns', ''))  # End2End time (3rd value)
                    time_us = time_ns / 1000.0  # Convert to microseconds
                    packet_size = int(parts[3].replace('bytes', ''))  # Packet size (4th value)
                    
                    protobuf_data[current_flat_size]['times'].append(time_us)
                    protobuf_data[current_flat_size]['packets'].append(packet_size)
                    parsed_count += 1
                else:
                    print(f"Warning: Could not parse Protobuf line (insufficient parts): {line}")
                    
            except (IndexError, ValueError) as e:
                print(f"Warning: Could not parse Protobuf line: {line} - Error: {e}")
                continue
                
        elif line.startswith('Ser1de:') and current_flat_size is not None:
            # Parse Ser1de line: "Ser1de:		31356ns, 34881ns, 66237ns, 2216bytes"
            try:
                # Remove the "Ser1de:" prefix and any leading/trailing whitespace
                data_part = line.split(':', 1)[1].strip()
                parts = [p.strip() for p in data_part.split(',')]
                
                if len(parts) >= 4:
                    time_ns = float(parts[2].replace('ns', ''))  # End2End time (3rd value)
                    time_us = time_ns / 1000.0  # Convert to microseconds
                    packet_size = int(parts[3].replace('bytes', ''))  # Packet size (4th value)
                    
                    ser1de_data[current_flat_size]['times'].append(time_us)
                    ser1de_data[current_flat_size]['packets'].append(packet_size)
                    parsed_count += 1
                else:
                    print(f"Warning: Could not parse Ser1de line (insufficient parts): {line}")
                    
            except (IndexError, ValueError) as e:
                print(f"Warning: Could not parse Ser1de line: {line} - Error: {e}")
                continue

    if not protobuf_data or not ser1de_data:
        print("Error: No data found in the input file. Please check the file format.")
        print(f"Total lines processed: {len(lines)}")
        print(f"Total data points parsed: {parsed_count}")
        sys.exit(1)

    # Calculate averages for each flat object size
    protobuf_sizes = []
    protobuf_times_avg = []
    protobuf_packets_avg = []
    protobuf_times_std = []
    protobuf_packets_std = []
    
    for size in sorted(protobuf_data.keys()):
        times = protobuf_data[size]['times']
        packets = protobuf_data[size]['packets']
        
        protobuf_sizes.append(size)
        protobuf_times_avg.append(np.mean(times))
        protobuf_packets_avg.append(np.mean(packets))
        protobuf_times_std.append(np.std(times))
        protobuf_packets_std.append(np.std(packets))
        
        print(f"Protobuf - Size: {size}bytes, Avg Time: {np.mean(times):.2f}μs ±{np.std(times):.2f}, Avg Packet: {np.mean(packets):.0f}bytes ±{np.std(packets):.0f} ({len(times)} runs)")

    ser1de_sizes = []
    ser1de_times_avg = []
    ser1de_packets_avg = []
    ser1de_times_std = []
    ser1de_packets_std = []
    
    for size in sorted(ser1de_data.keys()):
        times = ser1de_data[size]['times']
        packets = ser1de_data[size]['packets']
        
        ser1de_sizes.append(size)
        ser1de_times_avg.append(np.mean(times))
        ser1de_packets_avg.append(np.mean(packets))
        ser1de_times_std.append(np.std(times))
        ser1de_packets_std.append(np.std(packets))
        
        print(f"Ser1de - Size: {size}bytes, Avg Time: {np.mean(times):.2f}μs ±{np.std(times):.2f}, Avg Packet: {np.mean(packets):.0f}bytes ±{np.std(packets):.0f} ({len(times)} runs)")

    print(f"Successfully processed {len(protobuf_sizes)} unique object sizes for Protobuf and {len(ser1de_sizes)} for Ser1de")

    # Build plot title with size filter info
    title_suffix = f" (≤ {max_size_bytes} bytes)" if max_size_bytes is not None else ""

    # Create the first plot (Serialization Time) with error bars
    plt.figure(figsize=(12, 6))
    plt.axes([0.125, 0.15, 0.75, 0.75])
    
    # Plot with error bars
    plt.errorbar(protobuf_sizes, protobuf_times_avg, yerr=protobuf_times_std, 
                label='Protobuf', color='blue', marker='o', capsize=5)
    plt.errorbar(ser1de_sizes, ser1de_times_avg, yerr=ser1de_times_std, 
                label='SERenaDE', color='orange', marker='s', capsize=5)

    plt.xlabel('Flat Object Size (bytes)', fontsize=22)
    plt.ylabel('Serialization + Deserialization Time (μs)', fontsize=22)
    #plt.title(f'Performance Comparison: Protobuf vs SERenaDE{title_suffix}', fontsize=26)
    plt.title(f'End-to-End Performance Comparison', fontsize=26)
    plt.legend(fontsize=22)
    plt.grid(True, alpha=0.3)
    plt.margins(0.05)

    # Set custom x-axis ticks
    plt.gca().minorticks_off()
    plt.xticks(fontsize=22, rotation=0)
    plt.yticks(fontsize=22, rotation=0)

    plt.savefig('size_time.pdf', bbox_inches='tight', pad_inches=0.2)
    plt.close()
    print("Saved performance plot to size_time.pdf")

    # Create the second plot (Packet Size) with error bars
    plt.figure(figsize=(12, 6))
    plt.axes([0.125, 0.15, 0.75, 0.75])
    
    # Plot with error bars
    plt.errorbar(protobuf_sizes, protobuf_packets_avg, yerr=protobuf_packets_std, 
                label='Protobuf', color='blue', marker='o', capsize=5)
    plt.errorbar(ser1de_sizes, ser1de_packets_avg, yerr=ser1de_packets_std, 
                label='SERenaDE', color='orange', marker='s', capsize=5)

    plt.xlabel('Flat Object Size (bytes)', fontsize=22)
    plt.ylabel('Average Packet Size (bytes)', fontsize=22)
    #plt.title(f'Size Comparison: Protobuf vs SERenaDE{title_suffix}', fontsize=26)
    plt.title(f'Compression Rate Comparison', fontsize=26)
    plt.legend(fontsize=22)
    plt.grid(True, alpha=0.3)
    plt.margins(0.05)

    # Set custom x-axis ticks
    plt.gca().minorticks_off()
    plt.xticks(fontsize=22, rotation=0)
    plt.yticks(fontsize=22, rotation=0)

    plt.savefig('compress_rate.pdf', bbox_inches='tight', pad_inches=0.2)
    plt.close()
    print("Saved compression plot to compress_rate.pdf")

def main():
    # Add command-line argument parsing
    parser = argparse.ArgumentParser(description='Run benchmark, parse results, and generate plots')
    
    # Benchmark running options
    parser.add_argument('--run-benchmark', action='store_true',
                        help='Run the benchmark before plotting')
    parser.add_argument('--num-runs', type=int, default=10,
                        help='Number of benchmark runs to execute (default: 10)')
    parser.add_argument('--output-file', type=str, default='benchmark_results.txt',
                        help='Output file for benchmark results (default: benchmark_results.txt)')
    
    # Plotting options
    parser.add_argument('--input-file', type=str, default='benchmark_results.txt',
                        help='Input file with benchmark results to plot (default: benchmark_results.txt)')
    parser.add_argument('--max-size-bytes', type=int, default=None, 
                        help='Maximum flat object size in bytes to include (e.g., 512000 for 500KB)')
    parser.add_argument('--plot-only', action='store_true',
                        help='Only generate plots from existing results file')
    
    args = parser.parse_args()
    
    if args.run_benchmark:
        # Run the benchmark
        run_benchmark(args.num_runs, args.output_file)
        
        # Use the output file as input for plotting unless plot-only is specified
        if not args.plot_only:
            input_file = args.output_file
        else:
            input_file = args.input_file
    else:
        # Use the specified input file for plotting
        input_file = args.input_file
    
    # Generate plots unless we're only running benchmarks
    if not (args.run_benchmark and args.plot_only):
        parse_and_plot(input_file, args.max_size_bytes)

if __name__ == "__main__":
    main()