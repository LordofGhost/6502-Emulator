#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "support/Official6502Opcodes.h"
#include "support/W65C02TestHarness.h"

namespace {
using namespace cpu_test;

std::string_view modeName(const AddressingMode mode) {
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
            throw std::invalid_argument("Unsupported read addressing mode");
    }
}

InstructionCase makeReadCase(const OpcodeSpec& spec, InitialState initial,
                             const ExpectedState& expected, const Byte operand) {
    InstructionCase test = {
        .name = std::string(spec.mnemonic) + " " + std::string(modeName(spec.mode)),
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
            throw std::invalid_argument("Unsupported read addressing mode");
    }

    return test;
}

std::vector<InstructionCase> makeMnemonicCases(const std::string_view mnemonic,
                                                const InitialState& initial,
                                                const ExpectedState& expected,
                                                const Byte operand) {
    std::vector<InstructionCase> cases;
    for (const auto& spec : official6502Opcodes()) {
        if (spec.mnemonic == mnemonic) {
            cases.push_back(makeReadCase(spec, initial, expected, operand));
        }
    }
    return cases;
}

void append(std::vector<InstructionCase>& target, const std::vector<InstructionCase>& source) {
    target.insert(target.end(), source.begin(), source.end());
}

const std::vector<InstructionCase> LOGIC_COMPARE_CASES = [] {
    std::vector<InstructionCase> cases;
    append(cases, makeMnemonicCases("ORA", {.A = 0x40, .Z = true},
                                    {.A = 0xC1, .N = true, .Z = false}, 0x81));
    append(cases, makeMnemonicCases("AND", {.A = 0xF0, .N = true},
                                    {.A = 0x00, .N = false, .Z = true}, 0x0F));
    append(cases, makeMnemonicCases("EOR", {.A = 0xAA, .N = true, .Z = true},
                                    {.A = 0x55, .N = false, .Z = false}, 0xFF));
    append(cases, makeMnemonicCases("BIT", {.A = 0x0F},
                                    {.N = true, .V = true, .Z = true}, 0xC0));
    append(cases, makeMnemonicCases("CMP", {.A = 0x40, .N = true},
                                    {.N = false, .Z = true, .C = true}, 0x40));
    append(cases, makeMnemonicCases("CPX", {.X = 0x80, .N = true, .Z = true},
                                    {.N = false, .Z = false, .C = true}, 0x7F));
    append(cases, makeMnemonicCases("CPY", {.Y = 0x10, .Z = true, .C = true},
                                    {.N = true, .Z = false, .C = false}, 0x20));

    cases.push_back({.name = "CMP immediate reports a negative subtraction",
                     .opcode = 0xC9,
                     .operands = {0x01},
                     .cycles = 2,
                     .initial = {.A = 0x00},
                     .expected = {.N = true, .C = false}});
    cases.push_back({.name = "BIT absolute clears Z when A shares an operand bit",
                     .opcode = 0x2C,
                     .operands = {0x00, 0x30},
                     .cycles = 4,
                     .initial = {.A = 0x03, .Z = true},
                     .setup = [](TestMemory& memory) { memory.write(0x3000, 0xC3); },
                     .expected = {.N = true, .V = true, .Z = false}});
    cases.push_back({.name = "ORA absolute X adds a cycle on page crossing",
                     .opcode = 0x1D,
                     .operands = {0xF8, 0x30},
                     .cycles = 5,
                     .initial = {.A = 0x01, .X = 0x10},
                     .setup = [](TestMemory& memory) { memory.write(0x3108, 0x80); },
                     .expected = {.A = 0x81, .N = true}});
    return cases;
}();

const InstructionCaseRegistration LOGIC_COMPARE_REGISTRATION{"logic/compare",
                                                              LOGIC_COMPARE_CASES};

}  // namespace

TEST_CASE("Logic, bit and compare instructions support every official addressing mode",
          "[cpu][instruction][logic][compare]") {
    for (const auto& test : LOGIC_COMPARE_CASES) {
        DYNAMIC_SECTION(test.name) { verifyInstruction(test); }
    }
}
