#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import argparse
from pathlib import Path

def create_stacked_bar_plot(csv_file, plot_type, output_dir=None):
    """
    Create a stacked bar plot showing the distribution of time spent by operation
    across different message sizes.
    
    Args:
        csv_file: Path to the CSV file with latency data
        plot_type: 'serialize' or 'parse' to determine labels and colors
        output_dir: Directory to save the plot (if None, plot is displayed)
    """
    print(f"Processing {plot_type} data from {csv_file}...")
    
    # Read the CSV file
    print(f"  Reading CSV file...")
    df = pd.read_csv(csv_file)
    print(f"  Loaded {len(df)} rows of data")

    # Group by MessageSize and calculate mean for all numeric columns
    print(f"  Grouping data by message size and calculating averages...")
    df = df.groupby('MessageSize').mean().reset_index()
    print(f"  Data grouped into {len(df)} unique message sizes")

    
    # Set up colors and labels based on plot type
    print(f"  Setting up plot configuration for {plot_type}...")
    stack_columns = []
    labels = []
    if plot_type == 'serialize':
        labels = [
            'Schema Generation',
            'Gather and First Compression Job Submission', 
            'Second Compression Job Submission',
            'Wait for Compressions (Barrier)',
            'Make Header'
        ]
    else:  # parse
        labels = [
            'Read Header',
            'Decompress',
            'Allocate and Create Schema',
            'Wait for Decompression',
            'Scatter'
        ]
    if plot_type == 'serialize':
        stack_columns = [
            'SchemaGeneration', 
            'GatherAndFirstCompression', 
            'SecondCompression', 
            'WaitForCompressions', 
            'MakeHeader'
        ]

        title = 'Distribution of Time Spent by Operation (Serialization)'
        
    else:  # parse
        stack_columns = [
            'ReadHeader', 
            'Decompress', 
            'AllocateAndCreateSchema', 
            'WaitForDecompression', 
            'Scatter'
        ]
        
        title = 'Distribution of Time Spent by Operation (Deserialization)'
    
    # Create the plot
    print(f"  Creating {plot_type} plot...")
    
    df_sorted = df[df['MessageSize'] <= 100000].sort_values(by='MessageSize')
    ax = df_sorted.set_index("MessageSize")[stack_columns].plot(kind='bar', stacked=True, figsize=(12, 8))

    ax.tick_params(axis='both', which='major', labelsize=22)

    # Adding labels and title
    print(f"  Adding labels and formatting...")

    plt.xlabel('Message size (bytes)', fontsize=22)
    plt.ylabel('Time Spent (nanoseconds)', fontsize=22)
    plt.title(title, fontsize=26)
    
    # Add legend with explicit label mapping
    handles = ax.get_legend_handles_labels()[0]
    plt.legend(handles=handles, labels=labels, loc='upper left', fontsize=20)
    
    # Save or display the plot
    if output_dir:
        output_path = Path(output_dir) / f"{plot_type}_time_distribution.pdf"
        print(f"  Saving plot to {output_path}...")
        plt.savefig(output_path, format='pdf', dpi=300, bbox_inches='tight')
        print(f"  Plot saved successfully")
    else:
        print(f"  Displaying plot...")
        plt.show()
    
    print(f"Completed {plot_type} plot generation")

def main():
    print("Starting time distribution analysis...")
    
    parser = argparse.ArgumentParser(description='Create stacked bar plots of time distributions')
    parser.add_argument('--serialize', type=str, help='Path to serialization CSV file')
    parser.add_argument('--parse', type=str, help='Path to parsing CSV file')
    parser.add_argument('--output', type=str, help='Directory to save plots (optional)')
    
    args = parser.parse_args()
    print(f"Arguments parsed: serialize={args.serialize}, parse={args.parse}, output={args.output}")
    
    if args.output:
        output_dir = Path(args.output)
        output_dir.mkdir(parents=True, exist_ok=True)
        print(f"Output directory created/verified: {output_dir}")
    else:
        output_dir = None
        print("No output directory specified, plots will be displayed")
    
    if args.serialize:
        create_stacked_bar_plot(args.serialize, 'serialize', output_dir)
    
    if args.parse:
        create_stacked_bar_plot(args.parse, 'parse', output_dir)
    
    if not args.serialize and not args.parse:
        print("Please provide at least one CSV file path using --serialize or --parse")
    
    print("Analysis complete!")

if __name__ == "__main__":
    main()