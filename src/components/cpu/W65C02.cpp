#include "W65C02.h"

#include <iostream>

#include "../../exceptions/EmulatorException.h"
#include "../Bus.h"

extern Bus bus;

void W65C02::reset() noexcept {
    // Delete all existing calls
    while (!callQueue.empty()) {
        callQueue.pop();
    }

    // Cycle 1: Set processor flags P: I=1,D=0
    callQueue.push({nullptr, [&] {
                        registers.RW = true;     // set to read
                        bus.setAddress(0x01FF);  // dummy address
                        registers.P.I = true;
                        registers.P.D = false;
                    }});
    // Cycle 2: Dummy-readcycle
    callQueue.push({nullptr, [&] {
                        registers.RW = true;     // set to read
                        bus.setAddress(0x01FE);  // dummy address
                    }});
    // Cycle 3: Dummy-readcycle
    callQueue.push({nullptr, [&] {
                        registers.RW = true;     // set to read
                        bus.setAddress(0x01FD);  // dummy address
                    }});
    // Cycle 4: Dummy-readcycle
    callQueue.push({nullptr, [&] {
                        registers.RW = true;     // set to read
                        bus.setAddress(0x01FC);  // dummy address
                    }});
    // Cycle 5: Dummy-readcycle
    callQueue.push({nullptr, [&] {
                        registers.RW = true;     // set to read
                        bus.setAddress(0x01FB);  // dummy address
                    }});
    // Cycle 6: Load reset vector
    callQueue.push({[&] {
                        registers.RW = true;     // set to read
                        bus.setAddress(0xFFFC);  // Low reset vector byte
                    },
                    [&] { registers.PC = bus.getData(); }});
    // Cycle 7: Load reset vector
    callQueue.push({[&] {
                        registers.RW = true;     // set to read
                        bus.setAddress(0xFFFD);  // High reset vector byte
                    },
                    [&] {
                        registers.PC += bus.getData() << 8;  // Add high byte to Program Counter
                    }});
}

void W65C02::fetch() {
    callQueue.push({[&] {
                        registers.RW = true;
                        bus.setAddress(registers.PC);
                    },
                    [&] {
                        // Decode & Check if op code exists; increment Program Counter
                        if (decodeLogic.contains(registers.PC)) {
                            // Get instruction and execute it, which fills the call stack
                            decodeLogic.at(registers.PC)();
                            // Point the Program Counter to the next instruction
                            registers.PC++;
                        } else
                            throw EmulatorException(e_CPU, e_CRITICAL, 1400,
                                                    "Op code does not exist.");
                    }});
}

void W65C02::onClockCycle(Phase phase) {
    // Load new instruction if callQueue is empty
    if (callQueue.empty()) {
        fetch();
    }

    // Phase 1
    if (phase == Ph1) {
        const std::function<void()> ph1Call = callQueue.front()[0];

        // Securely execute function
        if (ph1Call != nullptr) ph1Call();
    }

    // Phase 2
    else {
        const std::function<void()> ph2Call = callQueue.front()[1];

        // Securely execute function
        if (ph2Call != nullptr) ph2Call();

        // Remove call from queue after Ph1 and Ph2 was executed
        callQueue.pop();
    }
}

std::string W65C02::toStringMD() const { return "# CPU\n" + registers.toStringMD(); }