#include "countdown_timer.h"
#include "../helper/WinAPI/timer.h"
#include <algorithm>

namespace CountdownTimer {

    Timer::Timer(int durationSeconds) 
        : startTime_us(0), 
          durationLimit_us(static_cast<std::uint64_t>(durationSeconds) * 1000000ULL),
          isRunning(false) {
        WinTimer::init();
    }

    void Timer::start() {
        startTime_us = WinTimer::now_us();
        isRunning = true;
    }

    void Timer::stop() {
        isRunning = false;
    }

    void Timer::reset() {
        startTime_us = 0;
        isRunning = false;
    }

    double Timer::getElapsedSeconds() const {
        if (!isRunning) {
            return 0.0;
        }
        
        std::uint64_t currentTime_us = WinTimer::now_us();
        std::uint64_t elapsed_us = currentTime_us - startTime_us;
        return elapsed_us / 1000000.0;
    }

    double Timer::getRemainingSeconds() const {
        if (!isRunning) {
            return durationLimit_us / 1000000.0;
        }
        
        double elapsed = getElapsedSeconds();
        double limit = durationLimit_us / 1000000.0;
        double remaining = limit - elapsed;
        
        return std::max(0.0, remaining);
    }

    bool Timer::isTimeUp() const {
        if (!isRunning) {
            return false;
        }
        
        return getRemainingSeconds() <= 0.0;
    }

    bool Timer::running() const {
        return isRunning;
    }

    double Timer::getProgress() const {
        if (!isRunning) {
            return 0.0;
        }
        
        double elapsed = getElapsedSeconds();
        double limit = durationLimit_us / 1000000.0;
        double progress = elapsed / limit;
        
        return std::min(1.0, std::max(0.0, progress));
    }

} // namespace CountdownTimer
