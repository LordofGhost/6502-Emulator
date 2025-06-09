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
    const std::function setRead_setBusPC_incrementPC = [&] {
        registers.RW = READ;
        bus.setAddress(registers.PC);
        registers.PC++;
    };

    // Used for LDA instructions
    const std::function setABus_checkPZ_checkPN = [&] {
        registers.A = bus.getData();
        registers.A == 0 ? registers.P.Z = true : registers.P.Z = false;
        registers.A >> 7 == 1 ? registers.P.N = true : registers.P.N = false;
    };

    // Used for X-indexed absolute
    const std::function XIndexedCycle4Ph1 = [&] {
        // Check for page crossing
        if (registers.ADL + registers.X <= 0xFF) {
            // No page crossing so data can bei read same cycle in Ph2
            registers.ADL += registers.X;
            bus.setAddress((registers.ADH << 8) + registers.ADL);
        } else {
            // Page crossed, additional cycle is added
            registers.ADL += registers.X - 0x80;
            callQueue.front()[1] = nullptr;
            callQueue.push({[&] {
                                registers.ADH++;
                                bus.setAddress((registers.ADH << 8) + registers.ADL);
                            },
                            setABus_checkPZ_checkPN});
        }
    };

    // Note the cycle count includes the 1 fetch cycle
    switch (registers.IR) {
        case 0xA9:
            // LDA #$nn; Addressing: immediate; Cycles: 2; Bytes: 2
            callQueue.push({setRead_setBusPC_incrementPC, setABus_checkPZ_checkPN});
            break;
        case 0xAD:
            // LDA $nnnn; Addressing: absolute; Cycles: 4; Bytes: 3
            callQueue.push({setRead_setBusPC_incrementPC, [&] { registers.ADL = bus.getData(); }});
            callQueue.push({setRead_setBusPC_incrementPC, nullptr});
            callQueue.push({[&] {
                                registers.RW = READ;
                                bus.setAddress((bus.getData() << 8) + registers.ADL);
                                registers.PC++;
                            },
                            setABus_checkPZ_checkPN});
            break;
        case 0xBD:
            // LDA $nnnn,X; Addressing: x-indexed absolute; Cycles: 4+p; Bytes 3
            // Fetch low byte
            callQueue.push({setRead_setBusPC_incrementPC, [&] { registers.ADL = bus.getData(); }});
            // Fetch high byte
            callQueue.push({setRead_setBusPC_incrementPC, [&] { registers.ADH = bus.getData(); }});

            callQueue.push({XIndexedCycle4Ph1, setABus_checkPZ_checkPN});
            break;
        default:
            throw EmulatorException(e_CPU, e_CRITICAL, 1400, "Op code does not exist.");
    }
}
