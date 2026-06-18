#include <chrono>
#include <string>
#include <iostream>

struct ScopeTimer {
    std::string name;
    std::chrono::time_point<std::chrono::high_resolution_clock> startPoint;

    ScopeTimer(const std::string& timerName)
        : name(timerName), startPoint(std::chrono::high_resolution_clock::now()) {
        std::cout << "[TIMER] " << name << " started!\n";
    }

    ~ScopeTimer() {
        auto endPoint = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endPoint - startPoint).count();

        std::cout << "[TIMER] " << name << " took: "
            << duration << " ms (" << duration / 1000.0f << " sec)\n";
    }
};