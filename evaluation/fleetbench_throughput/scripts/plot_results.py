import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# Read the benchmark output from a file
def read_results(filename):
    data = []
    current_type = None
    current_rps = None
    
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith('Request per Second:'):
                # Parse RPS
                current_rps = int(line.split(':')[1].split()[0])
            elif line.startswith('Tail Latencies (microseconds) - '):
                # Parse type
                type_str = line.split(' - ')[1]
                if 'Proto Serialize' in type_str:
                    current_type = 0
                elif 'Proto Deserialize' in type_str:
                    current_type = 1
                elif 'Ser1de Serialize' in type_str:
                    current_type = 2
                elif 'Ser1de Deserialize' in type_str:
                    current_type = 3
            elif line and not line.startswith('---') and not line.startswith('p50'):
                # Parse latency numbers - skip headers and separator lines
                parts = [int(x) for x in line.split() if x.isdigit()]
                if len(parts) == 3 and current_type is not None and current_rps is not None:
                    p50, p95, p99 = parts
                    data.append({
                        'type': current_type,
                        'rps': current_rps,
                        'p50': p50,
                        'p95': p95,
                        'p99': p99
                    })
    
    # Sort by type and rps for proper line plotting
    df = pd.DataFrame(data)
    df = df.sort_values(['type', 'rps'])
    return df

# ... existing code ...
def plot_latencies(df):
    type_names = {
        0: 'Proto Serialize',
        1: 'Proto Deserialize',
        2: 'SERenaDE Serialize',
        3: 'SERenaDE Deserialize'
    }
    
    colors = ['blue', 'red', 'green', 'orange']
    markers = ['o', 's', '^', 'D']
    
    # Create combined figure for p50 and p95
    plt.figure(figsize=(12, 7), constrained_layout=True)
    
    # First legend - line styles for percentiles
    line1, = plt.plot([], [], color='black', linestyle='-', label='p50')
    line2, = plt.plot([], [], color='black', linestyle=':', label='p95')
    lines = [line1, line2]
    
    plot_lines = []
    for i, (type_id, group) in enumerate(df.groupby('type')):
        # Plot p50 with solid line
        line, = plt.plot(group['rps'], group['p50'], 
                      color=colors[i],
                      marker=markers[i],
                      linewidth=2,
                      markersize=8)
        plot_lines.append(line)
        # Plot p95 with dotted line
        plt.plot(group['rps'], group['p95'],
                color=colors[i],
                marker=markers[i],
                linestyle=':',
                linewidth=2,
                markersize=8)
    
    plt.xlabel('Requests per Second', fontsize=22)
    plt.ylabel('Latency (microseconds)', fontsize=22)
    plt.title('50th and 95th Percentile Latency vs RPS', fontsize=26)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.xlim(2000, 10500)
    plt.ylim(0, 1500)
    plt.tick_params(axis='both', labelsize=22)
    
    # Create line style legend inside plot area
    first_legend = plt.legend(lines, ['p50', 'p95'], 
                            loc='upper left', 
                            fontsize=22)
    plt.gca().add_artist(first_legend)
    
    # Create data type legend inside plot area (lower right)
    plt.legend(plot_lines, type_names.values(),
              loc='upper right', 
              fontsize=18, 
              ncol=1,
              frameon=True)
    
    plt.savefig('p50_p95_latency.pdf')
    plt.close()
    
    # Create figure for p99
    plt.figure(figsize=(12, 8), constrained_layout=True)
    for i, (type_id, group) in enumerate(df.groupby('type')):
        plt.plot(group['rps'], group['p99'],
                label=type_names[type_id],
                color=colors[i],
                marker=markers[i],
                linewidth=2,
                markersize=8)
    
    plt.xlabel('Requests per Second', fontsize=22)
    plt.ylabel('P99 Latency (microseconds)', fontsize=22)
    plt.title('99th Percentile Latency vs RPS', fontsize=26)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.xlim(2000, 10000)
    plt.ylim(0, 10000)
    plt.tick_params(axis='both', labelsize=22)
    plt.legend(loc='upper left', fontsize=22)
    plt.savefig('p99_latency.pdf')
    plt.close()
# ... existing code ...

if __name__ == "__main__":
    # Assuming the benchmark output is saved to results.txt
    df = read_results('results.txt')
    plot_latencies(df) 