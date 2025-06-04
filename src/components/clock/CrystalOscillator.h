#ifndef CRYSTALOSCILLATOR_H
#define CRYSTALOSCILLATOR_H

#include <chrono>
#include <vector>

#include "../../tools/Logs.h"

class CrystalOscillator {
    std::chrono::steady_clock::time_point cycleStartTime;
    // This vector stores the time that a cycle took; the index corresponds to the cycle count.
    std::vector<long long> cycles;

   public:
    unsigned long getCycleCount() const { return cycles.size(); }
    std::chrono::steady_clock::time_point getCycleStartTime() const { return cycleStartTime; }

    // Start a cycle
    void start();
    // Stop a cycle and save it inside cycles
    void stop();
    // Stop the last cycle and start a new one
    void trigger();
    // Reset all Clock values
    void reset();

    std::string toString() const;
    std::string toStringMD() const;
};

#endif  // CRYSTALOSCILLATOR_H
