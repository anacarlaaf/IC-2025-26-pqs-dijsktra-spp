#include<bits/stdc++.h>
#include"define.hpp"
#include<papi.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <cstring>
#include <stdint.h>
#include <sys/resource.h>

using namespace std;

#include <iostream>
#include <chrono>

using namespace std;

#include <chrono>
#include <ctime>

struct timer {
    // tempo real (wall-clock)
    std::chrono::high_resolution_clock::time_point begin, end;

    // tempo de CPU
    std::clock_t cpu_begin, cpu_end;

    timer() {
        start();
    }

    void start() {
        begin = std::chrono::high_resolution_clock::now();
        cpu_begin = std::clock();
    }

    void stop() {
        end = std::chrono::high_resolution_clock::now();
        cpu_end = std::clock();
    }

    // tempo real em ms
    int elapsed_wall_ms() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            end - begin
        ).count();
    }

    // tempo de CPU em ms
    int elapsed_cpu_ms() const {
        return int(1000.0 * (cpu_end - cpu_begin) / CLOCKS_PER_SEC);
    }
};

// ─────────────────────────────────────────────────────────────
// perf_event_open wrapper
// ─────────────────────────────────────────────────────────────
static long perf_event_open(perf_event_attr* attr, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, attr, pid, cpu, group_fd, flags);
}

// ─────────────────────────────────────────────────────────────
// Contador genérico
// ─────────────────────────────────────────────────────────────
struct PerfCounter {
    int fd;

    PerfCounter(uint32_t type, uint64_t config) {
        perf_event_attr attr{};
        attr.type           = type;
        attr.size           = sizeof(attr);
        attr.config         = config;
        attr.disabled       = 1;
        attr.exclude_kernel = 1;
        attr.exclude_hv     = 1;

        fd = perf_event_open(&attr, 0, -1, -1, 0);
    }

    void start() {
        ioctl(fd, PERF_EVENT_IOC_RESET,  0);
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    }

    uint64_t stop() {
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        uint64_t count = 0;
        read(fd, &count, sizeof(count));
        return count;
    }

    ~PerfCounter() {
        if (fd >= 0) close(fd);
    }
};

// ─────────────────────────────────────────────────────────────
// Helper para configuração de cache/TLB
// ─────────────────────────────────────────────────────────────
constexpr uint64_t cache_config(uint32_t cache,
                                uint32_t op,
                                uint32_t result) {
    return cache | (op << 8) | (result << 16);
}

// ─────────────────────────────────────────────────────────────
// Conjunto de métricas
// ─────────────────────────────────────────────────────────────
struct CacheStats {

    // ─── Cache ─────────────────────────
    PerfCounter l1_load_miss {
        PERF_TYPE_HW_CACHE,
        cache_config(PERF_COUNT_HW_CACHE_L1D,
                     PERF_COUNT_HW_CACHE_OP_READ,
                     PERF_COUNT_HW_CACHE_RESULT_MISS) };

    PerfCounter l2_refs {
        PERF_TYPE_HARDWARE,
        PERF_COUNT_HW_CACHE_REFERENCES };

    PerfCounter llc_load_miss {
        PERF_TYPE_HW_CACHE,
        cache_config(PERF_COUNT_HW_CACHE_LL,
                     PERF_COUNT_HW_CACHE_OP_READ,
                     PERF_COUNT_HW_CACHE_RESULT_MISS) };

    // ─── TLB (NOVO) ───────────────────
    PerfCounter dtlb_load_miss {
        PERF_TYPE_HW_CACHE,
        cache_config(PERF_COUNT_HW_CACHE_DTLB,
                     PERF_COUNT_HW_CACHE_OP_READ,
                     PERF_COUNT_HW_CACHE_RESULT_MISS) };

    // ─── CPU ───────────────────────────
    PerfCounter cycles {
        PERF_TYPE_HARDWARE,
        PERF_COUNT_HW_CPU_CYCLES };

    PerfCounter instructions {
        PERF_TYPE_HARDWARE,
        PERF_COUNT_HW_INSTRUCTIONS };

    // ─── Branch ────────────────────────
    PerfCounter branch_misses {
        PERF_TYPE_HARDWARE,
        PERF_COUNT_HW_BRANCH_MISSES };

    PerfCounter branch_instr {
        PERF_TYPE_HARDWARE,
        PERF_COUNT_HW_BRANCH_INSTRUCTIONS };

    // ─── Memória / OS ──────────────────
    PerfCounter page_faults {
        PERF_TYPE_SOFTWARE,
        PERF_COUNT_SW_PAGE_FAULTS };

    // ─── Resultados ────────────────────
    uint64_t r_l1, r_l2, r_llc;
    uint64_t r_dtlb;
    uint64_t r_cycles, r_instructions;
    uint64_t r_branch_miss, r_branch_instr;
    uint64_t r_page_faults;
    long     r_rss_kb;

    // ───────────────────────────────────
    void start() {
        l1_load_miss.start();
        l2_refs.start();
        llc_load_miss.start();
        dtlb_load_miss.start();
        cycles.start();
        instructions.start();
        branch_misses.start();
        branch_instr.start();
        page_faults.start();
    }

    // ───────────────────────────────────
    void stop() {
        r_page_faults  = page_faults.stop();
        r_branch_instr = branch_instr.stop();
        r_branch_miss  = branch_misses.stop();
        r_instructions = instructions.stop();
        r_cycles       = cycles.stop();
        r_dtlb         = dtlb_load_miss.stop();
        r_llc          = llc_load_miss.stop();
        r_l2           = l2_refs.stop();
        r_l1           = l1_load_miss.stop();

    }

    // ───────────────────────────────────
    void print(const char* label) {

        double ipc = r_cycles ?
            (double)r_instructions / r_cycles : 0.0;

        double br_miss_rate = r_branch_instr ?
            (double)r_branch_miss / r_branch_instr : 0.0;

        printf("=== %s ===\n", label);

        printf("\nCPU:\n");
        printf("  Instructions : %10lu\n", r_instructions);
        printf("  Cycles       : %10lu\n", r_cycles);
        printf("  IPC          : %10.3f\n", ipc);

        printf("\nBranch:\n");
        printf("  Branch instr : %10lu\n", r_branch_instr);
        printf("  Branch miss  : %10lu\n", r_branch_miss);
        printf("  Miss rate    : %10.3f\n", br_miss_rate);

        printf("\nCache:\n");
        printf("  L1 miss      : %10lu\n", r_l1);
        printf("  L2 refs      : %10lu\n", r_l2);
        printf("  LLC miss     : %10lu\n", r_llc);

        printf("\nTLB:\n");
        printf("  DTLB miss    : %10lu\n", r_dtlb);

        printf("\nMemory:\n");
        printf("  Page faults  : %10lu\n", r_page_faults);

        printf("\n");
    }
};