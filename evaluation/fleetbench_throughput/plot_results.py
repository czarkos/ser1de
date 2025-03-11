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

def plot_latencies(df):
    type_names = {
        0: 'Proto Serialize',
        1: 'Proto Deserialize',
        2: 'Ser1de Serialize',
        3: 'Ser1de Deserialize'
    }
    
    colors = ['blue', 'red', 'green', 'orange']
    markers = ['o', 's', '^', 'D']
    
    # Create figure for p50
    plt.figure(figsize=(12, 7))
    for i, (type_id, group) in enumerate(df.groupby('type')):
        plt.plot(group['rps'], group['p50'], 
                label=type_names[type_id],
                color=colors[i],
                marker=markers[i],
                linewidth=2,
                markersize=8)
    
    plt.xlabel('Requests per Second')
    plt.ylabel('P50 Latency (microseconds)')
    plt.title('50th Percentile Latency vs RPS')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.ylim(0, 1000)
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.tight_layout()
    plt.savefig('p50_latency.png', bbox_inches='tight', dpi=300)
    plt.close()
    
    # Create figure for p95
    plt.figure(figsize=(12, 7))
    for i, (type_id, group) in enumerate(df.groupby('type')):
        # Plot the main line up to 2000
        plt.plot(group['rps'], group['p95'].clip(upper=2000),
                label=type_names[type_id],
                color=colors[i],
                marker=markers[i],
                linewidth=2,
                markersize=8)
        
        # Plot dotted lines for values exceeding 2000
        high_values = group[group['p95'] > 2000]
        if not high_values.empty:
            plt.plot(high_values['rps'], [2000] * len(high_values),
                    color=colors[i],
                    marker='x',
                    markersize=10,
                    linestyle='none')
    
    plt.xlabel('Requests per Second')
    plt.ylabel('P95 Latency (microseconds)')
    plt.title('95th Percentile Latency vs RPS')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.ylim(0, 2000)
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.tight_layout()
    plt.savefig('p95_latency.png', bbox_inches='tight', dpi=300)
    plt.close()
    
    # Create figure for p99
    plt.figure(figsize=(12, 7))
    for i, (type_id, group) in enumerate(df.groupby('type')):
        # Plot the main line up to 10000
        plt.plot(group['rps'], group['p99'].clip(upper=10000),
                label=type_names[type_id],
                color=colors[i],
                marker=markers[i],
                linewidth=2,
                markersize=8)
        
        # Plot dotted lines for values exceeding 10000
        high_values = group[group['p99'] > 10000]
        if not high_values.empty:
            plt.plot(high_values['rps'], [10000] * len(high_values),
                    color=colors[i],
                    marker='x',
                    markersize=10,
                    linestyle='none')
    
    plt.xlabel('Requests per Second')
    plt.ylabel('P99 Latency (microseconds)')
    plt.title('99th Percentile Latency vs RPS')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.ylim(0, 10000)
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.tight_layout()
    plt.savefig('p99_latency.png', bbox_inches='tight', dpi=300)
    plt.close()

if __name__ == "__main__":
    # Assuming the benchmark output is saved to results.txt
    df = read_results('results.txt')
    plot_latencies(df) 