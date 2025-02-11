import pandas as pd
import matplotlib.pyplot as plt
import os
import argparse

def dist_plot(df, name):
    stack_columns = ["gather_schema", "dsa_gather", "compression", "scatter_schema", "decompression", "dsa_scatter", "allocation"]
    df_sorted = df.sort_values(by="gather_out(bytes)")
    ax = df_sorted.set_index("gather_out(bytes)")[stack_columns].plot(kind='bar', stacked=True, figsize=(10, 8))
    plt.xlabel("Message size")
    plt.ylabel("Time Spent")
    plt.title("Distribution of Time Spent by Operation")
    plt.legend(title="Operations", loc='upper left', bbox_to_anchor=(1.05, 1), borderaxespad=0.)
    plt.savefig(name, dpi=300, bbox_inches='tight')
    plt.close()

def ser_perf_plot(df, name):
    df['gather_schema_gather_compression'] = df['gather_schema'] + df['dsa_gather'] + df['compression']
    df['gather_compression'] = df['dsa_gather'] + df['compression']
    
    plot_data = df.pivot_table(index=['gather_out(bytes)', 'width', 'depth'],
                              values=['gather_schema_gather_compression', 'gather_compression', 'serialization'],
                              aggfunc='sum').reset_index()
    
    widths = plot_data['width'].unique()
    depths = plot_data['depth'].unique()
    
    fig, axs = plt.subplots(nrows=len(depths), ncols=len(widths), figsize=(15, 10))
    for i, depth in enumerate(depths):
        for j, width in enumerate(widths):
            ax = axs[i, j] if len(depths) > 1 else axs[j]
            subset = plot_data[(plot_data['width'] == width) & (plot_data['depth'] == depth)]
            
            ax.plot(subset['gather_out(bytes)'], subset['gather_schema_gather_compression'], 
                   label='ser1de serialization', linewidth=2, marker='o', color='red')
            ax.plot(subset['gather_out(bytes)'], subset['gather_compression'], 
                   label='gather+compression', linewidth=2, marker='o', color='blue')
            ax.plot(subset['gather_out(bytes)'], subset['serialization'], 
                   label='protobuf serialization', linewidth=2, marker='o', color='green')
            
            ax.set_title(f'Width {width}, Depth {depth}')
            ax.set_xlabel('Total Message Size (bytes)')
            ax.set_ylabel('Time Spent (ns)')
            ax.grid(True)
            
            if depth == depths.min() and width == widths.max():
                ax.legend()
    
    plt.tight_layout(pad=2.0)
    plt.savefig(name, dpi=300, bbox_inches='tight')
    plt.close()

def deser_perf_plot(df, name):
    df['scatter_schema_decompression_scatter'] = df['scatter_schema'] + df['dsa_scatter'] + df['decompression'] + df['allocation']
    df['decompression_scatter'] = df['dsa_scatter'] + df['decompression']
    
    plot_data = df.pivot_table(index=['gather_out(bytes)', 'width', 'depth'],
                              values=['scatter_schema_decompression_scatter', 'decompression_scatter', 'deserialization'],
                              aggfunc='sum').reset_index()
    
    widths = plot_data['width'].unique()
    depths = plot_data['depth'].unique()
    
    fig, axs = plt.subplots(nrows=len(depths), ncols=len(widths), figsize=(15, 10))
    for i, depth in enumerate(depths):
        for j, width in enumerate(widths):
            ax = axs[i, j] if len(depths) > 1 else axs[j]
            subset = plot_data[(plot_data['width'] == width) & (plot_data['depth'] == depth)]
            
            ax.plot(subset['gather_out(bytes)'], subset['scatter_schema_decompression_scatter'], 
                   label='ser1de deserialization', linewidth=2, marker='o', color='red')
            ax.plot(subset['gather_out(bytes)'], subset['decompression_scatter'], 
                   label='decompression+scatter', linewidth=2, marker='o', color='blue')
            ax.plot(subset['gather_out(bytes)'], subset['deserialization'], 
                   label='protobuf deserialization', linewidth=2, marker='o', color='green')
            
            ax.set_title(f'Width {width}, Depth {depth}')
            ax.set_xlabel('Total Message Size (bytes)')
            ax.set_ylabel('Time Spent (ns)')
            ax.grid(True)
            
            if depth == depths.min() and width == widths.max():
                ax.legend()
    
    plt.tight_layout(pad=2.0)
    plt.savefig(name, dpi=300, bbox_inches='tight')
    plt.close()

def dsa_sensitivity_plot(df, name):
    # Calculate required sums for each plot line
    df['dsa_scatter_gather'] = df['dsa_gather'] + df['dsa_scatter']
    df['memcpy_scatter_gather'] = df['memcpy_gather'] + df['memcpy_scatter']

    print(df)

    # Create a pivot table for the plots
    plot_data = df.pivot_table(index=['gather_out(bytes)', 'width', 'depth'], 
                               values=['dsa_scatter_gather', 'memcpy_scatter_gather'],
                               aggfunc='sum').reset_index()

    # Get unique dimensions for subplots
    widths = plot_data['width'].unique()
    depths = plot_data['depth'].unique()

    # Create subplots
    fig, axs = plt.subplots(nrows=len(depths), ncols=len(widths), figsize=(15, 10))
    for i, depth in enumerate(depths):
        for j, width in enumerate(widths):
            ax = axs[i, j]
            subset = plot_data[(plot_data['width'] == width) & (plot_data['depth'] == depth)]
            print(f"depth: {depth}, width: {width}")
            print(subset)
            print("---------------")

            # Plot each line
            ax.plot(subset['gather_out(bytes)'], subset['dsa_scatter_gather'], label='dsa (hw)', linewidth=2, marker='o', color='red')
            ax.plot(subset['gather_out(bytes)'], subset['memcpy_scatter_gather'], label='memcpy (sw)', linewidth=2, marker='o', color='blue')

            ax.set_title(f'Width {width}, Depth {depth}')
            ax.set_xlabel('Total Message Size (bytes)')
            ax.set_ylabel('Time Spent (ns)')
            ax.grid(True)

            # Set independent axes limits
            ax.set_xlim(left=0.9*subset['gather_out(bytes)'].min(), right=1.1*subset['gather_out(bytes)'].max())
            ax.set_ylim(bottom=0, top=max(subset['dsa_scatter_gather'].max(), subset['memcpy_scatter_gather'].max())*1.1)

            # Move the legend to the top right subplot
            if depth == depths.min() and width == widths.max():
                ax.legend()

    # Adjust layout
    plt.tight_layout(pad=2.0)
    plt.savefig(name, dpi=300, bbox_inches='tight')
    plt.show()

def gather_plot(df, name):
    # Calculate required sums for each plot line
    #df['dsa_gather'] = df['dsa_gather'] + df['dsa_scatter']
    df['memcpy_and_gather'] = df['memcpy_gather'] + df['gather_schema']

    print(df)

    # Create a pivot table for the plots
    plot_data = df.pivot_table(index=['gather_out(bytes)', 'width', 'depth'], 
                               values=['dsa_gather', 'memcpy_gather', 'gather_schema', 'memcpy_and_gather'],
                               aggfunc='sum').reset_index()

    # Get unique dimensions for subplots
    widths = plot_data['width'].unique()
    depths = plot_data['depth'].unique()

    # Create subplots
    fig, axs = plt.subplots(nrows=len(depths), ncols=len(widths), figsize=(15, 10))
    for i, depth in enumerate(depths):
        for j, width in enumerate(widths):
            ax = axs[i, j]
            subset = plot_data[(plot_data['width'] == width) & (plot_data['depth'] == depth)]
            print(f"depth: {depth}, width: {width}")
            print(subset)
            print("---------------")

            # Plot each line
            ax.plot(subset['gather_out(bytes)'], subset['dsa_gather'], label='dsa gather (hw)', linewidth=2, marker='o', color='red')
            ax.plot(subset['gather_out(bytes)'], subset['memcpy_gather'], label='memcpy gather (sw)', linewidth=2, marker='o', color='blue')
            ax.plot(subset['gather_out(bytes)'], subset['gather_schema'], label='gather schema creation', linewidth=2, marker='o', color='green')
            ax.plot(subset['gather_out(bytes)'], subset['memcpy_and_gather'], label='sw schema creation and gather', linewidth=2, marker='o', color='purple')

            ax.set_title(f'Width {width}, Depth {depth}')
            ax.set_xlabel('Total Message Size (bytes)')
            ax.set_ylabel('Time Spent (ns)')
            ax.grid(True)

            # Set independent axes limits
            ax.set_xlim(left=0.9*subset['gather_out(bytes)'].min(), right=1.1*subset['gather_out(bytes)'].max())
            ax.set_ylim(bottom=0, top=max(subset['dsa_gather'].max(), subset['memcpy_gather'].max(), subset['gather_schema'].max(), subset['memcpy_and_gather'].max())*1.1)

            # Move the legend to the top right subplot
            if depth == depths.min() and width == widths.max():
                ax.legend()

    # Adjust layout
    plt.tight_layout(pad=2.0)
    plt.savefig(name, dpi=300, bbox_inches='tight')
    plt.show()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('csv_file', help='CSV file containing benchmark data')
    args = parser.parse_args()
    
    try:
        # Read CSV file
        df = pd.read_csv(args.csv_file)
        
        # Generate plots in current directory
        dist_plot(df, 'time_distribution_plot.png')
        ser_perf_plot(df, 'perf_serialization.png')
        deser_perf_plot(df, 'perf_deserialization.png')
        print("Plots generated successfully!")
        
    except FileNotFoundError:
        print(f"Error: Could not find file '{args.csv_file}'")
    except Exception as e:
        print(f"Error occurred: {str(e)}")

if __name__ == "__main__":
    main()