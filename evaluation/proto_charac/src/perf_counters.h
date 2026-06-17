// Standalone in-process hardware performance counters using perf_event_open.
//
// Measures, for a fenced code region:
//   - retired instructions and core cycles (CPU utilization / IPC)
//   - L1 instruction-cache read misses and iTLB read misses (icache impact)
//   - process CPU time and wall time (CPUs utilized)
//
// Counters are opened user-space only (exclude_kernel/exclude_hv) so they work
// under the default perf_event_paranoid=2 without root, and they attribute work
// to the measured region rather than the whole process. Each counter fails soft:
// if a given event is unsupported, its value is reported as "n/a" (-1) instead
// of aborting the run.

#ifndef PROTO_CHARAC_PERF_COUNTERS_H
#define PROTO_CHARAC_PERF_COUNTERS_H

#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>

class PerfCounters {
public:
    PerfCounters() {
        instr_fd_ = open_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);
        cycles_fd_ = open_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);
        l1i_miss_fd_ = open_counter(
            PERF_TYPE_HW_CACHE,
            cache_config(PERF_COUNT_HW_CACHE_L1I, PERF_COUNT_HW_CACHE_OP_READ,
                         PERF_COUNT_HW_CACHE_RESULT_MISS));
        itlb_miss_fd_ = open_counter(
            PERF_TYPE_HW_CACHE,
            cache_config(PERF_COUNT_HW_CACHE_ITLB, PERF_COUNT_HW_CACHE_OP_READ,
                         PERF_COUNT_HW_CACHE_RESULT_MISS));
    }

    ~PerfCounters() {
        close_fd(instr_fd_);
        close_fd(cycles_fd_);
        close_fd(l1i_miss_fd_);
        close_fd(itlb_miss_fd_);
    }

    PerfCounters(const PerfCounters&) = delete;
    PerfCounters& operator=(const PerfCounters&) = delete;

    // True if at least one hardware counter was successfully opened.
    bool hardware_available() const {
        return instr_fd_ != -1 || cycles_fd_ != -1 || l1i_miss_fd_ != -1 ||
               itlb_miss_fd_ != -1;
    }

    void start() {
        reset_enable(instr_fd_);
        reset_enable(cycles_fd_);
        reset_enable(l1i_miss_fd_);
        reset_enable(itlb_miss_fd_);
        wall_start_ = now(CLOCK_MONOTONIC);
        cpu_start_ = now(CLOCK_PROCESS_CPUTIME_ID);
    }

    void stop() {
        cpu_end_ = now(CLOCK_PROCESS_CPUTIME_ID);
        wall_end_ = now(CLOCK_MONOTONIC);
        disable(instr_fd_);
        disable(cycles_fd_);
        disable(l1i_miss_fd_);
        disable(itlb_miss_fd_);
        instructions_ = read_counter(instr_fd_);
        cycles_ = read_counter(cycles_fd_);
        l1i_miss_ = read_counter(l1i_miss_fd_);
        itlb_miss_ = read_counter(itlb_miss_fd_);
    }

    // Raw counts; -1 means the event was unavailable.
    long long instructions() const { return instructions_; }
    long long cycles() const { return cycles_; }
    long long l1i_miss() const { return l1i_miss_; }
    long long itlb_miss() const { return itlb_miss_; }

    double wall_seconds() const { return wall_end_ - wall_start_; }
    double cpu_seconds() const { return cpu_end_ - cpu_start_; }

private:
    static uint64_t cache_config(uint64_t id, uint64_t op, uint64_t result) {
        return id | (op << 8) | (result << 16);
    }

    static int open_counter(uint32_t type, uint64_t config) {
        struct perf_event_attr attr;
        std::memset(&attr, 0, sizeof(attr));
        attr.type = type;
        attr.size = sizeof(attr);
        attr.config = config;
        attr.disabled = 1;
        attr.exclude_kernel = 1;
        attr.exclude_hv = 1;
        // pid=0 (this thread), cpu=-1 (any CPU it runs on), no group, no flags.
        long fd = syscall(SYS_perf_event_open, &attr, 0, -1, -1, 0);
        return static_cast<int>(fd);  // -1 on failure
    }

    static void reset_enable(int fd) {
        if (fd == -1) return;
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    }

    static void disable(int fd) {
        if (fd == -1) return;
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    }

    static long long read_counter(int fd) {
        if (fd == -1) return -1;
        uint64_t value = 0;
        ssize_t n = read(fd, &value, sizeof(value));
        if (n != static_cast<ssize_t>(sizeof(value))) return -1;
        return static_cast<long long>(value);
    }

    static void close_fd(int fd) {
        if (fd != -1) close(fd);
    }

    static double now(clockid_t clk) {
        struct timespec ts;
        clock_gettime(clk, &ts);
        return static_cast<double>(ts.tv_sec) +
               static_cast<double>(ts.tv_nsec) * 1e-9;
    }

    int instr_fd_ = -1;
    int cycles_fd_ = -1;
    int l1i_miss_fd_ = -1;
    int itlb_miss_fd_ = -1;

    long long instructions_ = -1;
    long long cycles_ = -1;
    long long l1i_miss_ = -1;
    long long itlb_miss_ = -1;

    double wall_start_ = 0.0, wall_end_ = 0.0;
    double cpu_start_ = 0.0, cpu_end_ = 0.0;
};

#endif  // PROTO_CHARAC_PERF_COUNTERS_H
