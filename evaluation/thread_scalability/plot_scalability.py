import subprocess
import multiprocessing
import time
import os
import re
import matplotlib.pyplot as plt
import numpy as np
from concurrent.futures import ProcessPoolExecutor

def run_benchmark(core_id, output_file):
    """Run the benchmark on a specific core and save output to a file."""
    # Use taskset to pin the process to a specific CPU core
    # Wrap around after core 31 (0-based indexing, so cores 0-31 = 32 cores)
    physical_core = core_id % 32
    cmd = f"sudo taskset -c {physical_core} ./build/test > {output_file}"
    subprocess.run(cmd, shell=True)
    return output_file

def parse_benchmark_file(file_path):
    """Parse the benchmark output file to extract data."""
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Extract all benchmark sections
    sections = re.findall(r'Number of requests: (\d+).*?Protobuf Serialize: (\d+) ns.*?Protobuf Deserialize: (\d+) ns.*?Ser1de Serialize: (\d+) ns.*?Ser1de Deserialize: (\d+) ns', 
                         content, re.DOTALL)
    
    if not sections:
        return None
    
    # Get the last section (final result)
    last_section = sections[-1]
    
    return {
        'num_requests': int(last_section[0]),
        'protobuf_serialize': int(last_section[1]),
        'protobuf_deserialize': int(last_section[2]),
        'ser1de_serialize': int(last_section[3]),
        'ser1de_deserialize': int(last_section[4])
    }

def aggregate_results(results):
    """Aggregate results from multiple benchmark runs."""
    if not results:
        return None
    
    # Calculate throughput (requests per second) for each run
    for result in results:
        # Convert ns to seconds and calculate throughput
        result['protobuf_serialize_throughput'] = result['num_requests'] / (result['protobuf_serialize'] / 1e9)
        result['protobuf_deserialize_throughput'] = result['num_requests'] / (result['protobuf_deserialize'] / 1e9)
        result['ser1de_serialize_throughput'] = result['num_requests'] / (result['ser1de_serialize'] / 1e9)
        result['ser1de_deserialize_throughput'] = result['num_requests'] / (result['ser1de_deserialize'] / 1e9)
    
    # Aggregate throughput across all cores
    aggregate = {
        'num_cores': len(results),
        'protobuf_serialize_throughput': sum(r['protobuf_serialize_throughput'] for r in results),
        'protobuf_deserialize_throughput': sum(r['protobuf_deserialize_throughput'] for r in results),
        'ser1de_serialize_throughput': sum(r['ser1de_serialize_throughput'] for r in results),
        'ser1de_deserialize_throughput': sum(r['ser1de_deserialize_throughput'] for r in results)
    }
    
    return aggregate

def create_plots(core_counts, aggregated_results, output_dir):
    """Create and save plots showing throughput scaling with core count."""
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Extract data for plotting
    protobuf_serialize_throughputs = [r['protobuf_serialize_throughput'] for r in aggregated_results]
    protobuf_deserialize_throughputs = [r['protobuf_deserialize_throughput'] for r in aggregated_results]
    ser1de_serialize_throughputs = [r['ser1de_serialize_throughput'] for r in aggregated_results]
    ser1de_deserialize_throughputs = [r['ser1de_deserialize_throughput'] for r in aggregated_results]
    
    # Set up the figure style
    plt.rcParams.update({'font.size': 14})
    
    # Create figure 1: Serialization Throughput
    plt.figure(figsize=(12, 8))
    plt.plot(core_counts, protobuf_serialize_throughputs, 'o-', linewidth=2, markersize=8, label='Protobuf')
    plt.plot(core_counts, ser1de_serialize_throughputs, 's-', linewidth=2, markersize=8, label='SERenaDE')
    plt.xlabel('Number of Processes', fontsize=22)
    plt.ylabel('Throughput (requests/second)', fontsize=22)
    plt.title('Serialization Throughput Scaling', fontsize=26)
    plt.legend(fontsize=22)
    plt.grid(True)
    plt.xticks(core_counts, fontsize=22)
    plt.yticks(fontsize=22)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'serialization_throughput_scaling.pdf'))
    
    # Create figure 2: Deserialization Throughput
    plt.figure(figsize=(12, 8))
    plt.plot(core_counts, protobuf_deserialize_throughputs, 'o-', linewidth=2, markersize=8, label='Protobuf')
    plt.plot(core_counts, ser1de_deserialize_throughputs, 's-', linewidth=2, markersize=8, label='SERenaDE')
    plt.xlabel('Number of Processes', fontsize=22)
    plt.ylabel('Throughput (requests/second)', fontsize=22)
    plt.title('Deserialization Throughput Scaling', fontsize=26)
    plt.legend(fontsize=22)
    plt.grid(True)
    plt.xticks(core_counts, fontsize=22)
    plt.yticks(fontsize=22)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'deserialization_throughput_scaling.pdf'))
    
    # Create figure 3: Combined bar chart for maximum throughput
    plt.figure(figsize=(14, 10))
    
    max_core_count = max(core_counts)
    max_results_idx = core_counts.index(max_core_count)
    max_results = aggregated_results[max_results_idx]
    
    categories = ['Serialization', 'Deserialization']
    protobuf_values = [max_results['protobuf_serialize_throughput'], 
                       max_results['protobuf_deserialize_throughput']]
    ser1de_values = [max_results['ser1de_serialize_throughput'], 
                     max_results['ser1de_deserialize_throughput']]
    
    x = np.arange(len(categories))
    width = 0.35
    
    fig, ax = plt.subplots(figsize=(12, 8))
    rects1 = ax.bar(x - width/2, protobuf_values, width, label='Protobuf')
    rects2 = ax.bar(x + width/2, ser1de_values, width, label='SERenaDE')
    
    ax.set_ylabel('Throughput (requests/second)', fontsize=18)
    ax.set_title(f'Maximum Throughput with {max_core_count} Cores', fontsize=20)
    ax.set_xticks(x)
    ax.set_xticklabels(categories, fontsize=16)
    ax.legend(fontsize=18)
    ax.grid(True, axis='y')
    
    # Add values on top of bars
    def autolabel(rects):
        for rect in rects:
            height = rect.get_height()
            ax.annotate(f'{int(height):,}',
                        xy=(rect.get_x() + rect.get_width() / 2, height),
                        xytext=(0, 3),  # 3 points vertical offset
                        textcoords="offset points",
                        ha='center', va='bottom', fontsize=14)
    
    autolabel(rects1)
    autolabel(rects2)
    
    fig.tight_layout()
    plt.savefig(os.path.join(output_dir, 'maximum_throughput_comparison.pdf'))
    
    print(f"Plots have been saved as PDF files in the '{output_dir}' directory.")

def main():
    # Define the core counts to test
    core_counts = [1, 2, 4, 8, 12, 16, 20, 24, 28, 32, 64, 128, 256, 512, 1024]  # Adjust based on your system
    
    # Output directory for results and plots
    output_dir = 'parallel_benchmark_results'
    os.makedirs(output_dir, exist_ok=True)
    
    # Store aggregated results for each core count
    all_aggregated_results = []
    
    for num_cores in core_counts:
        print(f"Running benchmark with {num_cores} cores...")
        
        # Create a temporary directory for this run's output files
        run_dir = os.path.join(output_dir, f"run_{num_cores}_cores")
        os.makedirs(run_dir, exist_ok=True)
        
        # Run benchmarks in parallel
        output_files = []
        
        with ProcessPoolExecutor(max_workers=num_cores) as executor:
            futures = []
            for i in range(num_cores):
                output_file = os.path.join(run_dir, f"benchmark_core_{i}.txt")
                futures.append(executor.submit(run_benchmark, i, output_file))
            
            # Wait for all benchmarks to complete
            for future in futures:
                output_files.append(future.result())
        
        # Parse results
        results = []
        for file in output_files:
            result = parse_benchmark_file(file)
            if result:
                results.append(result)
        
        # Aggregate results
        if results:
            aggregated = aggregate_results(results)
            all_aggregated_results.append(aggregated)
            
            # Write summary to file
            with open(os.path.join(output_dir, f"summary_{num_cores}_cores.txt"), 'w') as f:
                f.write(f"Number of cores: {num_cores}\n")
                f.write(f"Protobuf Serialize Throughput: {aggregated['protobuf_serialize_throughput']:.2f} req/s\n")
                f.write(f"Protobuf Deserialize Throughput: {aggregated['protobuf_deserialize_throughput']:.2f} req/s\n")
                f.write(f"Ser1de Serialize Throughput: {aggregated['ser1de_serialize_throughput']:.2f} req/s\n")
                f.write(f"Ser1de Deserialize Throughput: {aggregated['ser1de_deserialize_throughput']:.2f} req/s\n")
            
            print(f"Completed benchmark with {num_cores} cores.")
            print(f"Aggregate Protobuf Serialize Throughput: {aggregated['protobuf_serialize_throughput']:.2f} req/s")
            print(f"Aggregate Protobuf Deserialize Throughput: {aggregated['protobuf_deserialize_throughput']:.2f} req/s")
            print(f"Aggregate Ser1de Serialize Throughput: {aggregated['ser1de_serialize_throughput']:.2f} req/s")
            print(f"Aggregate Ser1de Deserialize Throughput: {aggregated['ser1de_deserialize_throughput']:.2f} req/s")
            print()
    
    # Create plots
    if all_aggregated_results:
        create_plots(core_counts, all_aggregated_results, output_dir)

if __name__ == "__main__":
    main()
