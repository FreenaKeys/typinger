#include "timer.h"
#include <windows.h>

namespace WinTimer {
    static LARGE_INTEGER s_freq = {0};
    static bool s_inited = false;

    void init() {
        if (!s_inited) {
            QueryPerformanceFrequency(&s_freq);
            s_inited = true;
        }
    }

    static inline std::uint64_t ticks_to_us(LARGE_INTEGER t) {
        // guard init
        if (!s_inited) init();
        // Convert to microseconds: (ticks / freq) * 1e6
        // Reordered to avoid precision loss
        return static_cast<std::uint64_t>((t.QuadPart * 1000000ULL) / s_freq.QuadPart);
    }

    std::uint64_t now_us() {
        if (!s_inited) init();
        LARGE_INTEGER c;
        QueryPerformanceCounter(&c);
        return ticks_to_us(c);
    }

    double now_s() {
        return static_cast<double>(now_us()) / 1'000'000.0;
    }

    Stopwatch::Stopwatch() {
        init();
        reset();
    }

    void Stopwatch::reset() {
        LARGE_INTEGER c;
        QueryPerformanceCounter(&c);
        t0_us = ticks_to_us(c);
    }

    std::uint64_t Stopwatch::elapsed_us() const {
        LARGE_INTEGER c;
        QueryPerformanceCounter(&c);
        return ticks_to_us(c) - t0_us;
    }

    double Stopwatch::elapsed_s() const {
        return static_cast<double>(elapsed_us()) / 1'000'000.0;
    }
}
