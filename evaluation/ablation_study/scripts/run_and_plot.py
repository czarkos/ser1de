#!/usr/bin/env python3
"""Run the ablation benchmark and plot latency comparisons for all 4 approaches."""

import argparse
import re
import subprocess
import sys
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

ROOT = Path(__file__).resolve().parents[1]
BENCHMARK = ROOT / "build" / "test"
DEFAULT_RESULTS = ROOT / "results.txt"
DEFAULT_PLOTS = ROOT / "plots"

APPROACHES = {
    "Protobuf": {
        "serialize": "Proto Serialize",
        "deserialize": "Proto Deserialize",
    },
    "Ser1de": {
        "serialize": "Ser1de Serialize",
        "deserialize": "Ser1de Deserialize",
    },
    "Serial Ser1de (HW)": {
        "serialize": "Serial_Ser1de-HW Serialize",
        "deserialize": "Serial_Ser1de-HW Deserialize",
    },
    "Serial Ser1de (SW)": {
        "serialize": "Serial_Ser1de-SW Serialize",
        "deserialize": "Serial_Ser1de-SW Deserialize",
    },
}

CSV_LINE = re.compile(
    r"^(Proto Serialize|Proto Deserialize|"
    r"Ser1de Serialize|Ser1de Deserialize|"
    r"Serial_Ser1de-HW Serialize|Serial_Ser1de-HW Deserialize|"
    r"Serial_Ser1de-SW Serialize|Serial_Ser1de-SW Deserialize),"
    r"(\d+),(\d+),(\d+),(\d+)$"
)

COLORS = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728"]
MARKERS = ["o", "s", "^", "D"]


def run_benchmark(results_path: Path) -> None:
    if not BENCHMARK.exists():
        raise FileNotFoundError(
            f"Benchmark binary not found at {BENCHMARK}. "
            "Build it first: cd build && cmake .. && make"
        )

    print(f"Running benchmark -> {results_path}")
    with open(results_path, "w") as f:
        subprocess.run(
            [str(BENCHMARK)],
            stdout=f,
            stderr=subprocess.STDOUT,
            check=True,
            cwd=str(ROOT),
        )
    print("Benchmark finished.")


def read_results(path: Path) -> pd.DataFrame:
    rows = []
    with open(path) as f:
        lines = f.readlines()

    for line in lines:
        match = CSV_LINE.match(line.strip())
        if match:
            rows.append(
                {
                    "type": match.group(1),
                    "rps": int(match.group(2)),
                    "p50": int(match.group(3)),
                    "p95": int(match.group(4)),
                    "p99": int(match.group(5)),
                }
            )

    if rows:
        df = pd.DataFrame(rows)
        return df.sort_values(["type", "rps"])

    return _read_results_legacy(lines, path)


def _read_results_legacy(lines, path: Path) -> pd.DataFrame:
    """Parse human-readable benchmark output (pre-CSV format)."""
    rows = []
    current_type = None
    current_rps = None

    type_map = {
        "Proto Serialize": "Proto Serialize",
        "Proto Deserialize": "Proto Deserialize",
        "Ser1de Serialize": "Ser1de Serialize",
        "Ser1de Deserialize": "Ser1de Deserialize",
        "Serial_Ser1de-HW Serialize": "Serial_Ser1de-HW Serialize",
        "Serial_Ser1de-HW Deserialize": "Serial_Ser1de-HW Deserialize",
        "Serial_Ser1de-SW Serialize": "Serial_Ser1de-SW Serialize",
        "Serial_Ser1de-SW Deserialize": "Serial_Ser1de-SW Deserialize",
    }

    for line in lines:
        stripped = line.strip()
        if "Request per Second:" in stripped:
            current_rps = int(stripped.split(":")[1].split()[0])
        elif stripped.startswith("Tail Latencies (microseconds) - "):
            label = stripped.split(" - ", 1)[1].rstrip(":")
            current_type = type_map.get(label)
        elif stripped and not stripped.startswith("-") and stripped != "p50":
            parts = [int(x) for x in stripped.split() if x.isdigit()]
            if len(parts) == 3 and current_type is not None and current_rps is not None:
                p50, p95, p99 = parts
                rows.append(
                    {
                        "type": current_type,
                        "rps": current_rps,
                        "p50": p50,
                        "p95": p95,
                        "p99": p99,
                    }
                )

    if not rows:
        raise ValueError(
            f"No result rows found in {path}. "
            "Expected CSV lines or legacy tail-latency blocks."
        )

    df = pd.DataFrame(rows)
    return df.sort_values(["type", "rps"])


def plot_operation(df: pd.DataFrame, operation: str, output_path: Path) -> None:
    fig, ax = plt.subplots(figsize=(12, 7))

    for i, (label, types) in enumerate(APPROACHES.items()):
        type_name = types[operation]
        group = df[df["type"] == type_name].sort_values("rps")
        if group.empty:
            print(f"Warning: no data for {type_name}", file=sys.stderr)
            continue

        ax.plot(
            group["rps"],
            group["p50"],
            color=COLORS[i],
            marker=MARKERS[i],
            linewidth=2,
            markersize=8,
            label=label,
        )
        ax.plot(
            group["rps"],
            group["p95"],
            color=COLORS[i],
            marker=MARKERS[i],
            linestyle=":",
            linewidth=2,
            markersize=8,
        )

    title = "Serialization" if operation == "serialize" else "Deserialization"
    ax.set_xlabel("Requests per Second", fontsize=16)
    ax.set_ylabel("Latency (microseconds)", fontsize=16)
    ax.set_title(f"{title}: p50 (solid) and p95 (dotted)", fontsize=18)
    ax.grid(True, linestyle="--", alpha=0.7)
    ax.tick_params(axis="both", labelsize=14)
    ax.legend(fontsize=13, loc="best")

    fig.tight_layout()
    fig.savefig(output_path, bbox_inches="tight")
    plt.close(fig)
    print(f"Saved {output_path}")


def plot_p99(df: pd.DataFrame, operation: str, output_path: Path) -> None:
    fig, ax = plt.subplots(figsize=(12, 7))

    for i, (label, types) in enumerate(APPROACHES.items()):
        type_name = types[operation]
        group = df[df["type"] == type_name].sort_values("rps")
        if group.empty:
            continue

        ax.plot(
            group["rps"],
            group["p99"],
            color=COLORS[i],
            marker=MARKERS[i],
            linewidth=2,
            markersize=8,
            label=label,
        )

    title = "Serialization" if operation == "serialize" else "Deserialization"
    ax.set_xlabel("Requests per Second", fontsize=16)
    ax.set_ylabel("P99 Latency (microseconds)", fontsize=16)
    ax.set_title(f"{title}: p99", fontsize=18)
    ax.grid(True, linestyle="--", alpha=0.7)
    ax.tick_params(axis="both", labelsize=14)
    ax.legend(fontsize=13, loc="best")

    fig.tight_layout()
    fig.savefig(output_path, bbox_inches="tight")
    plt.close(fig)
    print(f"Saved {output_path}")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Run ablation benchmark and plot 4-way latency comparisons."
    )
    parser.add_argument(
        "--skip-run",
        action="store_true",
        help="Skip running the benchmark; parse existing results and plot only.",
    )
    parser.add_argument(
        "--results",
        type=Path,
        default=DEFAULT_RESULTS,
        help=f"Path to benchmark output (default: {DEFAULT_RESULTS})",
    )
    parser.add_argument(
        "--plots-dir",
        type=Path,
        default=DEFAULT_PLOTS,
        help=f"Directory for output plots (default: {DEFAULT_PLOTS})",
    )
    args = parser.parse_args()

    args.plots_dir.mkdir(parents=True, exist_ok=True)

    if not args.skip_run:
        run_benchmark(args.results)
    elif not args.results.exists():
        raise FileNotFoundError(f"Results file not found: {args.results}")

    df = read_results(args.results)
    print(f"Parsed {len(df)} result rows across {df['type'].nunique()} benchmark types.")

    plot_operation(df, "serialize", args.plots_dir / "serialization_comparison.pdf")
    plot_operation(df, "deserialize", args.plots_dir / "deserialization_comparison.pdf")
    plot_p99(df, "serialize", args.plots_dir / "serialization_p99.pdf")
    plot_p99(df, "deserialize", args.plots_dir / "deserialization_p99.pdf")


if __name__ == "__main__":
    main()
