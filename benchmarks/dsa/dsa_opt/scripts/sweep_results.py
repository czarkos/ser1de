import subprocess
import os
import argparse
import pandas as pd
import matplotlib.pyplot as plt

protobuf_path = '/home/christos/dev/protobuf/'
benchmark_path = protobuf_path + 'tachidromos/buffer_size_impact/scripts/'

def dist_plot(df, name):
    # Focusing on the columns to stack
    stack_columns = ["gather_schema", "gather", "compression", "scatter_schema", "decompression", "scatter", "allocation"]

    df_sorted = df.sort_values(by="gather_out(bytes)")

    # Sort the DataFrame by index
    df_sorted = df_sorted.set_index("buffersize").sort_index()

    # Create a stacked bar plot
    ax = df_sorted[stack_columns].plot(kind='bar', stacked=True, figsize=(10, 8))

    # Adding labels and title
    plt.xlabel("Message size")
    plt.ylabel("Time Spent")
    plt.title("Distribution of Time Spent by Operation")
    plt.legend(title="Operations", loc='upper left', bbox_to_anchor=(1.05, 1), borderaxespad=0.)

    plt.savefig(name, dpi=300, bbox_inches='tight')
    plt.show()


def ser_perf_plot(df, name):
    # Calculate required sums for each plot line
    df['gather_schema_gather_compression'] = df['gather_schema'] + df['gather'] + df['compression']
    df['gather_compression']               = df['gather'] + df['compression']

    # Create a pivot table for the plots
    #plot_data = df.pivot_table(index=['gather_out(bytes)', 'width', 'depth'], 
    plot_data = df.pivot_table(index=['buffersize', 'width', 'depth'], 
                               values=['gather_schema_gather_compression', 'gather_compression', 'serialization'],
                               aggfunc='sum').reset_index()

    # Get unique dimensions for subplots
    widths = plot_data['width'].unique()
    depths = plot_data['depth'].unique()

    # Create subplots
    fig, axs = plt.subplots(nrows=len(depths), ncols=len(widths), figsize=(15, 10))
    for i, depth in enumerate(depths):
        for j, width in enumerate(widths):
            if len(depths) == 1 and len(widths) == 1:
                ax = axs
            elif len(depths) == 1:
                ax = axs[j]
            elif len(widths) == 1:
                ax = axs[i]
            else:
                ax = axs[i, j]
            subset = plot_data[(plot_data['width'] == width) & (plot_data['depth'] == depth)]
            print(f"depth: {depth}, width: {width}")
            print(subset)
            print("---------------")

            # Plot each line
            ax.plot(subset['buffersize'], subset['gather_schema_gather_compression'], label='gather_schema+gather+compression', linewidth=2, marker='o', color='red')
            ax.plot(subset['buffersize'], subset['gather_compression'], label='gather+compression', linewidth=2, marker='o', color='blue')
            ax.plot(subset['buffersize'], subset['serialization'], label='serialization', linewidth=2, marker='o', color='yellow')
            #ax.plot(subset['gather_out(bytes)'], subset['gather_schema_gather_compression'], label='gather_schema+gather+compression', linewidth=2, marker='o', color='red')
            #ax.plot(subset['gather_out(bytes)'], subset['gather_compression'], label='gather+compression', linewidth=2, marker='o', color='blue')
            #ax.plot(subset['gather_out(bytes)'], subset['serialization'], label='serialization', linewidth=2, marker='o', color='yellow')

            ax.set_title(f'Width {width}, Depth {depth}')
            ax.set_xlabel('Buffer Sizes')
            ax.set_ylabel('Time Spent')

            # Set independent axes limits
            #ax.set_xlim(left=0.9*subset['gather_out(bytes)'].min(), right=1.1*subset['gather_out(bytes)'].max())
            #ax.set_xlim(left=0.9*subset['gather_out(bytes)'].min(), right=1.1*subset['gather_out(bytes)'].max())
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
    df['scatter_schema_decompression_scatter'] = df['scatter_schema'] + df['scatter'] + df['decompression'] + df['allocation']
    df['decompression_scatter']                = df['scatter'] + df['decompression']

    # Create a pivot table for the plots
    #plot_data = df.pivot_table(index=['gather_out(bytes)', 'width', 'depth'], 
    plot_data = df.pivot_table(index=['buffersize', 'width', 'depth'], 
                               values=['scatter_schema_decompression_scatter', 'decompression_scatter', 'deserialization'],
                               aggfunc='sum').reset_index()

    # Get unique dimensions for subplots
    widths = plot_data['width'].unique()
    depths = plot_data['depth'].unique()

    # Create subplots
    fig, axs = plt.subplots(nrows=len(depths), ncols=len(widths), figsize=(15, 10))
    for i, depth in enumerate(depths):
        for j, width in enumerate(widths):
            if len(depths) == 1 and len(widths) == 1:
                ax = axs
            elif len(depths) == 1:
                ax = axs[j]
            elif len(widths) == 1:
                ax = axs[i]
            else:
                ax = axs[i, j]
            subset = plot_data[(plot_data['width'] == width) & (plot_data['depth'] == depth)]
            print(f"depth: {depth}, width: {width}")
            print(subset)
            print("---------------")

            # Plot each line
            ax.plot(subset['buffersize'], subset['scatter_schema_decompression_scatter'], label='scatter_schema+decompression+scatter', linewidth=2, marker='o', color='red')
            ax.plot(subset['buffersize'], subset['decompression_scatter'], label='decompression+scatter', linewidth=2, marker='o', color='blue')
            ax.plot(subset['buffersize'], subset['deserialization'], label='deserialization', linewidth=2, marker='o', color='yellow')
            #ax.plot(subset['gather_out(bytes)'], subset['scatter_schema_decompression_scatter'], label='scatter_schema+decompression+scatter', linewidth=2, marker='o', color='red')
            #ax.plot(subset['gather_out(bytes)'], subset['decompression_scatter'], label='decompression+scatter', linewidth=2, marker='o', color='blue')
            #ax.plot(subset['gather_out(bytes)'], subset['deserialization'], label='deserialization', linewidth=2, marker='o', color='yellow')

            ax.set_title(f'Width {width}, Depth {depth}')
            ax.set_xlabel('Buffer Sizes')
            ax.set_ylabel('Time Spent')

            # Set independent axes limits
            #ax.set_xlim(left=0.9*subset['gather_out(bytes)'].min(), right=1.1*subset['gather_out(bytes)'].max())
            ax.set_ylim(bottom=0, top=max(subset['scatter_schema_decompression_scatter'].max(), subset['decompression_scatter'].max(), subset['deserialization'].max())*1.1)

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
        elif "gather" in line:
            gather = get_line_mean(line, warmups)
        elif "scatter_schemas" in line:
            scatter_schema = get_line_mean(line, warmups)
        elif "scatter" in line:
            scatter = get_line_mean(line, warmups)
        elif "allocation" in line:
            alloc = get_line_mean(line, warmups)
    return ser, deser, gather_schema, gather, comp, scatter_schema, decomp, scatter, alloc, gather_out, compress_out, decompress_out


parser = argparse.ArgumentParser()
parser.add_argument('--widths', metavar='N', type=int, nargs='+', default=[1,2,4])
parser.add_argument('--depths', metavar='N', type=int, nargs='+', default=[1])
parser.add_argument('--ratios', metavar='N', type=int, nargs='+', default=[4])
parser.add_argument('--sets', metavar='N', type=int, nargs='+', default=[1,2,3])
parser.add_argument('--buffersizes', metavar='N', type=int, nargs='+', default=[512, 1024, 2048, 4096, 8192, 12288, 16384, 20480, 24576, 28672, 32768])
#parser.add_argument('--buffersizes', metavar='N', type=int, nargs='+', default=[512, 1024, 2048, 4096, 8192, 16384, 32768])
#parser.add_argument('--buffersizes', metavar='N', type=int, nargs='+', default=[1024, 2048])

parser.add_argument("-p", "--plot", action="store_true", help="Generate plots")
args = parser.parse_args()

df = []
col_names = ["width", "depth", "#varint", "#string", "buffersize", "serialization", "deserialization", "gather_schema", "gather", "compression", "scatter_schema", "decompression", "scatter", "allocation", "gather_out(bytes)", "compress_out(bytes)", "decompress_out(bytes)"]
for width in args.widths:
    for depth in args.depths:
        for ratio in args.ratios:
            for fset in args.sets:
                for bsize in args.buffersizes:
                    num_varint = ratio * fset
                    num_string = fset

                    cmd = f'python3 {benchmark_path}bench.py --proto --build --run --string -w {width} -d {depth} -nv {num_varint} -ns {num_string} -bf {bsize}'
                    print(cmd)

                    res = subprocess.run(cmd, shell=True, text=True, capture_output=True).stdout
                    print(res)

                    ser, deser, gather_schema, gather, comp, scatter_schema, decomp, scatter, alloc, gather_out, compress_out, decompress_out = process_output(res)
                    elem = [width, depth, num_varint, num_string, bsize, ser, deser, gather_schema, gather, comp, scatter_schema, decomp, scatter, alloc, gather_out, compress_out, decompress_out]

                    df += [elem]

data_frame = pd.DataFrame(df, columns=col_names)
print(data_frame.to_string())

if args.plot:
    plt_dir = 'plots' + "_w_" + "_".join(map(str, args.widths)) + "_d_" + "_".join(map(str, args.depths)) + "_r_" + "_".join(map(str, args.ratios)) + "_s_" + "_".join(map(str, args.sets))
    if not os.path.exists(plt_dir):
        os.makedirs(plt_dir)
        print("Created" + plt_dir + "directory")
    dist_plot(data_frame, plt_dir + '/time_distribution_plot.png')
    ser_perf_plot(data_frame, plt_dir + '/perf_serialization.png')
    deser_perf_plot(data_frame, plt_dir + '/perf_deserialization.png')