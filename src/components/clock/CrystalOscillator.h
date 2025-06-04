#ifndef CRYSTALOSCILLATOR_H
#define CRYSTALOSCILLATOR_H

#include <chrono>
#include <vector>

#include "../../tools/Logs.h"

enum Phase { Ph1 = 1, Ph2 = 2 };

Phase& operator++(Phase& phase, int) {
    phase = static_cast<Phase>(static_cast<int>(phase) + 1);
    return phase;
}

class CrystalOscillator {
    std::chrono::steady_clock::time_point cycleStartTime;
    // This vector stores the time that a cycle took; the index corresponds to the cycle count.
    std::vector<long long> cycles;
    bool running = false;

   public:
    unsigned long getCycleCount() const { return cycles.size(); }
    std::chrono::steady_clock::time_point getCycleStartTime() const { return cycleStartTime; }

    // Start a cycle
    void start();
    // Reset all Clock values
    void reset();

   private:
    // This methode triggers a clock cycle
    static void tick();
    // This methode is used to check if the emulator should be stopped
    static bool checkForStopSignal();

   public:
    std::string toString() const;
    std::string toStringMD() const;
};

#endif  // CRYSTALOSCILLATOR_H
