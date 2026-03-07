#include<iostream>
#include<chrono>
#include"define.hpp"
#include<papi.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <cstring>
#include <stdint.h>

using namespace std;

#include <iostream>
#include <chrono>

using namespace std;

struct timer {
    chrono::high_resolution_clock::time_point begin, end;

    timer() {
        start();
    }

    void start() {
        begin = chrono::high_resolution_clock::now();
    }

    void stop() {
        end = chrono::high_resolution_clock::now();
    }

    int elapsed() {
        return chrono::duration_cast<chrono::milliseconds>(end - begin).count();
    }
};

long perf_event_open(perf_event_attr* hw_event, pid_t pid,
                     int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

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

    ~PerfCounter() { close(fd); }
};

// Helper para montar config de cache
constexpr uint64_t cache_config(uint32_t cache, uint32_t op, uint32_t result) {
    return cache | (op << 8) | (result << 16);
}

struct CacheStats {
    // L1 data cache
    PerfCounter l1_load_miss{PERF_TYPE_HW_CACHE,
        cache_config(PERF_COUNT_HW_CACHE_L1D,
                     PERF_COUNT_HW_CACHE_OP_READ,
                     PERF_COUNT_HW_CACHE_RESULT_MISS)};

    // L2 — acessos que passaram do L1
    PerfCounter l2_load_miss{PERF_TYPE_HARDWARE,
        PERF_COUNT_HW_CACHE_REFERENCES};

    // L3 / LLC
    PerfCounter llc_load_miss{PERF_TYPE_HW_CACHE,
        cache_config(PERF_COUNT_HW_CACHE_LL,
                     PERF_COUNT_HW_CACHE_OP_READ,
                     PERF_COUNT_HW_CACHE_RESULT_MISS)};

    PerfCounter cycles{PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES};

    uint64_t r_l1, r_l2, r_llc, r_cycles;

    void start() {
        l1_load_miss.start();
        l2_load_miss.start();
        llc_load_miss.start();
        cycles.start();
    }

    void stop() {
        r_cycles = cycles.stop();
        r_llc    = llc_load_miss.stop();
        r_l2     = l2_load_miss.stop();
        r_l1     = l1_load_miss.stop();
    }

    void print(const char* label) {
        printf("=== %s ===\n", label);
        printf("  L1  miss : %10lu\n", r_l1);
        printf("  L2  refs : %10lu\n", r_l2);
        printf("  LLC miss : %10lu\n", r_llc);
        printf("  Cycles   : %10lu\n\n", r_cycles);
    }
};