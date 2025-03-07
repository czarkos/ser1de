import subprocess
import os
import argparse
import pandas as pd
import matplotlib.pyplot as plt

protobuf_path = '/home/christos/dev/ser1de/protobuf/'
benchmark_path = '/home/christos/dev/ser1de/benchmarks/fleetbench_compatibility/scripts/'

def dist_plot(df, name):
    # Focusing on the columns to stack
    stack_columns = ["gather_schema", "gather", "compression", "make_header", "read_header", "decompression", "allocation", "scatter"]

    df_sorted = df.sort_values(by="gather_out(bytes)")

    # Create a stacked bar plot
    ax = df_sorted.set_index("gather_out(bytes)")[stack_columns].plot(kind='bar', stacked=True, figsize=(10, 8))

    # Adding labels and title
    plt.xlabel("Message size")
    plt.ylabel("Time Spent")
    plt.title("Distribution of Time Spent by Operation")
    plt.legend(title="Operations", loc='upper left', bbox_to_anchor=(1.05, 1), borderaxespad=0.)

    plt.savefig(name, dpi=300, bbox_inches='tight')
    plt.show()


def ser_perf_plot(df, name):
    # Calculate required sums for each plot line
    df['gather_schema_gather_compression'] = df['gather_schema'] + df['gather'] + df['compression'] + df['make_header']
    df['gather_compression']               = df['gather'] + df['compression']

    # Create a pivot table for the plots
    plot_data = df.pivot_table(index=['gather_out(bytes)', 'width', 'depth'], 
                               values=['gather_schema_gather_compression', 'gather_compression', 'serialization'],
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
            ax.plot(subset['gather_out(bytes)'], subset['gather_schema_gather_compression'], label='ser1de serialization', linewidth=2, marker='o', color='red')
            ax.plot(subset['gather_out(bytes)'], subset['gather_compression'], label='gather+compression', linewidth=2, marker='o', color='blue')
            ax.plot(subset['gather_out(bytes)'], subset['serialization'], label='protobuf serialization', linewidth=2, marker='o', color='green')

            ax.set_title(f'Width {width}, Depth {depth}')
            ax.set_xlabel('Total Message Size (bytes)')
            ax.set_ylabel('Time Spent (ns)')

            ax.grid(True)
            # Set independent axes limits
            ax.set_xlim(left=0.9*subset['gather_out(bytes)'].min(), right=1.1*subset['gather_out(bytes)'].max())
            ax.set_ylim(bottom=0, top=max(subset['gather_schema_gather_compression'].max(), subset['gather_compression'].max(), subset['serialization'].max())*1.1)

            # Move the legend to the top right subplot
            if depth == depths.min() and width == widths.max():
                ax.legend()

    # Adjust layout
    plt.tight_layout(pad=2.0)
    plt.savefig(name, dpi=300, bbox_inches='tight')
    plt.show()


def deser_perf_plot(df, name):
    # Calculate required sums for each plot line
    df['scatter_schema_decompression_scatter'] = df['scatter'] + df['decompression'] + df['allocation'] + df['read_header']
    df['decompression_scatter']                = df['scatter'] + df['decompression']

    # Create a pivot table for the plots
    plot_data = df.pivot_table(index=['gather_out(bytes)', 'width', 'depth'], 
                               values=['scatter_schema_decompression_scatter', 'decompression_scatter', 'deserialization'],
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
            ax.plot(subset['gather_out(bytes)'], subset['scatter_schema_decompression_scatter'], label='ser1de deserialization', linewidth=2, marker='o', color='red')
            ax.plot(subset['gather_out(bytes)'], subset['decompression_scatter'], label='decompression+scatter', linewidth=2, marker='o', color='blue')
            ax.plot(subset['gather_out(bytes)'], subset['deserialization'], label='protobuf deserialization', linewidth=2, marker='o', color='green')

            ax.set_title(f'Width {width}, Depth {depth}')
            ax.set_xlabel('Total Message Size (bytes)')
            ax.set_ylabel('Time Spent (ns)')
            ax.grid(True)

            # Set independent axes limits
            ax.set_xlim(left=0.9*subset['gather_out(bytes)'].min(), right=1.1*subset['gather_out(bytes)'].max())
            ax.set_ylim(bottom=0, top=max(subset['scatter_schema_decompression_scatter'].max(), subset['decompression_scatter'].max(), subset['deserialization'].max())*1.1)

            # Move the legend to the top right subplot
            if depth == depths.min() and width == widths.max():
                ax.legend()

    # Adjust layout
    plt.tight_layout(pad=2.0)
    plt.savefig(name, dpi=300, bbox_inches='tight')
    plt.show()


def end2end_plot(df, name):
    # Serialization path: gather_schema + gather + compression + make_header
    # Deserialization path: read_header + decompression + scatter + scatter_schema + allocation
    
    df['ser1de'] = df['gather_schema'] + df['gather'] + df['compression'] + df['make_header'] + df['read_header'] + df['decompression'] + df['allocation'] + df['scatter']
    df['protobuf'] = df['deserialization'] + df['serialization']

    # Create a pivot table for the plots
    plot_data = df.pivot_table(index=['gather_out(bytes)', 'width', 'depth'], 
                               values=['ser1de', 'protobuf'],
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
            ax.plot(subset['gather_out(bytes)'], subset['ser1de'], label='ser1de', linewidth=2, marker='o', color='red')
            ax.plot(subset['gather_out(bytes)'], subset['protobuf'], label='protobuf', linewidth=2, marker='o', color='blue')

            ax.set_title(f'Width {width}, Depth {depth}')
            ax.set_xlabel('Total Message Size (bytes)')
            ax.set_ylabel('Time Spent (ns)')
            ax.grid(True)

            # Set independent axes limits
            ax.set_xlim(left=0.9*subset['gather_out(bytes)'].min(), right=1.1*subset['gather_out(bytes)'].max())
            ax.set_ylim(bottom=0, top=max(subset['ser1de'].max(), subset['protobuf'].max())*1.1)

            # Move the legend to the top right subplot
            if depth == depths.min() and width == widths.max():
                ax.legend()

    # Adjust layout
    plt.tight_layout(pad=2.0)
    plt.savefig(name, dpi=300, bbox_inches='tight')
    plt.show()


def process_output(res, warmups=1):
    def get_line_mean(line, warmups):
        print(line)
        nums = [int(_) for _ in line.strip().split(", ")[1:]]
        nums = nums[warmups:]
        if len(nums) == 0:
            return -1
        return round(sum(nums) / len(nums), 2)
    def get_line_num(line):
        return int(line.strip().split(", ")[-1])

    ser, deser, comp, decomp, scatter, gather, alloc, gather_schema, scatter_schema = None, None, None, None, None, None, None, None, None
    gather_out, compress_out, decompress_out = None, None, None
    make_header, read_header = None, None
    for line in res.split("\n"):
        if "deserialization" in line:
            deser = get_line_mean(line, warmups)
        elif "serialization" in line:
            ser = get_line_mean(line, warmups)
        elif "decompress_out" in line:
            decompress_out = get_line_num(line)
        elif "compress_out" in line:
            compress_out = get_line_num(line)
        elif "gather_out" in line:
            gather_out = get_line_num(line)
        elif "decompression" in line:
            decomp = get_line_mean(line, warmups)
        elif "compression" in line:
            comp = get_line_mean(line, warmups)
        elif "gather_schemas" in line:
            gather_schema = get_line_mean(line, warmups)
        elif "scatter_schemas" in line:
            scatter_schema = get_line_mean(line, warmups)
        elif "allocation" in line:
            alloc = get_line_mean(line, warmups)
        elif "gather" in line:
            gather = get_line_num(line)
            #gather = get_line_mean(line, warmups)
        elif "scatter" in line:
            #scatter = get_line_mean(line, warmups)
            scatter = get_line_num(line)
        elif "make_header" in line:
            make_header = get_line_mean(line, warmups)
        elif "read_header" in line:
            read_header = get_line_mean(line, warmups)
    return ser, deser, gather_schema, gather, comp, scatter_schema, decomp, scatter, alloc, gather_out, compress_out, decompress_out, make_header, read_header


parser = argparse.ArgumentParser()
parser.add_argument('--widths', metavar='N', type=int, nargs='+', default=[3, 4, 5])
parser.add_argument('--depths', metavar='N', type=int, nargs='+', default=[2, 3, 4])
parser.add_argument('--ratios', metavar='N', type=int, nargs='+', default=[4])
parser.add_argument('--sets', metavar='N', type=int, nargs='+', default=[1, 2])

parser.add_argument("-p", "--plot", action="store_true", help="Generate plots")
args = parser.parse_args()

df = []
col_names = ["width", "depth", "#varint", "#string", "serialization", "deserialization", "gather_schema", "gather", "compression", "scatter_schema", "decompression", "scatter", "allocation", "gather_out(bytes)", "compress_out(bytes)", "decompress_out(bytes)", "make_header", "read_header"]
for width in args.widths:
    for depth in args.depths:
        for ratio in args.ratios:
            for fset in args.sets:
                num_varint = ratio * fset
                num_string = fset

                cmd = f'python3 {benchmark_path}bench.py --proto --build --run --string -w {width} -d {depth} -nv {num_varint} -ns {num_string}'
                print(cmd)

                res = subprocess.run(cmd, shell=True, text=True, capture_output=True).stdout
                print(res)

                ser, deser, gather_schema, gather, comp, scatter_schema, decomp, scatter, alloc, gather_out, compress_out, decompress_out, make_header, read_header = process_output(res)
                elem = [width, depth, num_varint, num_string, ser, deser, gather_schema, gather, comp, scatter_schema, decomp, scatter, alloc, gather_out, compress_out, decompress_out, make_header, read_header]

                df += [elem]

data_frame = pd.DataFrame(df, columns=col_names)
print(data_frame.to_string())

if args.plot:
    plt_dir = 'plots' + "_w_" + "_".join(map(str, args.widths)) + "_d_" + "_".join(map(str, args.depths)) + "_r_" + "_".join(map(str, args.ratios)) + "_s_" + "_".join(map(str, args.sets))
    if not os.path.exists(plt_dir):
        os.makedirs(plt_dir)
        print("Created" + plt_dir + "directory")
    data_frame.to_csv(plt_dir + '/dsa_sensitivity.csv', index=False)  # Save without the index
    ser_perf_plot(data_frame, plt_dir + '/perf_serialization.png')
    deser_perf_plot(data_frame, plt_dir + '/perf_deserialization.png')
    end2end_plot(data_frame, plt_dir + '/end2end_sensitivity.png')
    dist_plot(data_frame, plt_dir + '/time_distribution_plot.png')