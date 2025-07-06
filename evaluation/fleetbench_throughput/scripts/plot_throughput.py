import matplotlib.pyplot as plt
import re
import os

def parse_benchmark_file(file_path):
    """Parse the benchmark output file to extract data."""
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Extract all benchmark sections
    sections = re.findall(r'Number of requests: (\d+).*?Protobuf Serialize: (\d+) ns.*?Protobuf Deserialize: (\d+) ns.*?Ser1de Serialize: (\d+) ns.*?Ser1de Deserialize: (\d+) ns', 
                         content, re.DOTALL)
    
    # Organize data
    num_requests = []
    protobuf_serialize_times = []
    protobuf_deserialize_times = []
    ser1de_serialize_times = []
    ser1de_deserialize_times = []
    
    for section in sections:
        num_requests.append(int(section[0]))
        protobuf_serialize_times.append(int(section[1]))
        protobuf_deserialize_times.append(int(section[2]))
        ser1de_serialize_times.append(int(section[3]))
        ser1de_deserialize_times.append(int(section[4]))
    
    # Sort data by number of requests
    sorted_data = sorted(zip(num_requests, protobuf_serialize_times, protobuf_deserialize_times, 
                            ser1de_serialize_times, ser1de_deserialize_times))
    
    # Unpack sorted data
    num_requests = [data[0] for data in sorted_data]
    protobuf_serialize_times = [data[1] for data in sorted_data]
    protobuf_deserialize_times = [data[2] for data in sorted_data]
    ser1de_serialize_times = [data[3] for data in sorted_data]
    ser1de_deserialize_times = [data[4] for data in sorted_data]
    
    return (num_requests, protobuf_serialize_times, protobuf_deserialize_times, 
            ser1de_serialize_times, ser1de_deserialize_times)

def create_plots(data, output_dir):
    """Create and save the plots to the specified directory."""
    num_requests, protobuf_serialize_times, protobuf_deserialize_times, ser1de_serialize_times, ser1de_deserialize_times = data
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Convert nanoseconds to milliseconds for better readability
    protobuf_serialize_ms = [t / 1_000_000 for t in protobuf_serialize_times]
    protobuf_deserialize_ms = [t / 1_000_000 for t in protobuf_deserialize_times]
    ser1de_serialize_ms = [t / 1_000_000 for t in ser1de_serialize_times]
    ser1de_deserialize_ms = [t / 1_000_000 for t in ser1de_deserialize_times]
    
    # Calculate requests per second (throughput)
    # For each batch, calculate: (num_requests / time_in_seconds)
    protobuf_serialize_rps = [(n * 1_000_000_000) / t for n, t in zip(num_requests, protobuf_serialize_times)]
    protobuf_deserialize_rps = [(n * 1_000_000_000) / t for n, t in zip(num_requests, protobuf_deserialize_times)]
    ser1de_serialize_rps = [(n * 1_000_000_000) / t for n, t in zip(num_requests, ser1de_serialize_times)]
    ser1de_deserialize_rps = [(n * 1_000_000_000) / t for n, t in zip(num_requests, ser1de_deserialize_times)]
    
    # Set up the figure style
    #plt.style.use('seaborn-v0_8-darkgrid')
    plt.rcParams.update({'font.size': 14})
    
    # Create figure 1: Serialization Latency
    plt.figure(figsize=(12, 8))
    plt.plot(num_requests, protobuf_serialize_ms, 'o-', linewidth=2, markersize=8, label='Protobuf')
    plt.plot(num_requests, ser1de_serialize_ms, 's-', linewidth=2, markersize=8, label='Ser1de')
    plt.xlabel('Number of Requests', fontsize=18)
    plt.ylabel('Latency (ms)', fontsize=18)
    plt.title('Serialization Throughput', fontsize=20)
    plt.legend(fontsize=18)
    plt.grid(True)
    plt.xticks(fontsize=16)
    plt.yticks(fontsize=16)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'serialization_latency_comparison.pdf'))
    
    # Create figure 2: Deserialization Latency
    plt.figure(figsize=(12, 8))
    plt.plot(num_requests, protobuf_deserialize_ms, 'o-', linewidth=2, markersize=8, label='Protobuf')
    plt.plot(num_requests, ser1de_deserialize_ms, 's-', linewidth=2, markersize=8, label='Ser1de')
    plt.xlabel('Number of Requests', fontsize=18)
    plt.ylabel('Latency (ms)', fontsize=18)
    plt.title('Deserialization Throughput', fontsize=20)
    plt.legend(fontsize=18)
    plt.grid(True)
    plt.xticks(fontsize=16)
    plt.yticks(fontsize=16)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'deserialization_latency_comparison.pdf'))
    
    # Create figure 3: Serialization Throughput (requests per second)
    plt.figure(figsize=(12, 8))
    plt.plot(num_requests, protobuf_serialize_rps, 'o-', linewidth=2, markersize=8, label='Protobuf')
    plt.plot(num_requests, ser1de_serialize_rps, 's-', linewidth=2, markersize=8, label='Ser1de')
    plt.xlabel('Number of Requests', fontsize=18)
    plt.ylabel('Requests per Second', fontsize=18)
    plt.title('Serialization Throughput (RPS)', fontsize=20)
    plt.legend(fontsize=18)
    plt.grid(True)
    plt.xticks(fontsize=16)
    plt.yticks(fontsize=16)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'serialization_rps_comparison.pdf'))
    
    # Create figure 4: Deserialization Throughput (requests per second)
    plt.figure(figsize=(12, 8))
    plt.plot(num_requests, protobuf_deserialize_rps, 'o-', linewidth=2, markersize=8, label='Protobuf')
    plt.plot(num_requests, ser1de_deserialize_rps, 's-', linewidth=2, markersize=8, label='Ser1de')
    plt.xlabel('Number of Requests', fontsize=18)
    plt.ylabel('Requests per Second', fontsize=18)
    plt.title('Deserialization Throughput (RPS)', fontsize=20)
    plt.legend(fontsize=18)
    plt.grid(True)
    plt.xticks(fontsize=16)
    plt.yticks(fontsize=16)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'deserialization_rps_comparison.pdf'))
    
    # New plots with RPS on x-axis and latency on y-axis
    
    # Create figure 5: Serialization Latency vs RPS
    plt.figure(figsize=(12, 8))
    # Sort the data by RPS for better visualization
    protobuf_data = sorted(zip(protobuf_serialize_rps, protobuf_serialize_ms))
    ser1de_data = sorted(zip(ser1de_serialize_rps, ser1de_serialize_ms))
    
    plt.plot([x for x, _ in protobuf_data], [y for _, y in protobuf_data], 'o-', linewidth=2, markersize=8, label='Protobuf')
    plt.plot([x for x, _ in ser1de_data], [y for _, y in ser1de_data], 's-', linewidth=2, markersize=8, label='Ser1de')
    plt.xlabel('Requests per Second (RPS)', fontsize=18)
    plt.ylabel('Latency (ms)', fontsize=18)
    plt.title('Serialization: Latency vs Throughput', fontsize=20)
    plt.legend(fontsize=18)
    plt.grid(True)
    plt.xticks(fontsize=16)
    plt.yticks(fontsize=16)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'serialization_latency_vs_rps.pdf'))
    
    # Create figure 6: Deserialization Latency vs RPS
    plt.figure(figsize=(12, 8))
    # Sort the data by RPS for better visualization
    protobuf_data = sorted(zip(protobuf_deserialize_rps, protobuf_deserialize_ms))
    ser1de_data = sorted(zip(ser1de_deserialize_rps, ser1de_deserialize_ms))
    
    plt.plot([x for x, _ in protobuf_data], [y for _, y in protobuf_data], 'o-', linewidth=2, markersize=8, label='Protobuf')
    plt.plot([x for x, _ in ser1de_data], [y for _, y in ser1de_data], 's-', linewidth=2, markersize=8, label='Ser1de')
    plt.xlabel('Requests per Second (RPS)', fontsize=18)
    plt.ylabel('Latency (ms)', fontsize=18)
    plt.title('Deserialization: Latency vs Throughput', fontsize=20)
    plt.legend(fontsize=18)
    plt.grid(True)
    plt.xticks(fontsize=16)
    plt.yticks(fontsize=16)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'deserialization_latency_vs_rps.pdf'))
    
    print(f"Plots have been saved as PDF files in the '{output_dir}' directory.")

def main():
    # File path to the benchmark output
    file_path = 'benchmark_output.txt'
    
    # Output directory for plots
    output_dir = 'throughput_plots'
    
    try:
        # Parse the benchmark data
        data = parse_benchmark_file(file_path)
        
        # Create and save the plots
        create_plots(data, output_dir)
    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()