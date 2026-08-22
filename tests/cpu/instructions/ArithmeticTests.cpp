#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "support/Official6502Opcodes.h"
#include "support/W65C02TestHarness.h"

namespace {
using namespace cpu_test;

std::string_view arithmeticModeName(const AddressingMode mode) {
    switch (mode) {
        case AddressingMode::Immediate:
            return "immediate";
        case AddressingMode::ZeroPage:
            return "zero page";
        case AddressingMode::ZeroPageX:
            return "zero page X";
        case AddressingMode::Absolute:
            return "absolute";
        case AddressingMode::AbsoluteX:
            return "absolute X";
        case AddressingMode::AbsoluteY:
            return "absolute Y";
        case AddressingMode::IndexedIndirect:
            return "indexed indirect";
        case AddressingMode::IndirectIndexed:
            return "indirect indexed";
        default:
            throw std::invalid_argument("Unsupported arithmetic addressing mode");
    }
}

InstructionCase makeArithmeticCase(const OpcodeSpec& spec, InitialState initial,
                                   const ExpectedState& expected, const Byte operand) {
    InstructionCase test = {
        .name = std::string(spec.mnemonic) + " " + std::string(arithmeticModeName(spec.mode)),
        .opcode = spec.opcode,
        .operands = {},
        .cycles = 0,
        .initial = initial,
        .expected = expected,
    };

    switch (spec.mode) {
        case AddressingMode::Immediate:
            test.operands = {operand};
            test.cycles = 2;
            break;
        case AddressingMode::ZeroPage:
            test.operands = {0x44};
            test.cycles = 3;
            test.setup = [operand](TestMemory& memory) { memory.write(0x0044, operand); };
            break;
        case AddressingMode::ZeroPageX:
            test.operands = {0x40};
            test.cycles = 4;
            test.initial.X = 0x04;
            test.setup = [operand](TestMemory& memory) { memory.write(0x0044, operand); };
            break;
        case AddressingMode::Absolute:
            test.operands = {0x34, 0x12};
            test.cycles = 4;
            test.setup = [operand](TestMemory& memory) { memory.write(0x1234, operand); };
            break;
        case AddressingMode::AbsoluteX:
            test.operands = {0x20, 0x12};
            test.cycles = 4;
            test.initial.X = 0x0F;
            test.setup = [operand](TestMemory& memory) { memory.write(0x122F, operand); };
            break;
        case AddressingMode::AbsoluteY:
            test.operands = {0x30, 0x12};
            test.cycles = 4;
            test.initial.Y = 0x0E;
            test.setup = [operand](TestMemory& memory) { memory.write(0x123E, operand); };
            break;
        case AddressingMode::IndexedIndirect:
            test.operands = {0x20};
            test.cycles = 6;
            test.initial.X = 0x04;
            test.setup = [operand](TestMemory& memory) {
                memory.write(0x0024, 0x78);
                memory.write(0x0025, 0x56);
                memory.write(0x5678, operand);
            };
            break;
        case AddressingMode::IndirectIndexed:
            test.operands = {0x30};
            test.cycles = 5;
            test.initial.Y = 0x0A;
            test.setup = [operand](TestMemory& memory) {
                memory.write(0x0030, 0x40);
                memory.write(0x0031, 0x20);
                memory.write(0x204A, operand);
            };
            break;
        default:
            throw std::invalid_argument("Unsupported arithmetic addressing mode");
    }
    return test;
}

std::vector<InstructionCase> makeArithmeticCases(const std::string_view mnemonic,
                                                  const InitialState& initial,
                                                  const ExpectedState& expected,
                                                  const Byte operand) {
    std::vector<InstructionCase> cases;
    for (const auto& spec : official6502Opcodes()) {
        if (spec.mnemonic == mnemonic) {
            cases.push_back(makeArithmeticCase(spec, initial, expected, operand));
        }
    }
    return cases;
}

const std::vector<InstructionCase> ARITHMETIC_CASES = [] {
    auto cases = makeArithmeticCases("ADC", {.A = 0x50},
                                     {.A = 0xA0, .N = true, .V = true}, 0x50);
    const auto sbcCases = makeArithmeticCases(
        "SBC", {.A = 0x10, .C = true}, {.A = 0xF0, .N = true, .C = false}, 0x20);
    cases.insert(cases.end(), sbcCases.begin(), sbcCases.end());

    cases.push_back({.name = "ADC consumes carry-in and reports carry-out and zero",
                     .opcode = 0x69,
                     .operands = {0x00},
                     .cycles = 2,
                     .initial = {.A = 0xFF, .C = true},
                     .expected = {.A = 0x00, .Z = true, .C = true}});
    cases.push_back({.name = "ADC detects signed overflow for two negative operands",
                     .opcode = 0x69,
                     .operands = {0x80},
                     .cycles = 2,
                     .initial = {.A = 0x80},
                     .expected = {.A = 0x00, .V = true, .Z = true, .C = true}});
    cases.push_back({.name = "SBC detects signed overflow",
                     .opcode = 0xE9,
                     .operands = {0x01},
                     .cycles = 2,
                     .initial = {.A = 0x80, .C = true},
                     .expected = {.A = 0x7F, .V = true, .C = true}});
    cases.push_back({.name = "SBC consumes a cleared carry as borrow-in",
                     .opcode = 0xE9,
                     .operands = {0x00},
                     .cycles = 2,
                     .initial = {.A = 0x00, .C = false},
                     .expected = {.A = 0xFF, .N = true, .C = false}});
    cases.push_back({.name = "ADC BCD adjusts both digits and adds a decimal cycle",
                     .opcode = 0x69,
                     .operands = {0x46},
                     .cycles = 3,
                     .initial = {.A = 0x58, .D = true},
                     .expected = {.A = 0x04, .N = false, .V = true, .C = true}});
    cases.push_back({.name = "ADC BCD sets zero after decimal carry",
                     .opcode = 0x69,
                     .operands = {0x00},
                     .cycles = 3,
                     .initial = {.A = 0x99, .D = true, .C = true},
                     .expected = {.A = 0x00, .Z = true, .C = true}});
    cases.push_back({.name = "SBC BCD produces 99 when borrowing from zero",
                     .opcode = 0xE9,
                     .operands = {0x01},
                     .cycles = 3,
                     .initial = {.A = 0x00, .D = true, .C = true},
                     .expected = {.A = 0x99, .N = true, .C = false}});
    cases.push_back({.name = "SBC BCD subtracts packed decimal digits without borrow",
                     .opcode = 0xE9,
                     .operands = {0x49},
                     .cycles = 3,
                     .initial = {.A = 0x50, .D = true, .C = true},
                     .expected = {.A = 0x01, .C = true}});
    cases.push_back({.name = "ADC absolute X combines page and decimal cycle penalties",
                     .opcode = 0x7D,
                     .operands = {0xF8, 0x30},
                     .cycles = 6,
                     .initial = {.A = 0x10, .X = 0x10, .D = true},
                     .setup = [](TestMemory& memory) { memory.write(0x3108, 0x20); },
                     .expected = {.A = 0x30}});
    return cases;
}();

const InstructionCaseRegistration ARITHMETIC_REGISTRATION{"ADC/SBC", ARITHMETIC_CASES};

}  // namespace

TEST_CASE("ADC and SBC cover all modes and binary/decimal flag behavior",
          "[cpu][instruction][arithmetic]") {
    for (const auto& test : ARITHMETIC_CASES) {
        DYNAMIC_SECTION(test.name) { verifyInstruction(test); }
    }
}
