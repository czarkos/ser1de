import matplotlib.pyplot as plt
import re
import numpy as np

# Initialize lists to store data
protobuf_sizes = []
protobuf_times = []
protobuf_packets = []  # New list for packet sizes
ser1de_sizes = []
ser1de_times = []
ser1de_packets = []    # New list for packet sizes

# Read and parse the results file
with open('results.txt', 'r') as f:
    lines = f.readlines()
    
    current_flat_size = None
    for i, line in enumerate(lines):
        if 'Flat Object Size:' in line:
            # Extract flat object size
            current_flat_size = int(line.split('bytes')[0].split(':')[-1].strip())
        elif line.startswith('Protobuf:'):
            # Extract ser+deser time for Protobuf
            parts = line.strip().split(',')
            time = float(parts[4].split('us')[0].strip())
            packet_size = int(parts[3].split('bytes')[0].strip())
            protobuf_sizes.append(current_flat_size)
            protobuf_times.append(time)
            protobuf_packets.append(packet_size)
        elif line.startswith('Ser1de:'):
            # Extract ser+deser time for Ser1de
            parts = line.strip().split(',')
            time = float(parts[4].split('us')[0].strip())
            packet_size = int(parts[3].split('bytes')[0].strip())
            ser1de_sizes.append(current_flat_size)
            ser1de_times.append(time)
            ser1de_packets.append(packet_size)

# Sort all data by flat object size
protobuf_data = sorted(zip(protobuf_sizes, protobuf_times, protobuf_packets))
ser1de_data = sorted(zip(ser1de_sizes, ser1de_times, ser1de_packets))

# Unzip the sorted data
protobuf_sizes, protobuf_times, protobuf_packets = zip(*protobuf_data)
ser1de_sizes, ser1de_times, ser1de_packets = zip(*ser1de_data)

# Create the first plot (Serialization Time)
plt.figure(figsize=(12, 6))
plt.axes([0.125, 0.15, 0.75, 0.75])  # [left, bottom, width=9/12, height]
plt.scatter(protobuf_sizes, protobuf_times, label='Protobuf', alpha=0.6)
plt.plot(protobuf_sizes, protobuf_times, 'b-', alpha=0.3)
plt.scatter(ser1de_sizes, ser1de_times, label='Ser1de', alpha=0.6)
plt.plot(ser1de_sizes, ser1de_times, 'r-', alpha=0.3)

plt.xscale('log')
plt.yscale('log')
plt.xlabel('Flat Object Size (bytes)', fontsize=22)
plt.ylabel('Serialization + Deserialization Time (μs)', fontsize=22)
plt.title('Performance Comparison: Protobuf vs Ser1de', fontsize=26)
plt.legend(fontsize=22)
plt.grid(True, alpha=0.3)
plt.margins(0.05)

# Set custom x-axis ticks
custom_ticks = [2e3, 5e3, 1e4, 2e4]
plt.gca().set_xticks(custom_ticks)
plt.gca().set_xticklabels(['2×10³', '5×10³', '10⁴', '2×10⁴'])

custom_ticks = [2e2, 5e2, 1e3]
plt.gca().set_yticks(custom_ticks)
plt.gca().set_yticklabels(['2×10²', '5×10²', '10³'])

plt.xticks(fontsize=22, rotation=0)
plt.yticks(fontsize=22, rotation=0)

# Disable minor ticks
plt.gca().minorticks_off()

plt.savefig('serialization_comparison.pdf', bbox_inches='tight', pad_inches=0.2)
plt.close()

# Create the second plot (Packet Size)
plt.figure(figsize=(12, 6))
plt.axes([0.125, 0.15, 0.75, 0.75])  # [left, bottom, width=9/12, height]
plt.scatter(protobuf_sizes, protobuf_packets, label='Protobuf', alpha=0.6)
plt.plot(protobuf_sizes, protobuf_packets, 'b-', alpha=0.3)
plt.scatter(ser1de_sizes, ser1de_packets, label='Ser1de', alpha=0.6)
plt.plot(ser1de_sizes, ser1de_packets, 'r-', alpha=0.3)

plt.xscale('log')
plt.yscale('log')
plt.xlabel('Flat Object Size (bytes)', fontsize=22)
plt.ylabel('Packet Size (bytes)', fontsize=22)
plt.title('Size Comparison: Protobuf vs Ser1de', fontsize=26)
plt.legend(fontsize=22)
plt.grid(True, alpha=0.3)
plt.margins(0.05)

# Set custom x-axis ticks
custom_ticks = [2e3, 5e3, 1e4, 2e4]
plt.gca().set_xticks(custom_ticks)
plt.gca().set_xticklabels(['2×10³', '5×10³', '10⁴', '2×10⁴'])

custom_ticks = [1e3, 2e3, 5e3, 1e4]
plt.gca().set_yticks(custom_ticks)
plt.gca().set_yticklabels(['10³', '2×10³', '5×10³', '10⁴'])

plt.xticks(fontsize=22, rotation=0)
plt.yticks(fontsize=22, rotation=0)

# Disable minor ticks
plt.gca().minorticks_off()

plt.savefig('size_comparison.pdf', bbox_inches='tight', pad_inches=0.2)
plt.close()