import matplotlib.pyplot as plt
import re
import numpy as np
import subprocess
import argparse
import os
import time
import json
from collections import defaultdict

def run_benchmark(num_runs=1):
    """Run the benchmark multiple times and return all outputs."""
    outputs = []
    
    for i in range(num_runs):
        try:
            print(f"Running benchmark iteration {i+1}/{num_runs}...")
            result = subprocess.run(f'sudo taskset -c 0 ./test', cwd='./build/', 
                                   shell=True, text=True, capture_output=True)
            if result.returncode != 0:
                print(f"Warning: Benchmark returned non-zero exit code: {result.returncode}")
                print(f"Error output: {result.stderr}")
            
            outputs.append(result.stdout)
            
            # Add a short delay between runs to allow system to stabilize
            if i < num_runs - 1:
                time.sleep(1)
                
        except subprocess.CalledProcessError as e:
            print(f"Error running benchmark: {e}")
            return None
        except FileNotFoundError:
            print("Error: 'test' program not found. Make sure it exists and is executable.")
            return None
    
    return outputs

def read_benchmark_from_file(file_path):
    try:
        with open(file_path, 'r') as f:
            return [f.read()]  # Return as a list for consistency with run_benchmark
    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.")
        return None
    except Exception as e:
        print(f"Error reading file: {e}")
        return None

def parse_benchmark_results(text):
    # Regular expression to extract data from each benchmark section
    pattern = r"---------------------------------------------------------------------------------------------------------------\n" + \
              r"\s+Message:(\d+), Setter:(\d+), Flat Object Size: (\d+)bytes\n" + \
              r"---------------------------------------------------------------------------------------------------------------\n" + \
              r"\s+Serialization, Deserialization, End2End, Packet Size\n" + \
              r"Protobuf:\s+(\d+)ns, (\d+)ns, (\d+)ns, (\d+)bytes\n" + \
              r"Ser1de:\s+(\d+)ns, (\d+)ns, (\d+)ns, (\d+)bytes\n" + \
              r"Speedups:\s+([-\d\.]+)%, ([-\d\.]+)%, ([-\d\.]+)%\n" + \
              r"Compression:\s+([-\d\.]+)%"
    
    results = []
    matches = re.finditer(pattern, text, re.DOTALL)
    
    for match in matches:
        results.append({
            'message': int(match.group(1)),
            'setter': int(match.group(2)),
            'flat_size': int(match.group(3)),
            'proto_ser': int(match.group(4)),
            'proto_deser': int(match.group(5)),
            'proto_end2end': int(match.group(6)),
            'proto_bytes': int(match.group(7)),
            'ser1de_ser': int(match.group(8)),
            'ser1de_deser': int(match.group(9)),
            'ser1de_end2end': int(match.group(10)),
            'ser1de_bytes': int(match.group(11)),
            'speedup_ser': float(match.group(12)),
            'speedup_deser': float(match.group(13)),
            'speedup_end2end': float(match.group(14)),
            'compression': float(match.group(15))
        })
    
    return results

def aggregate_multiple_runs(all_results):
    """
    Aggregate results from multiple benchmark runs.
    Returns a dictionary with average values and standard deviations.
    """
    # Group results by message and setter
    grouped_results = defaultdict(list)
    
    for run_results in all_results:
        for result in run_results:
            key = (result['message'], result['setter'])
            grouped_results[key].append(result)
    
    # Calculate averages and standard deviations
    aggregated = []
    
    for key, results in grouped_results.items():
        message, setter = key
        
        # Ensure all results have the same flat_size
        flat_sizes = set(r['flat_size'] for r in results)
        if len(flat_sizes) > 1:
            print(f"Warning: Inconsistent flat sizes for Message:{message}, Setter:{setter}")
        
        flat_size = results[0]['flat_size']  # Use the first one
        
        # Calculate averages and standard deviations
        avg_result = {
            'message': message,
            'setter': setter,
            'flat_size': flat_size,
            'proto_ser': int(np.mean([r['proto_ser'] for r in results])),
            'proto_deser': int(np.mean([r['proto_deser'] for r in results])),
            'proto_end2end': int(np.mean([r['proto_end2end'] for r in results])),
            'proto_bytes': int(np.mean([r['proto_bytes'] for r in results])),
            'ser1de_ser': int(np.mean([r['ser1de_ser'] for r in results])),
            'ser1de_deser': int(np.mean([r['ser1de_deser'] for r in results])),
            'ser1de_end2end': int(np.mean([r['ser1de_end2end'] for r in results])),
            'ser1de_bytes': int(np.mean([r['ser1de_bytes'] for r in results])),
            'speedup_ser': float(np.mean([r['speedup_ser'] for r in results])),
            'speedup_deser': float(np.mean([r['speedup_deser'] for r in results])),
            'speedup_end2end': float(np.mean([r['speedup_end2end'] for r in results])),
            'compression': float(np.mean([r['compression'] for r in results])),
            # Standard deviations
            'std_proto_ser': int(np.std([r['proto_ser'] for r in results])),
            'std_proto_deser': int(np.std([r['proto_deser'] for r in results])),
            'std_proto_end2end': int(np.std([r['proto_end2end'] for r in results])),
            'std_ser1de_ser': int(np.std([r['ser1de_ser'] for r in results])),
            'std_ser1de_deser': int(np.std([r['ser1de_deser'] for r in results])),
            'std_ser1de_end2end': int(np.std([r['ser1de_end2end'] for r in results])),
            'std_speedup_ser': float(np.std([r['speedup_ser'] for r in results])),
            'std_speedup_deser': float(np.std([r['speedup_deser'] for r in results])),
            'std_speedup_end2end': float(np.std([r['speedup_end2end'] for r in results])),
            'num_runs': len(results)
        }
        
        aggregated.append(avg_result)
    
    return aggregated

def create_serialization_plot(results):
    plt.figure(figsize=(10, 6))
    
    # Sort data points by flat_size for proper line connections
    flat_sizes = [r['flat_size'] for r in results]
    proto_ser = [r['proto_ser'] for r in results]
    ser1de_ser = [r['ser1de_ser'] for r in results]
    
    # Get standard deviations if available
    proto_std = [r.get('std_proto_ser', 0) for r in results]
    ser1de_std = [r.get('std_ser1de_ser', 0) for r in results]
    
    # Create sorted pairs for plotting
    proto_data = sorted(zip(flat_sizes, proto_ser, proto_std))
    ser1de_data = sorted(zip(flat_sizes, ser1de_ser, ser1de_std))
    
    # Unzip the sorted data
    proto_x, proto_y, proto_err = zip(*proto_data)
    ser1de_x, ser1de_y, ser1de_err = zip(*ser1de_data)
    
    # Plot with both scatter points, connecting lines, and error bars
    plt.errorbar(proto_x, proto_y, yerr=proto_err, fmt='o-', label='Protobuf', alpha=0.7, capsize=3)
    plt.errorbar(ser1de_x, ser1de_y, yerr=ser1de_err, fmt='o-', label='Ser1de', alpha=0.7, capsize=3)
    
    plt.xlabel('Flat Object Size (bytes)', fontsize=16)
    plt.ylabel('Latency (ns)', fontsize=16)
    plt.title('Serialization Performance', fontsize=20)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tick_params(axis='both', which='major', labelsize=16)
    
    # Create directory if it doesn't exist
    os.makedirs('latency_plots', exist_ok=True)
    plt.savefig('latency_plots/serialization_performance.pdf', format='pdf', dpi=300, bbox_inches='tight')
    plt.close()

def create_deserialization_plot(results):
    plt.figure(figsize=(10, 6))
    
    # Sort data points by flat_size for proper line connections
    flat_sizes = [r['flat_size'] for r in results]
    proto_deser = [r['proto_deser'] for r in results]
    ser1de_deser = [r['ser1de_deser'] for r in results]
    
    # Get standard deviations if available
    proto_std = [r.get('std_proto_deser', 0) for r in results]
    ser1de_std = [r.get('std_ser1de_deser', 0) for r in results]
    
    # Create sorted pairs for plotting
    proto_data = sorted(zip(flat_sizes, proto_deser, proto_std))
    ser1de_data = sorted(zip(flat_sizes, ser1de_deser, ser1de_std))
    
    # Unzip the sorted data
    proto_x, proto_y, proto_err = zip(*proto_data)
    ser1de_x, ser1de_y, ser1de_err = zip(*ser1de_data)
    
    # Plot with both scatter points, connecting lines, and error bars
    plt.errorbar(proto_x, proto_y, yerr=proto_err, fmt='o-', label='Protobuf', alpha=0.7, capsize=3)
    plt.errorbar(ser1de_x, ser1de_y, yerr=ser1de_err, fmt='o-', label='Ser1de', alpha=0.7, capsize=3)
    
    plt.xlabel('Flat Object Size (bytes)', fontsize=16)
    plt.ylabel('Latency (ns)', fontsize=16)
    plt.title('Deserialization Performance', fontsize=20)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tick_params(axis='both', which='major', labelsize=16)
    
    # Create directory if it doesn't exist
    os.makedirs('latency_plots', exist_ok=True)
    plt.savefig('latency_plots/deserialization_performance.pdf', format='pdf', dpi=300, bbox_inches='tight')
    plt.close()

def create_end2end_plot(results):
    plt.figure(figsize=(10, 6))
    
    # Sort data points by flat_size for proper line connections
    flat_sizes = [r['flat_size'] for r in results]
    proto_end2end = [r['proto_end2end'] for r in results]
    ser1de_end2end = [r['ser1de_end2end'] for r in results]
    
    # Get standard deviations if available
    proto_std = [r.get('std_proto_end2end', 0) for r in results]
    ser1de_std = [r.get('std_ser1de_end2end', 0) for r in results]
    
    # Create sorted pairs for plotting
    proto_data = sorted(zip(flat_sizes, proto_end2end, proto_std))
    ser1de_data = sorted(zip(flat_sizes, ser1de_end2end, ser1de_std))
    
    # Unzip the sorted data
    proto_x, proto_y, proto_err = zip(*proto_data)
    ser1de_x, ser1de_y, ser1de_err = zip(*ser1de_data)
    
    # Plot with both scatter points, connecting lines, and error bars
    plt.errorbar(proto_x, proto_y, yerr=proto_err, fmt='o-', label='Protobuf', alpha=0.7, capsize=3)
    plt.errorbar(ser1de_x, ser1de_y, yerr=ser1de_err, fmt='o-', label='Ser1de', alpha=0.7, capsize=3)
    
    plt.xlabel('Flat Object Size (bytes)', fontsize=16)
    plt.ylabel('Latency (ns)', fontsize=16)
    plt.title('End-to-End Performance', fontsize=20)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tick_params(axis='both', which='major', labelsize=16)
    
    # Create directory if it doesn't exist
    os.makedirs('latency_plots', exist_ok=True)
    plt.savefig('latency_plots/end2end_performance.pdf', format='pdf', dpi=300, bbox_inches='tight')
    plt.close()

def create_compression_plot(results):
    plt.figure(figsize=(10, 6))
    
    # Sort data points by flat_size for proper line connections
    flat_sizes = [r['flat_size'] for r in results]
    proto_bytes = [r['proto_bytes'] for r in results]
    ser1de_bytes = [r['ser1de_bytes'] for r in results]
    
    # Create sorted pairs for plotting
    proto_data = sorted(zip(flat_sizes, proto_bytes))
    ser1de_data = sorted(zip(flat_sizes, ser1de_bytes))
    
    # Unzip the sorted data
    proto_x, proto_y = zip(*proto_data)
    ser1de_x, ser1de_y = zip(*ser1de_data)
    
    # Plot with both scatter points and connecting lines
    plt.plot(proto_x, proto_y, 'o-', label='Protobuf', alpha=0.7)
    plt.plot(ser1de_x, ser1de_y, 'o-', label='Ser1de', alpha=0.7)
    
    plt.xlabel('Flat Object Size (bytes)', fontsize=16)
    plt.ylabel('Output Size (bytes)', fontsize=16)
    plt.title('Compression Comparison', fontsize=20)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tick_params(axis='both', which='major', labelsize=16)
    
    # Create directory if it doesn't exist
    os.makedirs('latency_plots', exist_ok=True)
    plt.savefig('latency_plots/compression_comparison.pdf', format='pdf', dpi=300, bbox_inches='tight')
    plt.close()

def create_compression_ratio_plot(results):
    plt.figure(figsize=(10, 6))
    
    # Calculate compression ratios (serialized size / flat size)
    flat_sizes = [r['flat_size'] for r in results]
    proto_ratios = [r['proto_bytes'] / r['flat_size'] * 100 for r in results]
    ser1de_ratios = [r['ser1de_bytes'] / r['flat_size'] * 100 for r in results]
    
    # Create sorted pairs for plotting
    proto_data = sorted(zip(flat_sizes, proto_ratios))
    ser1de_data = sorted(zip(flat_sizes, ser1de_ratios))
    
    # Unzip the sorted data
    proto_x, proto_y = zip(*proto_data)
    ser1de_x, ser1de_y = zip(*ser1de_data)
    
    # Plot with both scatter points and connecting lines
    plt.plot(proto_x, proto_y, 'o-', label='Protobuf', alpha=0.7)
    plt.plot(ser1de_x, ser1de_y, 'o-', label='Ser1de', alpha=0.7)
    
    plt.xlabel('Flat Object Size (bytes)', fontsize=16)
    plt.ylabel('Compression Ratio (% of original size)', fontsize=16)
    plt.title('Compression Ratio Comparison', fontsize=20)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tick_params(axis='both', which='major', labelsize=16)
    
    # Create directory if it doesn't exist
    os.makedirs('latency_plots', exist_ok=True)
    plt.savefig('latency_plots/compression_ratio.pdf', format='pdf', dpi=300, bbox_inches='tight')
    plt.close()

def create_speedup_latency_plot(results):
    """Create a plot showing actual latencies by message type instead of speedup percentages."""
    plt.figure(figsize=(12, 8))
    
    # Group by message type for better visualization
    messages = sorted(set(r['message'] for r in results))
    
    # Create x positions for grouped bar chart
    x = np.arange(len(messages))
    width = 0.2  # Narrower bars to fit more data
    
    # Calculate average latencies for each message type
    proto_ser = []
    proto_deser = []
    proto_end2end = []
    ser1de_ser = []
    ser1de_deser = []
    ser1de_end2end = []
    
    # Standard deviations if available
    proto_ser_std = []
    proto_deser_std = []
    proto_end2end_std = []
    ser1de_ser_std = []
    ser1de_deser_std = []
    ser1de_end2end_std = []
    
    for msg in messages:
        msg_results = [r for r in results if r['message'] == msg]
        
        # Protobuf latencies
        proto_ser.append(np.mean([r['proto_ser'] for r in msg_results]))
        proto_deser.append(np.mean([r['proto_deser'] for r in msg_results]))
        proto_end2end.append(np.mean([r['proto_end2end'] for r in msg_results]))
        
        # Ser1de latencies
        ser1de_ser.append(np.mean([r['ser1de_ser'] for r in msg_results]))
        ser1de_deser.append(np.mean([r['ser1de_deser'] for r in msg_results]))
        ser1de_end2end.append(np.mean([r['ser1de_end2end'] for r in msg_results]))
        
        # Get standard deviations if available
        if 'std_proto_ser' in msg_results[0]:
            proto_ser_std.append(np.mean([r['std_proto_ser'] for r in msg_results]))
            proto_deser_std.append(np.mean([r['std_proto_deser'] for r in msg_results]))
            proto_end2end_std.append(np.mean([r['std_proto_end2end'] for r in msg_results]))
            ser1de_ser_std.append(np.mean([r['std_ser1de_ser'] for r in msg_results]))
            ser1de_deser_std.append(np.mean([r['std_ser1de_deser'] for r in msg_results]))
            ser1de_end2end_std.append(np.mean([r['std_ser1de_end2end'] for r in msg_results]))
    
    # Plot grouped bar chart with error bars if available
    if 'std_proto_ser' in results[0]:
        plt.bar(x - 3*width/2, proto_ser, width, yerr=proto_ser_std, label='Protobuf Ser', capsize=3, color='royalblue')
        plt.bar(x - width/2, proto_deser, width, yerr=proto_deser_std, label='Protobuf Deser', capsize=3, color='lightblue')
        plt.bar(x + width/2, ser1de_ser, width, yerr=ser1de_ser_std, label='Ser1de Ser', capsize=3, color='darkred')
        plt.bar(x + 3*width/2, ser1de_deser, width, yerr=ser1de_deser_std, label='Ser1de Deser', capsize=3, color='salmon')
    else:
        plt.bar(x - 3*width/2, proto_ser, width, label='Protobuf Ser', color='royalblue')
        plt.bar(x - width/2, proto_deser, width, label='Protobuf Deser', color='lightblue')
        plt.bar(x + width/2, ser1de_ser, width, label='Ser1de Ser', color='darkred')
        plt.bar(x + 3*width/2, ser1de_deser, width, label='Ser1de Deser', color='salmon')
    
    plt.xlabel('Message Type', fontsize=16)
    plt.ylabel('Latency (ns)', fontsize=16)
    plt.title('Serialization and Deserialization Latencies by Message Type', fontsize=20)
    plt.xticks(x, messages)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tick_params(axis='both', which='major', labelsize=16)
    
    # Create directory if it doesn't exist
    os.makedirs('latency_plots', exist_ok=True)
    
    # Save first plot
    plt.savefig('latency_plots/ser_deser_latency_comparison.pdf', format='pdf', dpi=300, bbox_inches='tight')
    
    # Add a second plot for end-to-end latencies
    plt.figure(figsize=(10, 6))
    if 'std_proto_end2end' in results[0]:
        plt.bar(x - width/2, proto_end2end, width, yerr=proto_end2end_std, label='Protobuf End2End', capsize=3, color='blue')
        plt.bar(x + width/2, ser1de_end2end, width, yerr=ser1de_end2end_std, label='Ser1de End2End', capsize=3, color='red')
    else:
        plt.bar(x - width/2, proto_end2end, width, label='Protobuf End2End', color='blue')
        plt.bar(x + width/2, ser1de_end2end, width, label='Ser1de End2End', color='red')
    
    plt.xlabel('Message Type', fontsize=16)
    plt.ylabel('Latency (ns)', fontsize=16)
    plt.title('End-to-End Latencies by Message Type', fontsize=20)
    plt.xticks(x, messages)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tick_params(axis='both', which='major', labelsize=16)
    
    plt.savefig('latency_plots/end2end_latency_comparison.pdf', format='pdf', dpi=300, bbox_inches='tight')
    plt.close()

def create_compression_percentage_plot(results):
    """Create a plot showing compression ratios by message type."""
    plt.figure(figsize=(10, 6))
    
    # Group by message type for better visualization
    messages = sorted(set(r['message'] for r in results))
    
    # Create x positions for grouped bar chart
    x = np.arange(len(messages))
    width = 0.35
    
    # Calculate average compression ratios for each message type
    proto_compression = []
    ser1de_compression = []
    
    for msg in messages:
        msg_results = [r for r in results if r['message'] == msg]
        
        # Calculate compression ratio: (original_size - compressed_size) / original_size * 100
        # This gives the percentage of space saved
        proto_comp = np.mean([(r['flat_size'] - r['proto_bytes']) / r['flat_size'] * 100 for r in msg_results])
        ser1de_comp = np.mean([(r['flat_size'] - r['ser1de_bytes']) / r['flat_size'] * 100 for r in msg_results])
        
        proto_compression.append(proto_comp)
        ser1de_compression.append(ser1de_comp)
    
    # Plot grouped bar chart
    plt.bar(x - width/2, proto_compression, width, label='Protobuf', color='blue')
    plt.bar(x + width/2, ser1de_compression, width, label='Ser1de', color='red')
    
    plt.xlabel('Message Type', fontsize=16)
    plt.ylabel('Compression Ratio (%)', fontsize=16)
    plt.title('Compression Ratio by Message Type (higher is better)', fontsize=20)
    plt.xticks(x, messages)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tick_params(axis='both', which='major', labelsize=16)
    
    # Add a horizontal line at 0% for reference
    plt.axhline(y=0, color='black', linestyle='--', alpha=0.5)
    
    # Annotate negative values (expansion rather than compression)
    for i, v in enumerate(proto_compression):
        if v < 0:
            plt.text(i - width/2, v - 5, f"{v:.1f}%", ha='center', color='white', fontweight='bold')
    
    for i, v in enumerate(ser1de_compression):
        if v < 0:
            plt.text(i + width/2, v - 5, f"{v:.1f}%", ha='center', color='white', fontweight='bold')
    
    # Create directory if it doesn't exist
    os.makedirs('latency_plots', exist_ok=True)
    plt.savefig('latency_plots/compression_ratio_percentage.pdf', format='pdf', dpi=300, bbox_inches='tight')
    plt.close()

def save_detailed_results(results, filename='detailed_results.json'):
    """Save detailed results including standard deviations to a JSON file."""
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2)
    print(f"Detailed results saved to {filename}")

def main():
    # Set up command line argument parsing
    parser = argparse.ArgumentParser(description='Process benchmark results and generate plots.')
    parser.add_argument('--input', '-i', type=str, help='Path to input file with benchmark results')
    parser.add_argument('--output', '-o', type=str, default='benchmark_results.txt', 
                        help='Path to save benchmark results (default: benchmark_results.txt)')
    parser.add_argument('--max-size', '-m', type=int, default=None,
                        help='Maximum flat object size in bytes to include in analysis (default: no limit)')
    parser.add_argument('--min-size', '-n', type=int, default=None,
                        help='Minimum flat object size in bytes to include in analysis (default: no limit)')
    parser.add_argument('--runs', '-r', type=int, default=5,
                        help='Number of benchmark runs to perform (default: 5)')
    args = parser.parse_args()
    
    # Create output directory
    os.makedirs('latency_plots', exist_ok=True)
    
    # Get benchmark data either from file or by running the benchmark
    if args.input:
        print(f"Reading benchmark results from {args.input}")
        benchmark_outputs = read_benchmark_from_file(args.input)
        if benchmark_outputs is None:
            return
    else:
        print(f"Running benchmark {args.runs} times...")
        benchmark_outputs = run_benchmark(args.runs)
        if benchmark_outputs is None:
            return
        
        # Save all benchmark outputs to a text file
        with open(args.output, 'w') as f:
            for i, output in enumerate(benchmark_outputs):
                f.write(f"=== RUN {i+1} ===\n")
                f.write(output)
                f.write("\n\n")
        print(f"Benchmark results saved to {args.output}")
    
    # Parse all benchmark runs
    all_results = []
    for output in benchmark_outputs:
        results = parse_benchmark_results(output)
        if results:
            all_results.append(results)
        else:
            print("Warning: No valid benchmark results found in one of the runs.")
    
    if not all_results:
        print("Error: No valid benchmark results found in any run.")
        return
    
    # Aggregate results from multiple runs
    aggregated_results = aggregate_multiple_runs(all_results)
    original_count = len(aggregated_results)
    
    # Filter results by size if min-size or max-size is specified
    filtered_results = aggregated_results.copy()
    
    # Apply minimum size filter
    if args.min_size is not None:
        filtered_results = [r for r in filtered_results if r['flat_size'] >= args.min_size]
        min_filtered_count = original_count - len(filtered_results)
        if min_filtered_count > 0:
            print(f"Filtered out {min_filtered_count} results with flat size < {args.min_size} bytes.")
    
    # Apply maximum size filter
    if args.max_size is not None:
        filtered_results = [r for r in filtered_results if r['flat_size'] <= args.max_size]
        max_filtered_count = len(aggregated_results) - len(filtered_results)
        if args.min_size is not None:
            max_filtered_count = len([r for r in aggregated_results if r['flat_size'] >= args.min_size and r['flat_size'] > args.max_size])
        
        if max_filtered_count > 0:
            print(f"Filtered out {max_filtered_count} results with flat size > {args.max_size} bytes.")
    
    # Update aggregated_results with filtered results
    aggregated_results = filtered_results
    
    if not aggregated_results:
        print("Error: No results left after size filtering. Try adjusting the size limits.")
        return
    
    print(f"Successfully processed {len(aggregated_results)} benchmark results across {len(all_results)} runs.")
    
    # Save detailed results including standard deviations
    save_detailed_results(aggregated_results, 'latency_plots/detailed_results.json')
    
    # Sort results by flat size for better visualization
    aggregated_results.sort(key=lambda x: x['flat_size'])
    
    # Create individual plots
    create_serialization_plot(aggregated_results)
    create_deserialization_plot(aggregated_results)
    create_end2end_plot(aggregated_results)
    create_compression_plot(aggregated_results)
    create_compression_ratio_plot(aggregated_results)
    create_speedup_latency_plot(aggregated_results)
    create_compression_percentage_plot(aggregated_results)
    
    print("Plots have been saved in the 'latency_plots' directory as:")
    print("- serialization_performance.pdf")
    print("- deserialization_performance.pdf")
    print("- end2end_performance.pdf")
    print("- compression_comparison.pdf")
    print("- compression_ratio.pdf")
    print("- ser_deser_latency_comparison.pdf")
    print("- end2end_latency_comparison.pdf")
    print("- compression_ratio_percentage.pdf")

if __name__ == "__main__":
    main()