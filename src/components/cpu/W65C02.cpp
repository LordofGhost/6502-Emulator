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
    // SYNC pin is high
    callQueue.push({[&] {
                        registers.RW = READ;
                        bus.setAddress(registers.PC);
                    },
                    [&] {
                        // Set the Instruction Register
                        registers.IR = bus.getData();
                        // Fill the call stack with the instruction
                        callInstruction();
                        // Point the Program Counter to the next instruction
                        registers.PC++;
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

void W65C02::callInstruction() {
    // Note the cycle count includes the 1 fetch cycle
    switch (registers.IR) {
        case 0xA9:
            // Addressing: immediate; Cycles: 2; Bytes: 2
            callQueue.push({[&] {
                                registers.RW = READ;
                                bus.setAddress(registers.PC);
                                registers.PC++;
                            },
                            [&] {
                                registers.A = bus.getData();
                                registers.A == 0 ? registers.P.Z = true : registers.P.Z = false;
                                registers.A >> 7 == 1 ? registers.P.N = true
                                                      : registers.P.N = false;
                            }});
            break;
        default:
            throw EmulatorException(e_CPU, e_CRITICAL, 1400, "Op code does not exist.");
    }
}
