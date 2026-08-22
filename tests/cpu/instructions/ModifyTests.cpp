#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "support/Official6502Opcodes.h"
#include "support/W65C02TestHarness.h"

namespace {
using namespace cpu_test;

std::string_view modifyModeName(const AddressingMode mode) {
    switch (mode) {
        case AddressingMode::Accumulator:
            return "accumulator";
        case AddressingMode::ZeroPage:
            return "zero page";
        case AddressingMode::ZeroPageX:
            return "zero page X";
        case AddressingMode::Absolute:
            return "absolute";
        case AddressingMode::AbsoluteX:
            return "absolute X";
        default:
            throw std::invalid_argument("Unsupported modify addressing mode");
    }
}

InstructionCase makeModifyCase(const OpcodeSpec& spec, InitialState initial,
                               ExpectedState expected, const Byte operand,
                               const Byte result) {
    InstructionCase test = {
        .name = std::string(spec.mnemonic) + " " + std::string(modifyModeName(spec.mode)),
        .opcode = spec.opcode,
        .operands = {},
        .cycles = 0,
        .initial = initial,
        .expected = expected,
    };

    switch (spec.mode) {
        case AddressingMode::Accumulator:
            test.cycles = 2;
            test.initial.A = operand;
            test.expected.A = result;
            break;
        case AddressingMode::ZeroPage:
            test.operands = {0x44};
            test.cycles = 5;
            test.setup = [operand](TestMemory& memory) { memory.write(0x0044, operand); };
            test.expectedMemory = {{0x0044, result}};
            break;
        case AddressingMode::ZeroPageX:
            test.operands = {0xFC};
            test.cycles = 6;
            test.initial.X = 0x08;
            test.setup = [operand](TestMemory& memory) { memory.write(0x0004, operand); };
            test.expectedMemory = {{0x0004, result}};
            break;
        case AddressingMode::Absolute:
            test.operands = {0x34, 0x12};
            test.cycles = 6;
            test.setup = [operand](TestMemory& memory) { memory.write(0x1234, operand); };
            test.expectedMemory = {{0x1234, result}};
            break;
        case AddressingMode::AbsoluteX:
            test.operands = {0xF8, 0x30};
            test.cycles = 7;
            test.initial.X = 0x10;
            test.setup = [operand](TestMemory& memory) { memory.write(0x3108, operand); };
            test.expectedMemory = {{0x3108, result}};
            break;
        default:
            throw std::invalid_argument("Unsupported modify addressing mode");
    }
    return test;
}

std::vector<InstructionCase> makeModifyCases(const std::string_view mnemonic,
                                              const InitialState& initial,
                                              const ExpectedState& expected,
                                              const Byte operand, const Byte result) {
    std::vector<InstructionCase> cases;
    for (const auto& spec : official6502Opcodes()) {
        if (spec.mnemonic == mnemonic) {
            cases.push_back(makeModifyCase(spec, initial, expected, operand, result));
        }
    }
    return cases;
}

void appendModify(std::vector<InstructionCase>& target,
                  const std::vector<InstructionCase>& source) {
    target.insert(target.end(), source.begin(), source.end());
}

const std::vector<InstructionCase> MODIFY_CASES = [] {
    std::vector<InstructionCase> cases;
    appendModify(cases, makeModifyCases("ASL", {.Z = true},
                                        {.N = false, .Z = false, .C = true}, 0x81, 0x02));
    appendModify(cases, makeModifyCases("LSR", {.N = true},
                                        {.N = false, .Z = true, .C = true}, 0x01, 0x00));
    appendModify(cases, makeModifyCases("ROL", {.C = true},
                                        {.N = false, .Z = false, .C = true}, 0x80, 0x01));
    appendModify(cases, makeModifyCases("ROR", {.C = true},
                                        {.N = true, .Z = false, .C = true}, 0x01, 0x80));
    appendModify(cases, makeModifyCases("DEC", {.A = 0x42, .Z = true, .C = true},
                                        {.N = true, .Z = false}, 0x00, 0xFF));
    appendModify(cases, makeModifyCases("INC", {.A = 0x42, .N = true, .C = true},
                                        {.N = false, .Z = true}, 0xFF, 0x00));

    cases.push_back({.name = "ROL accumulator shifts in a clear carry",
                     .opcode = 0x2A,
                     .operands = {},
                     .cycles = 2,
                     .initial = {.A = 0x01, .C = false},
                     .expected = {.A = 0x02, .C = false}});
    cases.push_back({.name = "ROR accumulator shifts in a clear carry",
                     .opcode = 0x6A,
                     .operands = {},
                     .cycles = 2,
                     .initial = {.A = 0x02, .C = false},
                     .expected = {.A = 0x01, .C = false}});
    return cases;
}();

const InstructionCaseRegistration MODIFY_REGISTRATION{"shift/rotate/inc/dec", MODIFY_CASES};

}  // namespace

TEST_CASE("Shift, rotate, increment and decrement cover every official opcode",
          "[cpu][instruction][modify]") {
    for (const auto& test : MODIFY_CASES) {
        DYNAMIC_SECTION(test.name) { verifyInstruction(test); }
    }
}
