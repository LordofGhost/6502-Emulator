#include <string>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "support/W65C02TestHarness.h"

namespace {
using namespace cpu_test;

InitialState branchState(const Byte opcode, const bool taken) {
    InitialState state;
    switch (opcode) {
        case 0x10:
            state.N = !taken;
            break;
        case 0x30:
            state.N = taken;
            break;
        case 0x50:
            state.V = !taken;
            break;
        case 0x70:
            state.V = taken;
            break;
        case 0x90:
            state.C = !taken;
            break;
        case 0xB0:
            state.C = taken;
            break;
        case 0xD0:
            state.Z = !taken;
            break;
        case 0xF0:
            state.Z = taken;
            break;
        default:
            break;
    }
    return state;
}

const std::vector<InstructionCase> BRANCH_CASES = [] {
    const std::vector<std::pair<Byte, std::string>> branches = {
        {0x10, "BPL"}, {0x30, "BMI"}, {0x50, "BVC"}, {0x70, "BVS"},
        {0x90, "BCC"}, {0xB0, "BCS"}, {0xD0, "BNE"}, {0xF0, "BEQ"},
    };

    std::vector<InstructionCase> cases;
    for (const auto& [opcode, mnemonic] : branches) {
        cases.push_back({.name = mnemonic + " branches when its condition is true",
                         .opcode = opcode,
                         .operands = {0x05},
                         .cycles = 3,
                         .initial = branchState(opcode, true),
                         .expected = {.PC = 0x0207}});
        cases.push_back({.name = mnemonic + " continues when its condition is false",
                         .opcode = opcode,
                         .operands = {0x05},
                         .cycles = 2,
                         .initial = branchState(opcode, false)});
    }

    cases.push_back({.name = "BCC adds a cycle for a positive page crossing",
                     .opcode = 0x90,
                     .operands = {0x20},
                     .cycles = 4,
                     .programStart = 0x02F0,
                     .expected = {.PC = 0x0312}});
    cases.push_back({.name = "BNE sign-extends a negative offset across a page",
                     .opcode = 0xD0,
                     .operands = {0xFC},
                     .cycles = 4,
                     .programStart = 0x0200,
                     .expected = {.PC = 0x01FE}});
    return cases;
}();

const InstructionCaseRegistration BRANCH_REGISTRATION{"branches", BRANCH_CASES};

}  // namespace

TEST_CASE("Every conditional branch handles taken and not-taken paths",
          "[cpu][instruction][branch]") {
    for (const auto& test : BRANCH_CASES) {
        DYNAMIC_SECTION(test.name) { verifyInstruction(test); }
    }
}
