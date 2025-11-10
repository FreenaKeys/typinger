#pragma once
#include <cstdint>

// High resolution timer utilities for Windows (QueryPerformanceCounter based)
// Unit: microseconds
namespace WinTimer {
    // Initialize internal frequency cache (idempotent)
    void init();

    // Current timestamp in microseconds since an arbitrary epoch
    std::uint64_t now_us();

    // Convenience: seconds as double
    double now_s();

    // Simple stopwatch helper
    struct Stopwatch {
        std::uint64_t t0_us;
        Stopwatch();
        void reset();
        std::uint64_t elapsed_us() const;
        double elapsed_s() const;
    };
}
