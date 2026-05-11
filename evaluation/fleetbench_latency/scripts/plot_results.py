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
                label='Protobuf', color='green', marker='o', capsize=5)
    plt.errorbar(ser1de_sizes, ser1de_times_avg, yerr=ser1de_times_std, 
                label='SERenaDE', color='red', marker='s', capsize=5)

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
                label='Protobuf', color='green', marker='o', capsize=5)
    plt.errorbar(ser1de_sizes, ser1de_packets_avg, yerr=ser1de_packets_std, 
                label='SERenaDE', color='red', marker='s', capsize=5)

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

    # Call the new compression comparison function
    plot_compression_comparison(protobuf_data, ser1de_data, title_suffix)
    
    # Call the new latency comparison function
    plot_latency_comparison(protobuf_data, ser1de_data, title_suffix)

def plot_compression_comparison(protobuf_data, ser1de_data, title_suffix=""):
    """Create a bar plot comparing compression ratios with 3 bars per message type"""
    
    # Get common sizes between protobuf and ser1de data
    common_sizes = sorted(set(protobuf_data.keys()) & set(ser1de_data.keys()))
    
    if not common_sizes:
        print("Warning: No common message sizes found between Protobuf and SERenaDE data")
        return
    
    # Prepare data for plotting
    original_sizes = []
    protobuf_compressed = []
    ser1de_compressed = []
    protobuf_ratios = []
    ser1de_ratios = []
    message_labels = []
    
    for size in common_sizes:
        # Calculate average packet sizes for this flat object size
        protobuf_avg = np.mean(protobuf_data[size]['packets'])
        ser1de_avg = np.mean(ser1de_data[size]['packets'])
        
        original_sizes.append(size)
        protobuf_compressed.append(protobuf_avg)
        ser1de_compressed.append(ser1de_avg)
        
        # Calculate compression ratios
        protobuf_ratios.append(size / protobuf_avg if protobuf_avg > 0 else 0)
        ser1de_ratios.append(size / ser1de_avg if ser1de_avg > 0 else 0)
        
        # Create readable message labels based on original size
        if size >= 1000000:
            message_labels.append(f"{size//1000000}MB")
        elif size >= 1000:
            message_labels.append(f"{size//1000}KB")
        else:
            message_labels.append(f"{size}B")
    
    # Set up the bar plot
    x = np.arange(len(common_sizes))  # Label locations
    width = 0.25  # Width of bars
    
    # PLOT 1: Bars only (without compression ratio lines)
    plt.figure(figsize=(14, 8))
    
    # Create bars with some transparency
    bars1 = plt.bar(x - width, original_sizes, width, label='Original', color='gray', alpha=0.8)
    bars2 = plt.bar(x, protobuf_compressed, width, label='Protobuf', color='green', alpha=0.8)
    bars3 = plt.bar(x + width, ser1de_compressed, width, label='SERenaDE', color='red', alpha=0.8)
    
    # Customize the plot
    plt.xlabel('Message Type (by Original Size)', fontsize=22)
    plt.ylabel('Size (bytes)', fontsize=22)
    plt.title(f'Compression Comparison', fontsize=26)
    plt.xticks(x, message_labels, fontsize=20, rotation=45)
    plt.yticks(fontsize=22)
    plt.legend(fontsize=22)
    plt.grid(True, alpha=0.3, axis='y')
    
    # Add value labels on top of bars for better readability
    def add_value_labels(bars, values):
        for bar, value in zip(bars, values):
            height = bar.get_height()
            plt.text(bar.get_x() + bar.get_width()/2., height + max(values)*0.01,
                    f'{int(value)}',
                    ha='center', va='bottom', fontsize=10, rotation=0)
    
    # Add labels only for smaller datasets to avoid clutter
    if len(common_sizes) <= 8:
        add_value_labels(bars1, original_sizes)
        add_value_labels(bars2, protobuf_compressed)
        add_value_labels(bars3, ser1de_compressed)
    
    plt.tight_layout()
    plt.savefig('compression_comparison.pdf', bbox_inches='tight', pad_inches=0.2)
    plt.close()
    print("Saved compression comparison bar plot to compression_comparison.pdf")
    
    # PLOT 2: Bars with compression ratio lines
    fig, ax1 = plt.subplots(figsize=(14, 8))
    
    # Create bars with some transparency
    bars1 = ax1.bar(x - width, original_sizes, width, label='Original', color='gray', alpha=0.8)
    bars2 = ax1.bar(x, protobuf_compressed, width, label='Protobuf', color='green', alpha=0.8)
    bars3 = ax1.bar(x + width, ser1de_compressed, width, label='SERenaDE', color='red', alpha=0.8)
    
    # Customize the primary y-axis (bars)
    ax1.set_xlabel('Message Type (by Original Size)', fontsize=22)
    ax1.set_ylabel('Size (bytes)', fontsize=22)
    ax1.set_title(f'Compression Ratio Comparison', fontsize=26)
    ax1.set_xticks(x)
    ax1.set_xticklabels(message_labels, fontsize=20, rotation=45)
    ax1.tick_params(axis='y', labelsize=22)
    ax1.grid(True, alpha=0.3, axis='y')
    
    # Create secondary y-axis for compression ratios
    ax2 = ax1.twinx()
    
    # Plot compression ratio lines with markers
    line1 = ax2.plot(x, protobuf_ratios, '--o', color='green', linewidth=2, markersize=8, 
                     label='Compression ratio, Protobuf')
    line2 = ax2.plot(x, ser1de_ratios, '--s', color='red', linewidth=2, markersize=8, 
                     label='Compression ratio, SERenaDE')
    
    # Customize the secondary y-axis
    ax2.set_ylabel('Compression Ratio', fontsize=22)
    ax2.tick_params(axis='y', labelsize=22)
    
    # Add more space at the top for compression ratio labels
    max_ratio = max(protobuf_ratios + ser1de_ratios)
    ax2.set_ylim(0, max_ratio * 1.15)  # Add 15% more space at the top
    
    # Add compression ratio value labels
    for i, (proto_ratio, ser1de_ratio) in enumerate(zip(protobuf_ratios, ser1de_ratios)):
        # Place labels near the markers with better spacing
        ax2.text(i, proto_ratio + max_ratio * 0.03, 
                f'{proto_ratio:.1f}x', ha='center', va='bottom', fontsize=12, color='green')
        ax2.text(i, ser1de_ratio + max_ratio * 0.03, 
                f'{ser1de_ratio:.1f}x', ha='center', va='bottom', fontsize=12, color='red')
    
    # Position legends
    bars_legend = ax1.legend(loc='upper left', fontsize=18)
    lines_legend = ax2.legend(loc='upper center', fontsize=18)
    
    # Add the bars legend back since lines_legend overwrites it
    ax1.add_artist(bars_legend)
    
    plt.tight_layout()
    plt.savefig('compression_comparison_with_ratios.pdf', bbox_inches='tight', pad_inches=0.2)
    plt.close()
    print("Saved compression comparison with ratio lines to compression_comparison_with_ratios.pdf")
    
    # Print compression ratio statistics
    print("\nCompression Ratio Statistics:")
    print("=" * 60)
    for i, size in enumerate(common_sizes):
        proto_ratio = (original_sizes[i] / protobuf_compressed[i]) if protobuf_compressed[i] > 0 else 0
        ser1de_ratio = (original_sizes[i] / ser1de_compressed[i]) if ser1de_compressed[i] > 0 else 0
        print(f"Message {message_labels[i]:>6}: "
              f"Protobuf {proto_ratio:.2f}x, "
              f"SERenaDE {ser1de_ratio:.2f}x compression")
    
    # Filter and show compression ratios for messages larger than 4KB
    large_messages = [(i, size) for i, size in enumerate(common_sizes) if size > 4096]
    
    if large_messages:
        print("\nCompression Ratios for Messages > 4KB:")
        print("=" * 70)
        
        total_proto_ratio = 0
        total_ser1de_ratio = 0
        
        for i, size in large_messages:
            proto_ratio = (original_sizes[i] / protobuf_compressed[i]) if protobuf_compressed[i] > 0 else 0
            ser1de_ratio = (original_sizes[i] / ser1de_compressed[i]) if ser1de_compressed[i] > 0 else 0
            
            total_proto_ratio += proto_ratio
            total_ser1de_ratio += ser1de_ratio
            
            # Calculate compression percentage
            proto_reduction = ((original_sizes[i] - protobuf_compressed[i]) / original_sizes[i]) * 100
            ser1de_reduction = ((original_sizes[i] - ser1de_compressed[i]) / original_sizes[i]) * 100
            
            print(f"Message {message_labels[i]:>6} ({size:>7} bytes): "
                  f"Protobuf {proto_ratio:.2f}x ({proto_reduction:.1f}% reduction), "
                  f"SERenaDE {ser1de_ratio:.2f}x ({ser1de_reduction:.1f}% reduction)")
        
        # Calculate averages
        avg_proto_ratio = total_proto_ratio / len(large_messages)
        avg_ser1de_ratio = total_ser1de_ratio / len(large_messages)
        
        print("-" * 70)
        print(f"Average compression for {len(large_messages)} messages > 4KB:")
        print(f"  Protobuf: {avg_proto_ratio:.2f}x compression")
        print(f"  SERenaDE: {avg_ser1de_ratio:.2f}x compression")
        
        if avg_ser1de_ratio > avg_proto_ratio:
            improvement = ((avg_ser1de_ratio - avg_proto_ratio) / avg_proto_ratio) * 100
            print(f"  SERenaDE achieves {improvement:.1f}% better compression than Protobuf")
        elif avg_proto_ratio > avg_ser1de_ratio:
            improvement = ((avg_proto_ratio - avg_ser1de_ratio) / avg_ser1de_ratio) * 100
            print(f"  Protobuf achieves {improvement:.1f}% better compression than SERenaDE")
        else:
            print("  Both achieve similar compression ratios")
    else:
        print("\nNo messages larger than 4KB found in the dataset.")

def plot_latency_comparison(protobuf_data, ser1de_data, title_suffix=""):
    """Create a bar plot comparing end-to-end latencies with speedup line"""
    
    # Get common sizes between protobuf and ser1de data
    common_sizes = sorted(set(protobuf_data.keys()) & set(ser1de_data.keys()))
    
    if not common_sizes:
        print("Warning: No common message sizes found between Protobuf and SERenaDE data")
        return
    
    # Prepare data for plotting
    protobuf_times_avg = []
    ser1de_times_avg = []
    protobuf_times_std = []
    ser1de_times_std = []
    speedup_ratios = []
    message_labels = []
    
    for size in common_sizes:
        # Calculate average times for this flat object size (convert ns to μs)
        protobuf_times = [t / 1000.0 for t in protobuf_data[size]['times']]
        ser1de_times = [t / 1000.0 for t in ser1de_data[size]['times']]
        
        protobuf_avg = np.mean(protobuf_times)
        ser1de_avg = np.mean(ser1de_times)
        
        protobuf_times_avg.append(protobuf_avg)
        ser1de_times_avg.append(ser1de_avg)
        protobuf_times_std.append(np.std(protobuf_times))
        ser1de_times_std.append(np.std(ser1de_times))
        
        # Calculate speedup (protobuf_time / ser1de_time)
        speedup = protobuf_avg / ser1de_avg if ser1de_avg > 0 else 0
        speedup_ratios.append(speedup)
        
        # Create readable message labels based on original size
        if size >= 1000000:
            message_labels.append(f"{size//1000000}MB")
        elif size >= 1000:
            message_labels.append(f"{size//1000}KB")
        else:
            message_labels.append(f"{size}B")
    
    # Set up the bar plot
    x = np.arange(len(common_sizes))  # Label locations
    width = 0.35  # Width of bars (wider since we only have 2 bars)
    
    fig, ax1 = plt.subplots(figsize=(14, 8))
    
    # Create bars with error bars
    bars1 = ax1.bar(x - width/2, protobuf_times_avg, width, 
                    yerr=protobuf_times_std, label='Protobuf', 
                    color='green', alpha=0.8, capsize=5)
    bars2 = ax1.bar(x + width/2, ser1de_times_avg, width, 
                    yerr=ser1de_times_std, label='SERenaDE', 
                    color='red', alpha=0.8, capsize=5)
    
    # Customize the primary y-axis (bars)
    ax1.set_xlabel('Message Type (by Original Size)', fontsize=22)
    ax1.set_ylabel('End-to-End Latency (μs)', fontsize=22)
    ax1.set_title(f'Latency Performance Comparison', fontsize=26)
    ax1.set_xticks(x)
    ax1.set_xticklabels(message_labels, fontsize=20, rotation=45)
    ax1.tick_params(axis='y', labelsize=22)
    ax1.grid(True, alpha=0.3, axis='y')
    
    # Create secondary y-axis for speedup ratios
    ax2 = ax1.twinx()
    
    # Plot speedup line with markers
    line1 = ax2.plot(x, speedup_ratios, '--o', color='black', linewidth=3, markersize=10, 
                     label='Speedup')
    
    # Customize the secondary y-axis
    ax2.set_ylabel('Speedup Factor', fontsize=22)
    ax2.tick_params(axis='y', labelsize=22)
    
    # Add more space at the top for speedup labels
    max_speedup = max(speedup_ratios)
    ax2.set_ylim(0, max_speedup * 1.15)  # Add 15% more space at the top
    
    # Add speedup value labels
    for i, speedup in enumerate(speedup_ratios):
        ax2.text(i, speedup + max_speedup * 0.03, 
                f'{speedup:.1f}x', ha='center', va='bottom', fontsize=12, 
                color='black', weight='bold')
    
    # Position legends
    bars_legend = ax1.legend(loc='upper left', fontsize=18)
    lines_legend = ax2.legend(loc='upper center', fontsize=18)
    
    # Add the bars legend back since lines_legend overwrites it
    ax1.add_artist(bars_legend)
    
    plt.tight_layout()
    plt.savefig('latency_comparison.pdf', bbox_inches='tight', pad_inches=0.2)
    plt.close()
    print("Saved latency comparison plot to latency_comparison.pdf")
    
    # Print latency statistics
    print("\nLatency Performance Statistics:")
    print("=" * 70)
    for i, size in enumerate(common_sizes):
        print(f"Message {message_labels[i]:>6}: "
              f"Protobuf {protobuf_times_avg[i]:.2f}μs ±{protobuf_times_std[i]:.2f}, "
              f"SERenaDE {ser1de_times_avg[i]:.2f}μs ±{ser1de_times_std[i]:.2f}, "
              f"Speedup: {speedup_ratios[i]:.2f}x")
    
    # Filter and show speedup for messages larger than 4KB
    large_messages = [(i, size) for i, size in enumerate(common_sizes) if size > 4096]
    
    if large_messages:
        print("\nSpeedup for Messages > 4KB:")
        print("=" * 70)
        
        total_speedup = 0
        
        for i, size in large_messages:
            speedup = speedup_ratios[i]
            total_speedup += speedup
            
            # Calculate latency reduction percentage
            reduction = ((protobuf_times_avg[i] - ser1de_times_avg[i]) / protobuf_times_avg[i]) * 100
            
            print(f"Message {message_labels[i]:>6} ({size:>7} bytes): "
                  f"{speedup:.2f}x speedup ({reduction:.1f}% latency reduction)")
        
        # Calculate averages
        avg_speedup = total_speedup / len(large_messages)
        
        print("-" * 70)
        print(f"Average speedup for {len(large_messages)} messages > 4KB: {avg_speedup:.2f}x")
        
        if avg_speedup > 1.0:
            print(f"SERenaDE is on average {avg_speedup:.2f}x faster than Protobuf for large messages")
        elif avg_speedup < 1.0:
            print(f"Protobuf is on average {1/avg_speedup:.2f}x faster than SERenaDE for large messages")
        else:
            print("Both achieve similar performance for large messages")
    else:
        print("\nNo messages larger than 4KB found in the dataset.")

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