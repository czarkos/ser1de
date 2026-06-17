#!/usr/bin/env python3
import argparse
import os
import subprocess
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.lines import Line2D
from matplotlib.patches import Patch
from matplotlib.ticker import FixedFormatter, FixedLocator

SCRIPT_DIR = Path(__file__).resolve().parent
BENCHMARK_ROOT = SCRIPT_DIR.parent
BENCH_SCRIPT = SCRIPT_DIR / "bench.py"

APPROACHES = [
    ("Protobuf", "proto_serialization", "proto_deserialization", "green"),
    ("Serial SERenaDE (SW)", "serial_ser1de_sw_serialization", "serial_ser1de_sw_deserialization", "orange"),
    ("Serial SERenaDE (HW)", "serial_ser1de_hw_serialization", "serial_ser1de_hw_deserialization", "blue"),
    ("SERenaDE", "ser1de_serialization", "ser1de_deserialization", "red"),
]

TITLE_FONTSIZE = 32
LEGEND_FONTSIZE = 28
SUBPLOT_TITLE_FONTSIZE = 26
AXIS_LABEL_FONTSIZE = 28
TICK_FONTSIZE = 22
END2END_TITLE = "End-to-End Ablation Sensitivity Study (Serialize + Deserialize)"
BREAKDOWN_TITLE = "Ablation Sensitivity Study (Serialize / Deserialize / End-to-End)"
BREAKDOWN_STACKED_TITLE = "Ablation Sensitivity Study (Serialize / Deserialize)"
BREAKDOWN_BY_METRIC_TITLE = "Ablation Sensitivity Study (Serialize / Deserialize / End-to-End)"

METRIC_STYLES = [
    ("Serialize", "ser", ""),
    ("Deserialize", "deser", "///"),
    ("End-to-End", "e2e", "xx"),
]

SER_DESER_METRIC_STYLES = METRIC_STYLES[:2]

COL_NAMES = [
    "width",
    "depth",
    "#varint",
    "#string",
    "proto_serialization",
    "proto_deserialization",
    "ser1de_serialization",
    "ser1de_deserialization",
    "serial_ser1de_hw_serialization",
    "serial_ser1de_hw_deserialization",
    "serial_ser1de_sw_serialization",
    "serial_ser1de_sw_deserialization",
    "gather_out(bytes)",
    "compress_out(bytes)",
]


def process_output(res, warmups=1):
    def get_line_mean(line, warmups_count):
        nums = [int(x) for x in line.strip().split(", ")[1:]]
        nums = nums[warmups_count:]
        if len(nums) == 0:
            return -1
        return round(sum(nums) / len(nums), 2)

    def get_line_num(line):
        return int(line.strip().split(", ")[-1])

    parsed = {key: None for _, ser_key, deser_key, _ in APPROACHES for key in (ser_key, deser_key)}
    gather_out, compress_out = None, None

    for line in res.split("\n"):
        for _, ser_key, deser_key, _ in APPROACHES:
            if deser_key in line:
                parsed[deser_key] = get_line_mean(line, warmups)
            elif ser_key in line:
                parsed[ser_key] = get_line_mean(line, warmups)
        if "compress_out" in line:
            compress_out = get_line_num(line)
        elif "gather_out" in line:
            gather_out = get_line_num(line)

    values = []
    for _, ser_key, deser_key, _ in APPROACHES:
        values.extend([parsed[ser_key], parsed[deser_key]])

    return values + [gather_out, compress_out]


def _set_message_size_xaxis(ax, tick_positions, tick_labels):
    ax.xaxis.set_major_locator(FixedLocator(tick_positions))
    ax.xaxis.set_major_formatter(FixedFormatter(tick_labels))
    ax.set_xticklabels(tick_labels, fontsize=TICK_FONTSIZE)
    ax.tick_params(
        axis="x",
        which="major",
        bottom=True,
        top=False,
        labelbottom=True,
        length=8,
        width=1.2,
    )


def plot_subplot_grid(df, value_columns, labels, colors, ylabel, title, output_path):
    plot_data = df.copy()
    widths = sorted(plot_data["width"].unique())
    depths = sorted(plot_data["depth"].unique())

    fig, axs = plt.subplots(nrows=len(depths), ncols=len(widths), figsize=(18, 10), squeeze=False)

    nrows = len(depths)
    ncols = len(widths)
    x_tick_info = {}

    for i, depth in enumerate(depths):
        for j, width in enumerate(widths):
            ax = axs[i, j]
            subset = plot_data[(plot_data["width"] == width) & (plot_data["depth"] == depth)]
            if subset.empty:
                ax.set_visible(False)
                continue

            ymax = 0
            for label, column, color in zip(labels, value_columns, colors):
                ymax = max(ymax, subset[column].max())
                ax.plot(
                    subset["gather_out(bytes)"],
                    subset[column],
                    label=label,
                    linewidth=2,
                    marker="o",
                    color=color,
                )

            ax.set_title(f"Width {width}, Depth {depth}", fontsize=SUBPLOT_TITLE_FONTSIZE)
            message_sizes = sorted(subset["gather_out(bytes)"].unique())
            tick_labels = [str(int(size)) for size in message_sizes]
            x_tick_info[(i, j)] = (message_sizes, tick_labels)
            ax.tick_params(axis="y", which="both", left=True, right=False, labelsize=TICK_FONTSIZE)
            if j > 0:
                ax.set_yticklabels([])
            ax.grid(True)

            x_min = subset["gather_out(bytes)"].min()
            x_max = subset["gather_out(bytes)"].max()
            ax.set_xlim(left=0.9 * x_min, right=1.1 * x_max)
            ax.set_ylim(bottom=0, top=1.1 * ymax if ymax > 0 else 1)

    plt.tight_layout(rect=[0.04, 0.05, 1, 0.83])
    for i in range(nrows):
        for j in range(ncols):
            if (i, j) in x_tick_info:
                _set_message_size_xaxis(axs[i, j], *x_tick_info[(i, j)])

    fig.supxlabel("Total Message Size (bytes)", fontsize=AXIS_LABEL_FONTSIZE)
    fig.supylabel(ylabel, fontsize=AXIS_LABEL_FONTSIZE)

    legend_handles = [
        Line2D([0], [0], color=color, marker="o", linewidth=2, label=label)
        for label, color in zip(labels, colors)
    ]
    fig.legend(
        legend_handles,
        labels,
        loc="lower center",
        bbox_to_anchor=(0.5, 0.88),
        ncol=len(labels),
        fontsize=LEGEND_FONTSIZE,
        frameon=True,
    )
    fig.suptitle(title, fontsize=TITLE_FONTSIZE, y=0.87)

    fig.savefig(output_path, dpi=300, bbox_inches="tight")
    plt.close(fig)


def ser_perf_plot(df, name):
    plot_subplot_grid(
        df,
        [ser for _, ser, _, _ in APPROACHES],
        [label for label, _, _, _ in APPROACHES],
        [color for _, _, _, color in APPROACHES],
        "Time Spent (ns)",
        "Serialization Sensitivity",
        name,
    )


def deser_perf_plot(df, name):
    plot_subplot_grid(
        df,
        [deser for _, _, deser, _ in APPROACHES],
        [label for label, _, _, _ in APPROACHES],
        [color for _, _, _, color in APPROACHES],
        "Time Spent (ns)",
        "Deserialization Sensitivity",
        name,
    )


def end2end_plot(df, name):
    plot_df = df.copy()
    end2end_columns = []
    end2end_labels = []
    end2end_colors = []

    for label, ser_key, deser_key, color in APPROACHES:
        end2end_key = label.lower().replace(" ", "_").replace("(", "").replace(")", "")
        plot_df[end2end_key] = plot_df[ser_key] + plot_df[deser_key]
        end2end_columns.append(end2end_key)
        end2end_labels.append(label)
        end2end_colors.append(color)

    plot_subplot_grid(
        plot_df,
        end2end_columns,
        end2end_labels,
        end2end_colors,
        "Time Spent (ns)",
        END2END_TITLE,
        name,
    )


def _add_breakdown_header(fig, axs, nrows, ncols, x_tick_info, title, approach_labels, approach_colors, metric_handles, metric_labels):
    plt.tight_layout(rect=[0.04, 0.05, 1, 0.78])
    for i in range(nrows):
        for j in range(ncols):
            if (i, j) in x_tick_info:
                _set_message_size_xaxis(axs[i, j], *x_tick_info[(i, j)])

    fig.supxlabel("Total Message Size (bytes)", fontsize=AXIS_LABEL_FONTSIZE)
    fig.supylabel("Time Spent (ns)", fontsize=AXIS_LABEL_FONTSIZE)

    metric_legend = fig.legend(
        metric_handles,
        metric_labels,
        loc="lower center",
        bbox_to_anchor=(0.5, 0.93),
        ncol=len(metric_labels),
        fontsize=LEGEND_FONTSIZE,
        frameon=True,
    )
    fig.add_artist(metric_legend)

    approach_handles = [
        Patch(facecolor=color, edgecolor="black", label=label)
        for label, color in zip(approach_labels, approach_colors)
    ]
    fig.legend(
        approach_handles,
        approach_labels,
        loc="lower center",
        bbox_to_anchor=(0.5, 0.845),
        ncol=len(approach_labels),
        fontsize=LEGEND_FONTSIZE,
        frameon=True,
    )
    fig.suptitle(title, fontsize=TITLE_FONTSIZE, y=0.815)


def _symmetric_xlim(content_left, content_right, pad_fraction=0.25):
    span = content_right - content_left
    pad = span * pad_fraction if span > 0 else 0.1
    center = (content_left + content_right) / 2
    half = span / 2 + pad
    return center - half, center + half


def _bar_edges(x, bar_width):
    half = bar_width / 2
    return x - half, x + half


def breakdown_bar_plot(df, name):
    plot_data = df.copy()
    widths = sorted(plot_data["width"].unique())
    depths = sorted(plot_data["depth"].unique())

    fig, axs = plt.subplots(nrows=len(depths), ncols=len(widths), figsize=(18, 10), squeeze=False)
    nrows = len(depths)
    ncols = len(widths)
    n_approaches = len(APPROACHES)
    n_metrics = len(METRIC_STYLES)
    bar_width = 0.07
    approach_group_width = n_metrics * bar_width
    size_group_width = n_approaches * approach_group_width + 0.35
    x_tick_info = {}

    for i, depth in enumerate(depths):
        for j, width in enumerate(widths):
            ax = axs[i, j]
            subset = plot_data[(plot_data["width"] == width) & (plot_data["depth"] == depth)].sort_values(
                "gather_out(bytes)"
            )
            if subset.empty:
                ax.set_visible(False)
                continue

            size_tick_positions = []
            size_tick_labels = []
            content_left = float("inf")
            content_right = float("-inf")

            for si, (_, row) in enumerate(subset.iterrows()):
                group_center = si * size_group_width
                size_tick_positions.append(group_center + (n_approaches * approach_group_width - bar_width) / 2)
                size_tick_labels.append(f"{int(row['gather_out(bytes)'])}")

                for ai, (label, ser_key, deser_key, color) in enumerate(APPROACHES):
                    metric_values = [
                        row[ser_key],
                        row[deser_key],
                        row[ser_key] + row[deser_key],
                    ]
                    group_start = group_center + ai * approach_group_width
                    for mi, (_, _, hatch) in enumerate(METRIC_STYLES):
                        x = group_start + mi * bar_width
                        left, right = _bar_edges(x, bar_width)
                        content_left = min(content_left, left)
                        content_right = max(content_right, right)
                        ax.bar(
                            x,
                            metric_values[mi],
                            width=bar_width,
                            color=color,
                            hatch=hatch,
                            edgecolor="black",
                            linewidth=0.6,
                        )

            ax.set_title(f"Width {width}, Depth {depth}", fontsize=SUBPLOT_TITLE_FONTSIZE)
            x_tick_info[(i, j)] = (list(size_tick_positions), size_tick_labels)
            ax.set_xlim(*_symmetric_xlim(content_left, content_right))
            ax.tick_params(axis="y", which="both", left=True, right=False, labelsize=TICK_FONTSIZE)
            if j > 0:
                ax.set_yticklabels([])
            ax.grid(True, axis="y", linestyle="--", alpha=0.7)

    metric_handles = [
        Patch(facecolor="white", edgecolor="black", hatch=hatch, label=metric_label)
        for metric_label, _, hatch in METRIC_STYLES
    ]
    metric_labels = [metric_label for metric_label, _, _ in METRIC_STYLES]
    _add_breakdown_header(
        fig,
        axs,
        nrows,
        ncols,
        x_tick_info,
        BREAKDOWN_TITLE,
        [label for label, _, _, _ in APPROACHES],
        [color for _, _, _, color in APPROACHES],
        metric_handles,
        metric_labels,
    )

    fig.savefig(name, dpi=300, bbox_inches="tight")
    plt.close(fig)


def breakdown_stacked_bar_plot(df, name):
    plot_data = df.copy()
    widths = sorted(plot_data["width"].unique())
    depths = sorted(plot_data["depth"].unique())

    fig, axs = plt.subplots(nrows=len(depths), ncols=len(widths), figsize=(18, 10), squeeze=False)
    nrows = len(depths)
    ncols = len(widths)
    n_approaches = len(APPROACHES)
    bar_width = 0.07
    approach_group_width = bar_width + 0.02
    size_group_width = n_approaches * approach_group_width + 0.35
    x_tick_info = {}

    for i, depth in enumerate(depths):
        for j, width in enumerate(widths):
            ax = axs[i, j]
            subset = plot_data[(plot_data["width"] == width) & (plot_data["depth"] == depth)].sort_values(
                "gather_out(bytes)"
            )
            if subset.empty:
                ax.set_visible(False)
                continue

            size_tick_positions = []
            size_tick_labels = []
            content_left = float("inf")
            content_right = float("-inf")

            for si, (_, row) in enumerate(subset.iterrows()):
                group_center = si * size_group_width
                size_tick_positions.append(group_center + (n_approaches * approach_group_width - bar_width) / 2)
                size_tick_labels.append(f"{int(row['gather_out(bytes)'])}")

                for ai, (_, ser_key, deser_key, color) in enumerate(APPROACHES):
                    ser_value = row[ser_key]
                    deser_value = row[deser_key]
                    x = group_center + ai * approach_group_width
                    left, right = _bar_edges(x, bar_width)
                    content_left = min(content_left, left)
                    content_right = max(content_right, right)
                    ax.bar(
                        x,
                        ser_value,
                        width=bar_width,
                        color=color,
                        hatch="",
                        edgecolor="black",
                        linewidth=0.6,
                    )
                    ax.bar(
                        x,
                        deser_value,
                        width=bar_width,
                        bottom=ser_value,
                        color=color,
                        hatch="///",
                        edgecolor="black",
                        linewidth=0.6,
                    )

            ax.set_title(f"Width {width}, Depth {depth}", fontsize=SUBPLOT_TITLE_FONTSIZE)
            x_tick_info[(i, j)] = (list(size_tick_positions), size_tick_labels)
            ax.set_xlim(*_symmetric_xlim(content_left, content_right))
            ax.tick_params(axis="y", which="both", left=True, right=False, labelsize=TICK_FONTSIZE)
            if j > 0:
                ax.set_yticklabels([])
            ax.grid(True, axis="y", linestyle="--", alpha=0.7)

    metric_handles = [
        Patch(facecolor="white", edgecolor="black", hatch=hatch, label=metric_label)
        for metric_label, _, hatch in SER_DESER_METRIC_STYLES
    ]
    metric_labels = [metric_label for metric_label, _, _ in SER_DESER_METRIC_STYLES]
    _add_breakdown_header(
        fig,
        axs,
        nrows,
        ncols,
        x_tick_info,
        BREAKDOWN_STACKED_TITLE,
        [label for label, _, _, _ in APPROACHES],
        [color for _, _, _, color in APPROACHES],
        metric_handles,
        metric_labels,
    )

    fig.savefig(name, dpi=300, bbox_inches="tight")
    plt.close(fig)


def breakdown_by_metric_bar_plot(df, name):
    plot_data = df.copy()
    widths = sorted(plot_data["width"].unique())
    depths = sorted(plot_data["depth"].unique())

    fig, axs = plt.subplots(nrows=len(depths), ncols=len(widths), figsize=(18, 10), squeeze=False)
    nrows = len(depths)
    ncols = len(widths)
    n_approaches = len(APPROACHES)
    n_metrics = len(METRIC_STYLES)
    bar_width = 0.07
    metric_group_width = n_approaches * bar_width + 0.08
    size_group_width = n_metrics * metric_group_width + 0.35
    x_tick_info = {}

    for i, depth in enumerate(depths):
        for j, width in enumerate(widths):
            ax = axs[i, j]
            subset = plot_data[(plot_data["width"] == width) & (plot_data["depth"] == depth)].sort_values(
                "gather_out(bytes)"
            )
            if subset.empty:
                ax.set_visible(False)
                continue

            size_tick_positions = []
            size_tick_labels = []
            content_left = float("inf")
            content_right = float("-inf")

            for si, (_, row) in enumerate(subset.iterrows()):
                group_center = si * size_group_width
                size_tick_positions.append(group_center + (n_metrics * metric_group_width - bar_width) / 2)
                size_tick_labels.append(f"{int(row['gather_out(bytes)'])}")

                for mi, (_, _, hatch) in enumerate(METRIC_STYLES):
                    metric_group_start = group_center + mi * metric_group_width
                    for ai, (_, ser_key, deser_key, color) in enumerate(APPROACHES):
                        metric_values = [
                            row[ser_key],
                            row[deser_key],
                            row[ser_key] + row[deser_key],
                        ]
                        x = metric_group_start + ai * bar_width
                        left, right = _bar_edges(x, bar_width)
                        content_left = min(content_left, left)
                        content_right = max(content_right, right)
                        ax.bar(
                            x,
                            metric_values[mi],
                            width=bar_width,
                            color=color,
                            hatch=hatch,
                            edgecolor="black",
                            linewidth=0.6,
                        )

            ax.set_title(f"Width {width}, Depth {depth}", fontsize=SUBPLOT_TITLE_FONTSIZE)
            x_tick_info[(i, j)] = (list(size_tick_positions), size_tick_labels)
            ax.set_xlim(*_symmetric_xlim(content_left, content_right))
            ax.tick_params(axis="y", which="both", left=True, right=False, labelsize=TICK_FONTSIZE)
            if j > 0:
                ax.set_yticklabels([])
            ax.grid(True, axis="y", linestyle="--", alpha=0.7)

    metric_handles = [
        Patch(facecolor="white", edgecolor="black", hatch=hatch, label=metric_label)
        for metric_label, _, hatch in METRIC_STYLES
    ]
    metric_labels = [metric_label for metric_label, _, _ in METRIC_STYLES]
    _add_breakdown_header(
        fig,
        axs,
        nrows,
        ncols,
        x_tick_info,
        BREAKDOWN_BY_METRIC_TITLE,
        [label for label, _, _, _ in APPROACHES],
        [color for _, _, _, color in APPROACHES],
        metric_handles,
        metric_labels,
    )

    fig.savefig(name, dpi=300, bbox_inches="tight")
    plt.close(fig)


def generate_plots(data_frame, plt_dir):
    os.makedirs(plt_dir, exist_ok=True)
    ser_perf_plot(data_frame, os.path.join(plt_dir, "perf_serialization.pdf"))
    deser_perf_plot(data_frame, os.path.join(plt_dir, "perf_deserialization.pdf"))
    end2end_plot(data_frame, os.path.join(plt_dir, "end2end_sensitivity.pdf"))
    breakdown_bar_plot(data_frame, os.path.join(plt_dir, "latency_breakdown.pdf"))
    breakdown_stacked_bar_plot(data_frame, os.path.join(plt_dir, "latency_breakdown_stacked.pdf"))
    breakdown_by_metric_bar_plot(data_frame, os.path.join(plt_dir, "latency_breakdown_by_metric.pdf"))
    print(f"Saved plots under {plt_dir}/")


def main():
    parser = argparse.ArgumentParser(description="Run ablation sensitivity sweep and plot 4-way comparisons.")
    parser.add_argument("--widths", metavar="N", type=int, nargs="+", default=[3, 4, 5, 6])
    parser.add_argument("--depths", metavar="N", type=int, nargs="+", default=[3, 4, 5])
    parser.add_argument("--ratios", metavar="N", type=int, nargs="+", default=[4])
    parser.add_argument("--sets", metavar="N", type=int, nargs="+", default=[2, 3])
    parser.add_argument("-p", "--plot", action="store_true", help="Generate plots")
    parser.add_argument(
        "--plot-only",
        action="store_true",
        help="Generate plots from existing CSV without running the benchmark.",
    )
    parser.add_argument(
        "--csv",
        type=str,
        default=None,
        help="Path to sensitivity.csv (required with --plot-only unless --output-dir is set).",
    )
    parser.add_argument(
        "--output-dir",
        type=str,
        default=None,
        help="Directory for plot output (default: same directory as --csv, or auto-named after sweep).",
    )
    args = parser.parse_args()

    if args.plot_only:
        if args.csv is None:
            parser.error("--plot-only requires --csv pointing to an existing sensitivity.csv")
        csv_path = Path(args.csv)
        if not csv_path.is_absolute():
            csv_path = BENCHMARK_ROOT / csv_path
        if not csv_path.exists():
            parser.error(f"CSV not found: {csv_path}")
        plt_dir = args.output_dir or str(csv_path.parent)
        data_frame = pd.read_csv(csv_path)
        print(data_frame.to_string())
        generate_plots(data_frame, plt_dir)
        return

    rows = []
    for width in args.widths:
        for depth in args.depths:
            for ratio in args.ratios:
                for fset in args.sets:
                    num_varint = ratio * fset
                    num_string = fset

                    cmd = (
                        f"python3 {BENCH_SCRIPT} --proto --build --run --string "
                        f"-w {width} -d {depth} -nv {num_varint} -ns {num_string}"
                    )
                    print(cmd)
                    res = subprocess.run(cmd, shell=True, text=True, capture_output=True, cwd=BENCHMARK_ROOT).stdout
                    print(res)

                    values = process_output(res)
                    rows.append([width, depth, num_varint, num_string] + values)

    data_frame = pd.DataFrame(rows, columns=COL_NAMES)
    print(data_frame.to_string())

    if args.plot:
        plt_dir = args.output_dir or (
            "plots"
            + "_w_" + "_".join(map(str, args.widths))
            + "_d_" + "_".join(map(str, args.depths))
            + "_r_" + "_".join(map(str, args.ratios))
            + "_s_" + "_".join(map(str, args.sets))
        )
        os.makedirs(plt_dir, exist_ok=True)
        data_frame.to_csv(os.path.join(plt_dir, "sensitivity.csv"), index=False)
        generate_plots(data_frame, plt_dir)


if __name__ == "__main__":
    main()
