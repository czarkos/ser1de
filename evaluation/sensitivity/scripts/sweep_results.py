import subprocess
import os
import argparse
import pandas as pd
import matplotlib.pyplot as plt

protobuf_path = '/home/czarkos/dev/ser1de/protobuf/'
benchmark_path = '/home/czarkos/dev/ser1de/evaluation/sensitivity/scripts/'

def ser_perf_plot(df, name):
    # Create a pivot table for the plots
    plot_data = df.pivot_table(index=['gather_out(bytes)', 'width', 'depth'], 
                               values=['ser1de_serialization', 'proto_serialization'],
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
            ax.plot(subset['gather_out(bytes)'], subset['ser1de_serialization'], label='ser1de serialization', linewidth=2, marker='o', color='red')
            ax.plot(subset['gather_out(bytes)'], subset['proto_serialization'], label='protobuf serialization', linewidth=2, marker='o', color='green')

            ax.set_title(f'Width {width}, Depth {depth}')
            ax.set_xlabel('Total Message Size (bytes)')
            ax.set_ylabel('Time Spent (ns)')

            ax.grid(True)
            # Set independent axes limits
            ax.set_xlim(left=0.9*subset['gather_out(bytes)'].min(), right=1.1*subset['gather_out(bytes)'].max())
            ax.set_ylim(bottom=0, top=max(subset['ser1de_serialization'].max(), subset['proto_serialization'].max())*1.1)

            # Move the legend to the top right subplot
            if depth == depths.min() and width == widths.max():
                ax.legend()

    # Adjust layout
    plt.tight_layout(pad=2.0)
    plt.savefig(name, dpi=300, bbox_inches='tight')
    plt.show()


def deser_perf_plot(df, name):
    # Create a pivot table for the plots
    plot_data = df.pivot_table(index=['gather_out(bytes)', 'width', 'depth'], 
                               values=['ser1de_deserialization', 'proto_deserialization'],
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
            ax.plot(subset['gather_out(bytes)'], subset['ser1de_deserialization'], label='ser1de deserialization', linewidth=2, marker='o', color='red')
            ax.plot(subset['gather_out(bytes)'], subset['proto_deserialization'], label='protobuf deserialization', linewidth=2, marker='o', color='green')

            ax.set_title(f'Width {width}, Depth {depth}')
            ax.set_xlabel('Total Message Size (bytes)')
            ax.set_ylabel('Time Spent (ns)')
            ax.grid(True)

            # Set independent axes limits
            ax.set_xlim(left=0.9*subset['gather_out(bytes)'].min(), right=1.1*subset['gather_out(bytes)'].max())
            ax.set_ylim(bottom=0, top=max(subset['ser1de_deserialization'].max(), subset['proto_deserialization'].max())*1.1)

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
    
    df['ser1de'] = df['ser1de_serialization'] + df['ser1de_deserialization']
    df['protobuf'] =  df['proto_serialization'] + df['proto_deserialization']

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
            ax.plot(subset['gather_out(bytes)'], subset['protobuf'], label='protobuf', linewidth=2, marker='o', color='green')

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

    proto_ser, proto_deser = None, None
    ser1de_ser, ser1de_deser = None, None
    gather_out, compress_out = None, None
    for line in res.split("\n"):
        if "proto_deserialization" in line:
            proto_deser = get_line_mean(line, warmups)
        elif "proto_serialization" in line:
            proto_ser = get_line_mean(line, warmups)
        if "ser1de_deserialization" in line:
            ser1de_deser = get_line_mean(line, warmups)
        elif "ser1de_serialization" in line:
            ser1de_ser = get_line_mean(line, warmups)
        elif "compress_out" in line:
            compress_out = get_line_num(line)
        elif "gather_out" in line:
            gather_out = get_line_num(line)

    return proto_ser, proto_deser, ser1de_ser, ser1de_deser, gather_out, compress_out

parser = argparse.ArgumentParser()
parser.add_argument('--widths', metavar='N', type=int, nargs='+', default=[3, 4, 5])
parser.add_argument('--depths', metavar='N', type=int, nargs='+', default=[3, 4])
parser.add_argument('--ratios', metavar='N', type=int, nargs='+', default=[4])
parser.add_argument('--sets', metavar='N', type=int, nargs='+', default=[1, 2, 3])

parser.add_argument("-p", "--plot", action="store_true", help="Generate plots")
args = parser.parse_args()

df = []
col_names = ["width", "depth", "#varint", "#string", "proto_serialization", "proto_deserialization", "ser1de_serialization", "ser1de_deserialization", "gather_out(bytes)", "compress_out(bytes)"]
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

                proto_ser, proto_deser, ser1de_ser, ser1de_deser, gather_out, compress_out = process_output(res)
                elem = [width, depth, num_varint, num_string, proto_ser, proto_deser, ser1de_ser, ser1de_deser, gather_out, compress_out]

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