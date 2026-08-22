#include "W65C02.h"

#include <cstdint>
#include <utility>

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
        registers.RW = true;     // Set to read
        bus.setAddress(0x01FF);  // Dummy address
        registers.P.I = true;
        registers.P.D = false;
    }});
    // Cycle 2: Dummy-readcycle
    callQueue.push({[&] {
        registers.RW = true;     // Set to read
        bus.setAddress(0x01FE);  // Dummy address
    }});
    // Cycle 3: Dummy-readcycle
    callQueue.push({[&] {
        registers.RW = true;     // Set to read
        bus.setAddress(0x01FD);  // Dummy address
    }});
    // Cycle 4: Dummy-readcycle
    callQueue.push({[&] {
        registers.RW = true;     // Set to read
        bus.setAddress(0x01FC);  // Dummy address
    }});
    // Cycle 5: Dummy-readcycle
    callQueue.push({[&] {
        registers.RW = true;     // Set to read
        bus.setAddress(0x01FB);  // Dummy address
    }});
    // Cycle 6: Load reset vector
    callQueue.push({[&] {
                        registers.RW = true;     // set to read
                        bus.setAddress(0xFFFC);  // Low-reset vector byte
                    },
                    [&] { registers.PC = bus.getData(); }});
    // Cycle 7: Load reset vector
    callQueue.push({[&] {
                        registers.RW = true;     // set to read
                        bus.setAddress(0xFFFD);  // High-reset vector byte
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

void W65C02::onClockCycle(const Phase phase) {
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

        // Remove call from the queue after Ph1 and Ph2 was executed
        callQueue.pop();
    }
}

std::string W65C02::toStringMD() const { return "# CPU\n" + registers.toStringMD(); }

void W65C02::callInstruction() {
    // Note the cycle count includes one fetch cycle
    switch (registers.IR) {
        case 0x00:
            QueueBRK();
            break;
        case 0x01:
        case 0x21:
        case 0x41:
        case 0x61:
        case 0xA1:
        case 0xC1:
        case 0xE1:
            QueueReadInstruction(ReadAddressingMode::IndexedIndirect);
            break;
        case 0x05:
        case 0x24:
        case 0x25:
        case 0x45:
        case 0x65:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xC4:
        case 0xC5:
        case 0xE4:
        case 0xE5:
            QueueReadInstruction(ReadAddressingMode::ZeroPage);
            break;
        case 0x06:
        case 0x26:
        case 0x46:
        case 0x66:
        case 0xC6:
        case 0xE6:
            QueueModifyInstruction(ModifyAddressingMode::ZeroPage);
            break;
        case 0x08:
            QueuePush(ProcessorStatusForPush(true));
            break;
        case 0x09:
        case 0x29:
        case 0x49:
        case 0x69:
        case 0xA0:
        case 0xA2:
        case 0xA9:
        case 0xC0:
        case 0xC9:
        case 0xE0:
        case 0xE9:
            QueueReadInstruction(ReadAddressingMode::Immediate);
            break;
        case 0x0A:
        case 0x2A:
        case 0x4A:
        case 0x6A:
            QueueModifyInstruction(ModifyAddressingMode::Accumulator);
            break;
        case 0x0D:
        case 0x2C:
        case 0x2D:
        case 0x4D:
        case 0x6D:
        case 0xAC:
        case 0xAD:
        case 0xAE:
        case 0xCC:
        case 0xCD:
        case 0xEC:
        case 0xED:
            QueueReadInstruction(ReadAddressingMode::Absolute);
            break;
        case 0x0E:
        case 0x2E:
        case 0x4E:
        case 0x6E:
        case 0xCE:
        case 0xEE:
            QueueModifyInstruction(ModifyAddressingMode::Absolute);
            break;
        case 0x10:
        case 0x30:
        case 0x50:
        case 0x70:
        case 0x90:
        case 0xB0:
        case 0xD0:
        case 0xF0:
            QueueBranch();
            break;
        case 0x11:
        case 0x31:
        case 0x51:
        case 0x71:
        case 0xB1:
        case 0xD1:
        case 0xF1:
            QueueReadInstruction(ReadAddressingMode::IndirectIndexed);
            break;
        case 0x15:
        case 0x35:
        case 0x55:
        case 0x75:
        case 0xB4:
        case 0xB5:
        case 0xD5:
        case 0xF5:
            QueueReadInstruction(ReadAddressingMode::ZeroPageX);
            break;
        case 0x16:
        case 0x36:
        case 0x56:
        case 0x76:
        case 0xD6:
        case 0xF6:
            QueueModifyInstruction(ModifyAddressingMode::ZeroPageX);
            break;
        case 0x18:
            // CLC; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] { registers.P.C = false; });
            break;
        case 0x19:
        case 0x39:
        case 0x59:
        case 0x79:
        case 0xB9:
        case 0xBE:
        case 0xD9:
        case 0xF9:
            QueueReadInstruction(ReadAddressingMode::AbsoluteY);
            break;
        case 0x1D:
        case 0x3D:
        case 0x5D:
        case 0x7D:
        case 0xBC:
        case 0xBD:
        case 0xDD:
        case 0xFD:
            QueueReadInstruction(ReadAddressingMode::AbsoluteX);
            break;
        case 0x1E:
        case 0x3E:
        case 0x5E:
        case 0x7E:
        case 0xDE:
        case 0xFE:
            QueueModifyInstruction(ModifyAddressingMode::AbsoluteX);
            break;
        case 0x20:
            QueueJSR();
            break;
        case 0x28:
            QueuePull(true);
            break;
        case 0x38:
            // SEC; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] { registers.P.C = true; });
            break;
        case 0x40:
            QueueRTI();
            break;
        case 0x48:
            QueuePush(registers.A);
            break;
        case 0x4C:
            QueueJMPAbsolute();
            break;
        case 0x58:
            // CLI; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] { registers.P.I = false; });
            break;
        case 0x60:
            QueueRTS();
            break;
        case 0x68:
            QueuePull(false);
            break;
        case 0x6C:
            QueueJMPIndirect();
            break;
        case 0x78:
            // SEI; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] { registers.P.I = true; });
            break;
        case 0x81:
            // STA ($nn,X); Addressing: x-indexed zero page indirect; Cycles: 6; Bytes: 2
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] {
                registers.RW = READ;
                registers.ADL += registers.X;
            }});
            callQueue.push({[this] { bus.setAddress(registers.ADL); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] { ReadNextZPPh1(); }, [this] { SetADHPh2(); }});
            callQueue.push({[this] { WriteRegisterPh1(); }});
            break;
        case 0x84:
        case 0x85:
        case 0x86:
            // STY/STA/STX $nn; Addressing: zero page; Cycles: 3; Bytes: 2
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] { WriteRegisterPh1(); }});
            break;
        case 0x88:
            // DEY; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] { SetZN(--registers.Y); });
            break;
        case 0x8A:
            // TXA; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] {
                registers.A = registers.X;
                SetZN(registers.A);
            });
            break;
        case 0x8C:
        case 0x8D:
        case 0x8E:
            // STY/STA/STX $nnnn; Addressing: absolute; Cycles: 4; Bytes: 3
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADHPh2(); }});
            callQueue.push({[this] { WriteRegisterPh1(); }});
            break;
        case 0x91:
            // STA ($nn),Y; Addressing: indirect y-indexed; Cycles: 6; Bytes: 2
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] { ReadADPh1(); }, [this] { registers.ADL = bus.getData(); }});
            callQueue.push({[this] { ReadNextZPPh1(); }, [this] { SetADHPh2(); }});
            callQueue.push({[this] { AddIndexToAddress(registers.Y); }});
            callQueue.push({[this] { WriteRegisterPh1(); }});
            break;
        case 0x94:
        case 0x95:
            // STY/STA $nn,X; Addressing: x-indexed zero page; Cycles: 4; Bytes: 2
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] {
                registers.RW = READ;
                registers.ADL += registers.X;
            }});
            callQueue.push({[this] { WriteRegisterPh1(); }});
            break;
        case 0x96:
            // STX $nn,Y; Addressing: y-indexed zero page; Cycles: 4; Bytes: 2
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] {
                registers.RW = READ;
                registers.ADL += registers.Y;
            }});
            callQueue.push({[this] { WriteRegisterPh1(); }});
            break;
        case 0x98:
            // TYA; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] {
                registers.A = registers.Y;
                SetZN(registers.A);
            });
            break;
        case 0x9A:
            // TXS; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] { registers.S = registers.X; });
            break;
        case 0x99:
            // STA $nnnn,Y; Addressing: y-indexed absolute; Cycles: 5; Bytes: 3
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADHPh2(); }});
            callQueue.push({[this] { AddIndexToAddress(registers.Y); }});
            callQueue.push({[this] { WriteRegisterPh1(); }});
            break;
        case 0x9D:
            // STA $nnnn,X; Addressing: x-indexed absolute; Cycles: 5; Bytes: 3
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADHPh2(); }});
            callQueue.push({[this] { AddIndexToAddress(registers.X); }});
            callQueue.push({[this] { WriteRegisterPh1(); }});
            break;
        case 0xA8:
            // TAY; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] {
                registers.Y = registers.A;
                SetZN(registers.Y);
            });
            break;
        case 0xAA:
            // TAX; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] {
                registers.X = registers.A;
                SetZN(registers.X);
            });
            break;
        case 0xB6:
            QueueReadInstruction(ReadAddressingMode::ZeroPageY);
            break;
        case 0xB8:
            // CLV; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] { registers.P.V = false; });
            break;
        case 0xBA:
            // TSX; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] {
                registers.X = registers.S;
                SetZN(registers.X);
            });
            break;
        case 0xC8:
            // INY; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] { SetZN(++registers.Y); });
            break;
        case 0xCA:
            // DEX; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] { SetZN(--registers.X); });
            break;
        case 0xD8:
            // CLD; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] { registers.P.D = false; });
            break;
        case 0xE8:
            // INX; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] { SetZN(++registers.X); });
            break;
        case 0xEA:
            // NOP; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([] {});
            break;
        case 0xF8:
            // SED; Addressing: implied; Cycles: 2; Bytes: 1
            ImpliedInstruction([this] { registers.P.D = true; });
            break;
        default:
            throw EmulatorException(e_CPU, e_CRITICAL, 1400, "Op code does not exist.");
    }
}

void W65C02::QueueReadInstruction(const ReadAddressingMode mode) {
    switch (mode) {
        case ReadAddressingMode::Immediate:
            callQueue.push({[this] { ReadPCPh1(); }, [this] { ExecuteReadOperationPh2(); }});
            break;
        case ReadAddressingMode::ZeroPage:
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] { ReadADPh1(); }, [this] { ExecuteReadOperationPh2(); }});
            break;
        case ReadAddressingMode::ZeroPageX:
        case ReadAddressingMode::ZeroPageY:
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this, mode] {
                registers.RW = READ;
                registers.ADL +=
                    mode == ReadAddressingMode::ZeroPageX ? registers.X : registers.Y;
            }});
            callQueue.push({[this] { bus.setAddress(registers.ADL); },
                            [this] { ExecuteReadOperationPh2(); }});
            break;
        case ReadAddressingMode::Absolute:
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADHPh2(); }});
            callQueue.push({[this] { ReadADPh1(); }, [this] { ExecuteReadOperationPh2(); }});
            break;
        case ReadAddressingMode::AbsoluteX:
        case ReadAddressingMode::AbsoluteY:
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADHPh2(); }});
            callQueue.push(
                {mode == ReadAddressingMode::AbsoluteX
                     ? std::function<void()>([this] { XIndexedCycle4Ph1(); })
                     : std::function<void()>([this] { YIndexedCycle4Ph1(); }),
                 [this] { ExecuteReadOperationPh2(); }});
            break;
        case ReadAddressingMode::IndexedIndirect:
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] {
                registers.RW = READ;
                registers.ADL += registers.X;
            }});
            callQueue.push({[this] { bus.setAddress(registers.ADL); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] { ReadNextZPPh1(); }, [this] { SetADHPh2(); }});
            callQueue.push({[this] { ReadADPh1(); }, [this] { ExecuteReadOperationPh2(); }});
            break;
        case ReadAddressingMode::IndirectIndexed:
            callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
            callQueue.push({[this] { ReadADPh1(); }, [this] { IndirectYIndexedCycle3Ph2(); }});
            break;
    }
}

void W65C02::QueueBranch() {
    callQueue.push({[this] { ReadPCPh1(); }, [this] { BranchPh2(); }});
}

void W65C02::BranchPh2() {
    bool takeBranch = false;
    switch (registers.IR) {
        case 0x10:
            takeBranch = !registers.P.N;
            break;
        case 0x30:
            takeBranch = registers.P.N;
            break;
        case 0x50:
            takeBranch = !registers.P.V;
            break;
        case 0x70:
            takeBranch = registers.P.V;
            break;
        case 0x90:
            takeBranch = !registers.P.C;
            break;
        case 0xB0:
            takeBranch = registers.P.C;
            break;
        case 0xD0:
            takeBranch = !registers.P.Z;
            break;
        case 0xF0:
            takeBranch = registers.P.Z;
            break;
        default:
            break;
    }
    if (!takeBranch) return;

    const auto offset = static_cast<std::int8_t>(bus.getData());
    const Word target = static_cast<Word>(registers.PC + offset);
    registers.ADL = static_cast<Byte>(target & 0x00FF);
    registers.ADH = static_cast<Byte>(target >> 8);

    if ((registers.PC & 0xFF00) != (target & 0xFF00)) {
        callQueue.push({});
    }
    callQueue.push({nullptr, [this] {
                        registers.PC = static_cast<Word>((registers.ADH << 8) + registers.ADL);
                    }});
}

void W65C02::QueueJMPAbsolute() {
    callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
    callQueue.push({[this] { ReadPCPh1(); }, [this] {
                        registers.PC = static_cast<Word>((bus.getData() << 8) + registers.ADL);
                    }});
}

void W65C02::QueueJMPIndirect() {
    callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
    callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADHPh2(); }});
    callQueue.push({[this] { ReadADPh1(); }, [this] { registers.TMP = bus.getData(); }});
    callQueue.push({[this] {
                        registers.RW = READ;
                        const Word highAddress = static_cast<Word>(
                            (registers.ADH << 8) | static_cast<Byte>(registers.ADL + 1));
                        bus.setAddress(highAddress);
                    },
                    [this] {
                        registers.PC = static_cast<Word>((bus.getData() << 8) + registers.TMP);
                    }});
}

void W65C02::QueueJSR() {
    callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
    callQueue.push({});
    callQueue.push({[this] { PushPh1(static_cast<Byte>(registers.PC >> 8)); },
                    [this] { --registers.S; }});
    callQueue.push({[this] { PushPh1(static_cast<Byte>(registers.PC & 0x00FF)); },
                    [this] { --registers.S; }});
    callQueue.push({[this] { ReadPCPh1(); }, [this] {
                        registers.PC = static_cast<Word>((bus.getData() << 8) + registers.ADL);
                    }});
}

void W65C02::QueueRTS() {
    callQueue.push({});
    callQueue.push({[this] { PullStackPh1(); }, [this] { registers.TMP = bus.getData(); }});
    callQueue.push({[this] { PullStackPh1(); }, [this] { registers.ADH = bus.getData(); }});
    callQueue.push({nullptr, [this] {
                        registers.PC = static_cast<Word>((registers.ADH << 8) + registers.TMP);
                    }});
    callQueue.push({nullptr, [this] { ++registers.PC; }});
}

void W65C02::QueueBRK() {
    callQueue.push({[this] { ReadPCPh1(); }});
    callQueue.push({[this] { PushPh1(static_cast<Byte>(registers.PC >> 8)); },
                    [this] { --registers.S; }});
    callQueue.push({[this] { PushPh1(static_cast<Byte>(registers.PC & 0x00FF)); },
                    [this] { --registers.S; }});
    callQueue.push({[this] { PushPh1(ProcessorStatusForPush(true)); },
                    [this] {
                        --registers.S;
                        registers.P.I = true;
                        registers.P.D = false;
                    }});
    callQueue.push({[this] {
                        registers.RW = READ;
                        bus.setAddress(0xFFFE);
                    },
                    [this] { registers.TMP = bus.getData(); }});
    callQueue.push({[this] {
                        registers.RW = READ;
                        bus.setAddress(0xFFFF);
                    },
                    [this] {
                        registers.PC = static_cast<Word>((bus.getData() << 8) + registers.TMP);
                    }});
}

void W65C02::QueueRTI() {
    callQueue.push({});
    callQueue.push({[this] { PullStackPh1(); }, [this] { SetProcessorStatus(bus.getData()); }});
    callQueue.push({[this] { PullStackPh1(); }, [this] { registers.TMP = bus.getData(); }});
    callQueue.push({[this] { PullStackPh1(); }, [this] { registers.ADH = bus.getData(); }});
    callQueue.push({nullptr, [this] {
                        registers.PC = static_cast<Word>((registers.ADH << 8) + registers.TMP);
                    }});
}

void W65C02::QueuePush(const Byte value) {
    callQueue.push({});
    callQueue.push({[this, value] { PushPh1(value); }, [this] { --registers.S; }});
}

void W65C02::QueuePull(const bool processorStatus) {
    callQueue.push({});
    callQueue.push({[this] { PullStackPh1(); }, [this] { registers.TMP = bus.getData(); }});
    callQueue.push({nullptr, [this, processorStatus] {
                        if (processorStatus) {
                            SetProcessorStatus(registers.TMP);
                        } else {
                            registers.A = registers.TMP;
                            SetZN(registers.A);
                        }
                    }});
}

Byte W65C02::ProcessorStatusForPush(const bool breakFlag) const {
    return static_cast<Byte>((registers.P.N << 7) | (registers.P.V << 6) | 0x20 |
                             (breakFlag << 4) | (registers.P.D << 3) |
                             (registers.P.I << 2) | (registers.P.Z << 1) | registers.P.C);
}

void W65C02::SetProcessorStatus(const Byte value) {
    registers.P.N = (value & 0x80) != 0;
    registers.P.V = (value & 0x40) != 0;
    registers.P.D = (value & 0x08) != 0;
    registers.P.I = (value & 0x04) != 0;
    registers.P.Z = (value & 0x02) != 0;
    registers.P.C = (value & 0x01) != 0;
}

void W65C02::PushPh1(const Byte value) {
    registers.RW = WRITE;
    bus.setAddress(static_cast<Word>(0x0100 | registers.S));
    bus.setData(value);
}

void W65C02::PullStackPh1() {
    registers.RW = READ;
    ++registers.S;
    bus.setAddress(static_cast<Word>(0x0100 | registers.S));
}

void W65C02::QueueModifyInstruction(const ModifyAddressingMode mode) {
    if (mode == ModifyAddressingMode::Accumulator) {
        ImpliedInstruction([this] { registers.A = ModifyValue(registers.A); });
        return;
    }

    callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADLPh2(); }});
    if (mode == ModifyAddressingMode::Absolute || mode == ModifyAddressingMode::AbsoluteX) {
        callQueue.push({[this] { ReadPCPh1(); }, [this] { SetADHPh2(); }});
    }
    if (mode == ModifyAddressingMode::ZeroPageX) {
        callQueue.push({[this] {
            registers.RW = READ;
            registers.ADL += registers.X;
        }});
    } else if (mode == ModifyAddressingMode::AbsoluteX) {
        callQueue.push({[this] { AddIndexToAddress(registers.X); }});
    }

    callQueue.push({[this] { ReadADPh1(); }, [this] { ExecuteModifyOperationPh2(); }});
    callQueue.push({[this] {
        registers.RW = READ;
        bus.setAddress((registers.ADH << 8) + registers.ADL);
    }});
    callQueue.push({[this] { WriteModifiedValuePh1(); }});
}

Byte W65C02::ModifyValue(const Byte value) {
    Byte result = value;
    switch (registers.IR) {
        case 0x06:
        case 0x0A:
        case 0x0E:
        case 0x16:
        case 0x1E:
            registers.P.C = (value & 0x80) != 0;
            result = static_cast<Byte>(value << 1);
            break;
        case 0x26:
        case 0x2A:
        case 0x2E:
        case 0x36:
        case 0x3E: {
            const bool carryIn = registers.P.C;
            registers.P.C = (value & 0x80) != 0;
            result = static_cast<Byte>((value << 1) | carryIn);
            break;
        }
        case 0x46:
        case 0x4A:
        case 0x4E:
        case 0x56:
        case 0x5E:
            registers.P.C = (value & 0x01) != 0;
            result = static_cast<Byte>(value >> 1);
            break;
        case 0x66:
        case 0x6A:
        case 0x6E:
        case 0x76:
        case 0x7E: {
            const bool carryIn = registers.P.C;
            registers.P.C = (value & 0x01) != 0;
            result = static_cast<Byte>((value >> 1) | (carryIn ? 0x80 : 0x00));
            break;
        }
        case 0xC6:
        case 0xCE:
        case 0xD6:
        case 0xDE:
            result = static_cast<Byte>(value - 1);
            break;
        case 0xE6:
        case 0xEE:
        case 0xF6:
        case 0xFE:
            result = static_cast<Byte>(value + 1);
            break;
        default:
            break;
    }
    SetZN(result);
    return result;
}

void W65C02::ExecuteModifyOperationPh2() { registers.TMP = ModifyValue(bus.getData()); }

void W65C02::WriteModifiedValuePh1() {
    registers.RW = WRITE;
    bus.setAddress((registers.ADH << 8) + registers.ADL);
    bus.setData(registers.TMP);
}

void W65C02::XIndexedCycle4Ph1() {
    // Check for page crossing
    if (registers.ADL + registers.X <= 0xFF) {
        // No page crossing so data can bei read same cycle in Ph2
        registers.ADL += registers.X;
        bus.setAddress((registers.ADH << 8) + registers.ADL);
    } else {
        // Page crossed, an additional cycle is added
        registers.ADL += registers.X;
        callQueue.front()[1] = nullptr;
        callQueue.push({[this] {
                            registers.ADH++;
                            bus.setAddress((registers.ADH << 8) + registers.ADL);
                        },
                        [this] { ExecuteReadOperationPh2(); }});
    }
}

void W65C02::YIndexedCycle4Ph1() {
    // Check for page crossing
    if (registers.ADL + registers.Y <= 0xFF) {
        // No page crossing so data can bei read same cycle in Ph2
        registers.ADL += registers.Y;
        bus.setAddress((registers.ADH << 8) + registers.ADL);
    } else {
        // Page crossed, an additional cycle is added
        registers.ADL += registers.Y;
        callQueue.front()[1] = nullptr;
        callQueue.push({[this] {
                            registers.ADH++;
                            bus.setAddress((registers.ADH << 8) + registers.ADL);
                        },
                        [this] { ExecuteReadOperationPh2(); }});
    }
}

void W65C02::IndirectYIndexedCycle3Ph2() {
    // Check if page crossed
    if (registers.Y + bus.getData() <= 0xFF) {
        // No page crossing
        registers.ADL = registers.Y + bus.getData();

        // Read a high-address byte from the zero page and load it in ADH
        callQueue.push({[this] { ReadNextZPPh1(); }, [this] { SetADHPh2(); }});
    } else {
        // Page crossed, extra cycle needed
        registers.ADL = registers.Y + bus.getData() - 0x100;
        // Read high-address byte from the zero page
        callQueue.push({[this] { ReadNextZPPh1(); }});
        // Store address in ADH incremented by one, because of page cross
        callQueue.push({[this] { registers.ADH = bus.getData() + 1; }});
    }

    // Read from AD register and store inside A
    callQueue.push({[this] { ReadADPh1(); }, [this] { ExecuteReadOperationPh2(); }});
}

void W65C02::ExecuteReadOperationPh2() {
    const Byte operand = bus.getData();
    switch (registers.IR) {
        case 0x01:
        case 0x05:
        case 0x09:
        case 0x0D:
        case 0x11:
        case 0x15:
        case 0x19:
        case 0x1D:
            registers.A |= operand;
            SetZN(registers.A);
            return;
        case 0x21:
        case 0x25:
        case 0x29:
        case 0x2D:
        case 0x31:
        case 0x35:
        case 0x39:
        case 0x3D:
            registers.A &= operand;
            SetZN(registers.A);
            return;
        case 0x24:
        case 0x2C:
            registers.P.Z = (registers.A & operand) == 0;
            registers.P.V = (operand & 0x40) != 0;
            registers.P.N = (operand & 0x80) != 0;
            return;
        case 0x41:
        case 0x45:
        case 0x49:
        case 0x4D:
        case 0x51:
        case 0x55:
        case 0x59:
        case 0x5D:
            registers.A ^= operand;
            SetZN(registers.A);
            return;
        case 0x61:
        case 0x65:
        case 0x69:
        case 0x6D:
        case 0x71:
        case 0x75:
        case 0x79:
        case 0x7D:
            AddWithCarry(operand);
            if (registers.P.D) callQueue.push({});
            return;
        case 0xA2:
        case 0xA6:
        case 0xAE:
        case 0xB6:
        case 0xBE:
            registers.X = operand;
            SetZN(registers.X);
            return;
        case 0xA0:
        case 0xA4:
        case 0xAC:
        case 0xB4:
        case 0xBC:
            registers.Y = operand;
            SetZN(registers.Y);
            return;
        case 0xC0:
        case 0xC4:
        case 0xCC:
            Compare(registers.Y, operand);
            return;
        case 0xC1:
        case 0xC5:
        case 0xC9:
        case 0xCD:
        case 0xD1:
        case 0xD5:
        case 0xD9:
        case 0xDD:
            Compare(registers.A, operand);
            return;
        case 0xE0:
        case 0xE4:
        case 0xEC:
            Compare(registers.X, operand);
            return;
        case 0xE1:
        case 0xE5:
        case 0xE9:
        case 0xED:
        case 0xF1:
        case 0xF5:
        case 0xF9:
        case 0xFD:
            SubtractWithCarry(operand);
            if (registers.P.D) callQueue.push({});
            return;
        default:
            registers.A = operand;
            SetZN(registers.A);
            return;
    }
}

void W65C02::Compare(const Byte value, const Byte operand) {
    const Byte result = static_cast<Byte>(value - operand);
    registers.P.C = value >= operand;
    registers.P.Z = value == operand;
    registers.P.N = (result & 0x80) != 0;
}

void W65C02::AddWithCarry(const Byte operand) {
    const Byte accumulator = registers.A;
    const unsigned int binarySum = accumulator + operand + registers.P.C;
    const Byte binaryResult = static_cast<Byte>(binarySum);
    registers.P.V = ((~(accumulator ^ operand) & (accumulator ^ binaryResult)) & 0x80) != 0;

    if (!registers.P.D) {
        registers.A = binaryResult;
        registers.P.C = binarySum > 0xFF;
        SetZN(registers.A);
        return;
    }

    unsigned int low = (accumulator & 0x0F) + (operand & 0x0F) + registers.P.C;
    unsigned int high = (accumulator >> 4) + (operand >> 4);
    if (low > 9) {
        low += 6;
        ++high;
    }
    if (high > 9) high += 6;

    registers.P.C = high > 0x0F;
    registers.A = static_cast<Byte>((high << 4) | (low & 0x0F));
    SetZN(registers.A);
}

void W65C02::SubtractWithCarry(const Byte operand) {
    const Byte accumulator = registers.A;
    const int borrow = registers.P.C ? 0 : 1;
    const int binaryDifference = accumulator - operand - borrow;
    const Byte binaryResult = static_cast<Byte>(binaryDifference);
    registers.P.V = (((accumulator ^ binaryResult) & (accumulator ^ operand)) & 0x80) != 0;

    if (!registers.P.D) {
        registers.A = binaryResult;
        registers.P.C = binaryDifference >= 0;
        SetZN(registers.A);
        return;
    }

    int low = (accumulator & 0x0F) - (operand & 0x0F) - borrow;
    int high = (accumulator >> 4) - (operand >> 4);
    if (low < 0) {
        low -= 6;
        --high;
    }
    if (high < 0) high -= 6;

    registers.P.C = binaryDifference >= 0;
    registers.A = static_cast<Byte>(((high * 16) & 0xF0) | (low & 0x0F));
    SetZN(registers.A);
}

void W65C02::SetZN(const Byte value) {
    registers.P.Z = value == 0;
    registers.P.N = (value & 0x80) != 0;
}

void W65C02::ImpliedInstruction(std::function<void()> operation) {
    callQueue.push({nullptr, std::move(operation)});
}

void W65C02::WriteRegisterPh1() {
    Byte value = registers.A;
    switch (registers.IR) {
        case 0x86:
        case 0x8E:
        case 0x96:
            value = registers.X;
            break;
        case 0x84:
        case 0x8C:
        case 0x94:
            value = registers.Y;
            break;
        default:
            break;
    }

    registers.RW = WRITE;
    bus.setAddress((registers.ADH << 8) + registers.ADL);
    bus.setData(value);
}

void W65C02::AddIndexToAddress(const Byte index) {
    registers.RW = READ;
    const Word address = static_cast<Word>((registers.ADH << 8) + registers.ADL + index);
    registers.ADL = static_cast<Byte>(address & 0x00FF);
    registers.ADH = static_cast<Byte>(address >> 8);
    bus.setAddress(address);
}

void W65C02::SetADLPh2() {
    registers.ADL = bus.getData();
    registers.ADH = 0;
}

void W65C02::SetADHPh2() { registers.ADH = bus.getData(); }

void W65C02::ReadPCPh1() {
    registers.RW = READ;
    bus.setAddress(registers.PC);
    registers.PC++;
}

void W65C02::ReadADPh1() {
    registers.RW = READ;
    bus.setAddress((registers.ADH << 8) + registers.ADL);
}

void W65C02::ReadNextZPPh1() {
    registers.RW = READ;
    bus.setAddress(static_cast<Byte>(bus.getAddress() + 1));
}
