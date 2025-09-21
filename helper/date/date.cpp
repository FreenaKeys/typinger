#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include "date.h"
namespace date {
    std::string date() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        return std::ctime(&now_c); // std::string型で返す
}
}