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
    callQueue.push({[&] {
        registers.RW = true;     // set to read
        bus.setAddress(0x01FF);  // dummy address
        registers.P.I = true;
        registers.P.D = false;
    }});
    // Cycle 2: Dummy-readcycle
    callQueue.push({[&] {
        registers.RW = true;     // set to read
        bus.setAddress(0x01FE);  // dummy address
    }});
    // Cycle 3: Dummy-readcycle
    callQueue.push({[&] {
        registers.RW = true;     // set to read
        bus.setAddress(0x01FD);  // dummy address
    }});
    // Cycle 4: Dummy-readcycle
    callQueue.push({[&] {
        registers.RW = true;     // set to read
        bus.setAddress(0x01FC);  // dummy address
    }});
    // Cycle 5: Dummy-readcycle
    callQueue.push({[&] {
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
        case 0xA5:
            // LDA $nn; Addressing: zero page; Cycles: 3; Bytes: 2
            // Read at PC and store in ADL
            callQueue.push({[this] { ReadPCPh1(); }, [this] { registers.ADL = bus.getData(); }});
            // Read at ADL und load the value in A
            callQueue.push({[this] { ReadADPh1(); }, [this] { LDAStorePh2(); }});
            break;
        case 0xA9:
            // LDA #$nn; Addressing: immediate; Cycles: 2; Bytes: 2
            // Read at PC and store in A
            callQueue.push({[this] { ReadPCPh1(); }, [this] { LDAStorePh2(); }});
            break;
        case 0xAD:
            // LDA $nnnn; Addressing: absolute; Cycles: 4; Bytes: 3
            // Fetch low byte
            callQueue.push({[this] { ReadPCPh1(); }, [this] { registers.ADL = bus.getData(); }});
            // Fetch high byte
            callQueue.push({[this] { ReadPCPh1(); }, [this] { registers.ADH = bus.getData(); }});
            // Read from Address and load the value in A
            callQueue.push({[this] {
                                registers.RW = READ;
                                bus.setAddress((registers.ADH << 8) + registers.ADL);
                                registers.PC++;
                            },
                            [this] { LDAStorePh2(); }});
            break;
        case 0xB9:
            // LDA $nnnn,Y; Addressing: y-indexed absolute; Cycles: 4+p; Bytes 3
            // Fetch low byte
            callQueue.push({[this] { ReadPCPh1(); }, [this] { registers.ADL = bus.getData(); }});
            // Fetch high byte
            callQueue.push({[this] { ReadPCPh1(); }, [this] { registers.ADH = bus.getData(); }});
            // Add y to the address and load the value in A
            callQueue.push({[this] { YIndexedCycle4Ph1(); }, [this] { LDAStorePh2(); }});
            break;
        case 0xBD:
            // LDA $nnnn,X; Addressing: x-indexed absolute; Cycles: 4+p; Bytes 3
            // Fetch low byte
            callQueue.push({[this] { ReadPCPh1(); }, [this] { registers.ADL = bus.getData(); }});
            // Fetch high byte
            callQueue.push({[this] { ReadPCPh1(); }, [this] { registers.ADH = bus.getData(); }});
            // Add x to the address and load the value in a
            callQueue.push({[this] { XIndexedCycle4Ph1(); }, [this] { LDAStorePh2(); }});
            break;
        default:
            throw EmulatorException(e_CPU, e_CRITICAL, 1400, "Op code does not exist.");
    }
}

void W65C02::XIndexedCycle4Ph1() {
    // Check for page crossing
    if (registers.ADL + registers.X <= 0xFF) {
        // No page crossing so data can bei read same cycle in Ph2
        registers.ADL += registers.X;
        bus.setAddress((registers.ADH << 8) + registers.ADL);
    } else {
        // Page crossed, additional cycle is added
        registers.ADL += registers.X;
        callQueue.front()[1] = nullptr;
        callQueue.push({[this] {
                            registers.ADH++;
                            bus.setAddress((registers.ADH << 8) + registers.ADL);
                        },
                        [this] { LDAStorePh2(); }});
    }
}

void W65C02::YIndexedCycle4Ph1() {
    // Check for page crossing
    if (registers.ADL + registers.Y <= 0xFF) {
        // No page crossing so data can bei read same cycle in Ph2
        registers.ADL += registers.Y;
        bus.setAddress((registers.ADH << 8) + registers.ADL);
    } else {
        // Page crossed, additional cycle is added
        registers.ADL += registers.Y;
        callQueue.front()[1] = nullptr;
        callQueue.push({[this] {
                            registers.ADH++;
                            bus.setAddress((registers.ADH << 8) + registers.ADL);
                        },
                        [this] { LDAStorePh2(); }});
    }
}

// Used for LDA instructions to read data from the bus and write it in the A register
void W65C02::LDAStorePh2() {
    registers.A = bus.getData();
    registers.A == 0 ? registers.P.Z = true : registers.P.Z = false;
    registers.A >> 7 == 1 ? registers.P.N = true : registers.P.N = false;
}

void W65C02::ReadPCPh1() {
    registers.RW = READ;
    bus.setAddress(registers.PC);
    registers.PC++;
}

void W65C02::ReadADPh1() {
    registers.RW = READ;
    bus.setAddress((registers.ADH << 8) + registers.ADL);
}
