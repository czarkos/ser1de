import os
import re
import matplotlib.pyplot as plt
import numpy as np
import argparse
import glob

def parse_summary_file(file_path):
    """Parse a summary file to extract benchmark data."""
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Extract data using regex
    num_cores_match = re.search(r'Number of cores: (\d+)', content)
    protobuf_serialize_match = re.search(r'Protobuf Serialize Throughput: ([\d.]+) req/s', content)
    protobuf_deserialize_match = re.search(r'Protobuf Deserialize Throughput: ([\d.]+) req/s', content)
    ser1de_serialize_match = re.search(r'Ser1de Serialize Throughput: ([\d.]+) req/s', content)
    ser1de_deserialize_match = re.search(r'Ser1de Deserialize Throughput: ([\d.]+) req/s', content)
    
    if not all([num_cores_match, protobuf_serialize_match, protobuf_deserialize_match, 
                ser1de_serialize_match, ser1de_deserialize_match]):
        return None
    
    return {
        'num_cores': int(num_cores_match.group(1)),
        'protobuf_serialize_throughput': float(protobuf_serialize_match.group(1)),
        'protobuf_deserialize_throughput': float(protobuf_deserialize_match.group(1)),
        'ser1de_serialize_throughput': float(ser1de_serialize_match.group(1)),
        'ser1de_deserialize_throughput': float(ser1de_deserialize_match.group(1))
    }

def load_results_from_directory(results_dir):
    """Load all benchmark results from summary files in the directory."""
    summary_files = glob.glob(os.path.join(results_dir, 'summary_*_cores.txt'))
    
    if not summary_files:
        raise ValueError(f"No summary files found in {results_dir}")
    
    results = []
    for file_path in summary_files:
        result = parse_summary_file(file_path)
        if result:
            results.append(result)
    
    # Sort by number of cores
    results.sort(key=lambda x: x['num_cores'])
    
    return results

def create_plots(results, output_dir):
    """Create and save plots showing throughput scaling with core count."""
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Extract data for plotting
    core_counts = [r['num_cores'] for r in results]
    protobuf_serialize_throughputs = [r['protobuf_serialize_throughput'] for r in results]
    protobuf_deserialize_throughputs = [r['protobuf_deserialize_throughput'] for r in results]
    ser1de_serialize_throughputs = [r['ser1de_serialize_throughput'] for r in results]
    ser1de_deserialize_throughputs = [r['ser1de_deserialize_throughput'] for r in results]
    
    # Set up the figure style
    plt.rcParams.update({'font.size': 14})
    
    # Create figure 1: Serialization Throughput
    plt.figure(figsize=(12, 8))
    plt.plot(core_counts, protobuf_serialize_throughputs, 'o-', linewidth=2, markersize=8, label='Protobuf', color='green')
    plt.plot(core_counts, ser1de_serialize_throughputs, 's-', linewidth=2, markersize=8, label='SERenaDE', color='red')
    plt.xlabel('Number of Processes', fontsize=28)
    plt.ylabel('Throughput (requests/second)', fontsize=28)
    plt.title('Serialization Throughput Scaling', fontsize=32)
    plt.legend(fontsize=18, loc='upper left')
    plt.grid(True)
    plt.xticks(core_counts, fontsize=28)
    plt.yticks(fontsize=28)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'serialization_throughput_scaling.pdf'))
    plt.close()
    
    # Create figure 2: Deserialization Throughput
    plt.figure(figsize=(12, 8))
    plt.plot(core_counts, protobuf_deserialize_throughputs, 'o-', linewidth=2, markersize=8, label='Protobuf', color='green')
    plt.plot(core_counts, ser1de_deserialize_throughputs, 's-', linewidth=2, markersize=8, label='SERenaDE', color='red')
    plt.xlabel('Number of Processes', fontsize=28)
    plt.ylabel('Throughput (requests/second)', fontsize=28)
    plt.title('Deserialization Throughput Scaling', fontsize=32)
    plt.legend(fontsize=18, loc='upper left')
    plt.grid(True)
    plt.xticks(core_counts, fontsize=28)
    plt.yticks(fontsize=28)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'deserialization_throughput_scaling.pdf'))
    plt.close()
    
    # Create figure 3: Combined bar chart for maximum throughput
    plt.figure(figsize=(14, 10))
    
    max_core_count = max(core_counts)
    max_results_idx = core_counts.index(max_core_count)
    max_results = results[max_results_idx]
    
    categories = ['Serialization', 'Deserialization']
    protobuf_values = [max_results['protobuf_serialize_throughput'], 
                       max_results['protobuf_deserialize_throughput']]
    ser1de_values = [max_results['ser1de_serialize_throughput'], 
                     max_results['ser1de_deserialize_throughput']]
    
    x = np.arange(len(categories))
    width = 0.35
    
    fig, ax = plt.subplots(figsize=(12, 8))
    rects1 = ax.bar(x - width/2, protobuf_values, width, label='Protobuf', color='green')
    rects2 = ax.bar(x + width/2, ser1de_values, width, label='SERenaDE', color='red')
    
    ax.set_ylabel('Throughput (requests/second)', fontsize=18)
    ax.set_title(f'Maximum Throughput with {max_core_count} Cores', fontsize=20)
    ax.set_xticks(x)
    ax.set_xticklabels(categories, fontsize=16)
    ax.legend(fontsize=14, loc='upper left')
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
    plt.close()
    
    # Create combined plot showing both serialization and deserialization
    create_combined_plot(results, output_dir)
    
    print(f"Plots have been saved as PDF files in the '{output_dir}' directory.")

def create_combined_plot(results, output_dir):
    """Create a combined plot showing both serialization and deserialization throughput."""
    # Extract data for plotting
    core_counts = [r['num_cores'] for r in results]
    protobuf_serialize_throughputs = [r['protobuf_serialize_throughput'] for r in results]
    protobuf_deserialize_throughputs = [r['protobuf_deserialize_throughput'] for r in results]
    ser1de_serialize_throughputs = [r['ser1de_serialize_throughput'] for r in results]
    ser1de_deserialize_throughputs = [r['ser1de_deserialize_throughput'] for r in results]
    
    # Create combined figure
    plt.figure(figsize=(14, 10))
    
    # Plot serialization with dotted lines
    plt.plot(core_counts, protobuf_serialize_throughputs, 'o--', linewidth=2, markersize=8, 
             label='Protobuf Serialization', color='green')
    plt.plot(core_counts, ser1de_serialize_throughputs, 's--', linewidth=2, markersize=8, 
             label='SERenaDE Serialization', color='red')
    
    # Plot deserialization with solid lines
    plt.plot(core_counts, protobuf_deserialize_throughputs, 'o-', linewidth=2, markersize=8, 
             label='Protobuf Deserialization', color='green')
    plt.plot(core_counts, ser1de_deserialize_throughputs, 's-', linewidth=2, markersize=8, 
             label='SERenaDE Deserialization', color='red')
    
    plt.xlabel('Number of Processes', fontsize=28)
    plt.ylabel('Throughput (requests/second)', fontsize=28)
    plt.title('Serialization and Deserialization Throughput Scaling', fontsize=32)
    plt.legend(fontsize=22, loc='upper left', markerscale=0.85)
    plt.grid(True)
    plt.xticks(core_counts, fontsize=28)
    plt.yticks(fontsize=28)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combined_throughput_scaling.pdf'))
    plt.close()

def main():
    parser = argparse.ArgumentParser(description='Recreate plots from benchmark results directory')
    parser.add_argument('results_dir', help='Directory containing benchmark summary files')
    parser.add_argument('--output-dir', default='recreated_plots', 
                       help='Output directory for plots (default: recreated_plots)')
    
    args = parser.parse_args()
    
    if not os.path.exists(args.results_dir):
        print(f"Error: Results directory '{args.results_dir}' does not exist.")
        return
    
    try:
        # Load results from summary files
        results = load_results_from_directory(args.results_dir)
        
        print(f"Found {len(results)} result sets:")
        for result in results:
            print(f"  {result['num_cores']} cores")
        
        # Create plots
        create_plots(results, args.output_dir)
        
        print(f"\nSuccessfully recreated plots in '{args.output_dir}' directory.")
        
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()