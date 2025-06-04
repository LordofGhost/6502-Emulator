#include "CrystalOscillator.h"

#include "../../Main.h"

extern Arguments arguments;

void CrystalOscillator::start() { cycleStartTime = std::chrono::steady_clock::now(); }

void CrystalOscillator::stop() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::nanoseconds>(now - cycleStartTime).count();
    cycles.push_back(elapsed);
    cycleStartTime = std::chrono::steady_clock::now();

    // Create logs at the of the cycle
    if (arguments.logsAll) Logs::log();
}

void CrystalOscillator::trigger() {
    stop();
    start();
}

void CrystalOscillator::reset() {
    cycles.clear();
    start();
}

std::string CrystalOscillator::toString() const {
    return "-- Clock --\n"
           "Cycle count: " +
           std::to_string(cycles.size()) + '\n';
}

std::string CrystalOscillator::toStringMD() const {
    if (Logs::getClockHistoryConfig()) {
        std::string content =
            "# Clock\n"
            "| Cycle | Time (ns) |\n"
            "|-------|-----------|\n";
        for (int index = 0; index < cycles.size(); index++) {
            content += std::format("| {} | {} |\n", index+1, cycles[index]);
        }
        return content;
    }

    return "# Clock\n"
           "Cycle count: " +
           std::to_string(cycles.size()) + '\n';
}