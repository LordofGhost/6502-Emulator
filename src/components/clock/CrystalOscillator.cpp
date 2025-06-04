#include "CrystalOscillator.h"

#include "../../Main.h"
#include "../../exceptions/EmulatorException.h"
#include "../Component.h"

extern const std::array<Component *, 4> components;
extern Arguments arguments;
extern std::vector<EmulatorException> EmulatorExceptions;

[[noreturn]] void CrystalOscillator::start() {
    running = true;
    while (running) {
        // Set the cycle start time
        cycleStartTime = std::chrono::steady_clock::now();

        // Execute one cycle
        tick();

        // Set the cycle end time
        auto now = std::chrono::steady_clock::now();
        auto elapsed =
            std::chrono::duration_cast<std::chrono::nanoseconds>(now - cycleStartTime).count();
        cycles.push_back(elapsed);

        // Create logs at the of the cycle
        if (arguments.logsAll) Logs::log();

        running = !checkForStopSignal();
    }
}

void CrystalOscillator::tick() {
    for (Phase phase = Ph1; phase <= Ph2; phase++) {
        for (Component *component : components) {
            component->onClockCycle(phase);
        }
    }
}

bool CrystalOscillator::checkForStopSignal() {
    if (!EmulatorExceptions.empty()) return true;
    return false;
}

void CrystalOscillator::reset() {
    cycles.clear();
    running = false;
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
            content += std::format("| {} | {} |\n", index + 1, cycles[index]);
        }
        return content;
    }

    return "# Clock\n"
           "Cycle count: " +
           std::to_string(cycles.size()) + '\n';
}