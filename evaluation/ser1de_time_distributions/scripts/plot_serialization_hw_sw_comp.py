import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import argparse
from pathlib import Path


def plot_accumulated_latencies(csv_file, output_dir=None):
    print(f"Reading CSV file: {csv_file}")
    df = pd.read_csv(csv_file)
    print(f"Loaded {len(df)} rows of data")

    # Group by MessageSize and calculate mean for all numeric columns
    df = df.groupby('MessageSize').mean().reset_index()
    df = df[df['MessageSize'] <= 100000].sort_values(by='MessageSize')

    # Calculate accumulated latencies
    blue = df['SchemaGeneration'] + df['GatherAndFirstCompression']
    red = df['SecondCompression'] + df['WaitForCompressions']
    green = df['SecondCompression'] + df['WaitForCompressions'] + df['MakeHeader']

    x = df['MessageSize']

    plt.figure(figsize=(12, 8))
    plt.plot(x, blue, label='Schema Generation + Gather and First Compression', color='blue', linewidth=2)
    plt.plot(x, red, label='Second Compression + Wait for Compressions', color='red', linewidth=2)
    plt.plot(x, green, label='Second Compression + Wait for Compressions + Make Header', color='green', linewidth=2)

    plt.xlabel('Message size (bytes)', fontsize=22)
    plt.ylabel('Latency (nanoseconds)', fontsize=22)
    plt.title('Accumulated Serialization Latencies', fontsize=26)
    plt.legend(fontsize=18)
    plt.xticks(fontsize=16)
    plt.yticks(fontsize=16)
    plt.tight_layout()

    if output_dir:
        output_path = Path(output_dir) / "serialization_accumulated_latencies.pdf"
        print(f"Saving plot to {output_path}...")
        plt.savefig(output_path, format='pdf', dpi=300, bbox_inches='tight')
        print("Plot saved successfully.")
    else:
        print("Displaying plot...")
        plt.show()


def main():
    parser = argparse.ArgumentParser(description='Plot accumulated serialization latencies as line plot')
    parser.add_argument('--input', type=str, required=True, help='Path to serialization CSV file')
    parser.add_argument('--output', type=str, help='Directory to save plot (optional)')
    args = parser.parse_args()

    if args.output:
        output_dir = Path(args.output)
        output_dir.mkdir(parents=True, exist_ok=True)
        print(f"Output directory created/verified: {output_dir}")
    else:
        output_dir = None
        print("No output directory specified, plot will be displayed")

    plot_accumulated_latencies(args.input, output_dir)

if __name__ == "__main__":
    main()
