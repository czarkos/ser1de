import subprocess
import re
import pandas as pd
import matplotlib.pyplot as plt
import os

# Configuration
binary = "./build/test"
num_requests = 10000  # requests per thread
#thread_counts = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
thread_counts = [1, 2, 4, 8, 16, 32]
csv_file = "thread_scalability_results.csv"

# Regex patterns
proto_ser_pattern = re.compile(r"Protobuf Serialize: (\d+) ns")
proto_deser_pattern = re.compile(r"Protobuf Deserialize: (\d+) ns")
ser1de_ser_pattern = re.compile(r"Ser1de Serialize: (\d+) ns")
ser1de_deser_pattern = re.compile(r"Ser1de Deserialize: (\d+) ns")

results = []

num_cores = os.cpu_count()

for threads in thread_counts:
    # Determine core range for this run
    if threads <= num_cores:
        core_range = f"0-{threads-1}"
    else:
        core_range = f"0-{num_cores-1}"
    cmd = ["sudo", "taskset", "-a", "-c", core_range, binary, str(threads), str(num_requests)]
    result = subprocess.run(cmd, capture_output=True, text=True)
    output = result.stdout + result.stderr

    # Extract timings (sum across all threads)
    proto_ser = sum(map(int, proto_ser_pattern.findall(output)))
    proto_deser = sum(map(int, proto_deser_pattern.findall(output)))
    ser1de_ser = sum(map(int, ser1de_ser_pattern.findall(output)))
    ser1de_deser = sum(map(int, ser1de_deser_pattern.findall(output)))

    total_ops = threads * num_requests

    # Throughput: ops per second = total_ops / (total_time_in_seconds)
    def throughput(ns):
        return total_ops / (ns / 1e9) if ns > 0 else 0

    results.append({
        "threads": threads,
        "proto_ser_throughput": throughput(proto_ser),
        "proto_deser_throughput": throughput(proto_deser),
        "ser1de_ser_throughput": throughput(ser1de_ser),
        "ser1de_deser_throughput": throughput(ser1de_deser),
    })

# Save to CSV
df = pd.DataFrame(results)
df.to_csv(csv_file, index=False)
print(f"Results saved to {csv_file}")

# Plot
plt.figure(figsize=(10, 6))
plt.plot(df["threads"], df["proto_ser_throughput"], marker='o', label="Protobuf Serialize")
plt.plot(df["threads"], df["proto_deser_throughput"], marker='o', label="Protobuf Deserialize")
plt.plot(df["threads"], df["ser1de_ser_throughput"], marker='o', label="Ser1de Serialize")
plt.plot(df["threads"], df["ser1de_deser_throughput"], marker='o', label="Ser1de Deserialize")
plt.xlabel("Number of Threads")
plt.ylabel("Throughput (ops/sec)")
plt.title("Aggregated Throughput vs Number of Threads")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("throughput_vs_threads.png")
plt.show()