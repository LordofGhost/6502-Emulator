#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "support/W65C02TestHarness.h"

namespace {
using namespace cpu_test;

const std::vector<InstructionCase> STACK_CASES = {
    {.name = "PHA pushes A and decrements S",
     .opcode = 0x48,
     .operands = {},
     .cycles = 3,
     .initial = {.A = 0xAB, .S = 0xFF, .N = true, .C = true},
     .expected = {.S = 0xFE},
     .expectedMemory = {{0x01FF, 0xAB}}},
    {.name = "PHA wraps S within stack page one",
     .opcode = 0x48,
     .operands = {},
     .cycles = 3,
     .initial = {.A = 0xCD, .S = 0x00},
     .expected = {.S = 0xFF},
     .expectedMemory = {{0x0100, 0xCD}}},
    {.name = "PHP pushes all flags with reserved and break bits set",
     .opcode = 0x08,
     .operands = {},
     .cycles = 3,
     .initial = {.S = 0xFF, .N = true, .V = true, .D = true, .I = true, .Z = true, .C = true},
     .expected = {.S = 0xFE},
     .expectedMemory = {{0x01FF, 0xFF}}},
    {.name = "PLA pulls A and updates N/Z",
     .opcode = 0x68,
     .operands = {},
     .cycles = 4,
     .initial = {.A = 0x00, .S = 0xFE, .Z = true},
     .setup = [](TestMemory& memory) { memory.write(0x01FF, 0x80); },
     .expected = {.A = 0x80, .S = 0xFF, .N = true, .Z = false}},
    {.name = "PLA sets Z when pulling zero",
     .opcode = 0x68,
     .operands = {},
     .cycles = 4,
     .initial = {.A = 0x80, .S = 0xFE, .N = true},
     .setup = [](TestMemory& memory) { memory.write(0x01FF, 0x00); },
     .expected = {.A = 0x00, .S = 0xFF, .N = false, .Z = true}},
    {.name = "PLP restores implemented status flags and ignores B",
     .opcode = 0x28,
     .operands = {},
     .cycles = 4,
     .initial = {.S = 0xFE, .I = true},
     .setup = [](TestMemory& memory) { memory.write(0x01FF, 0xCB); },
     .expected = {
         .S = 0xFF,
         .N = true,
         .V = true,
         .D = true,
         .I = false,
         .Z = true,
         .C = true,
     }},
};

const InstructionCaseRegistration STACK_REGISTRATION{"stack", STACK_CASES};

}  // namespace

TEST_CASE("Stack transfer instructions preserve stack and flag semantics",
          "[cpu][instruction][stack]") {
    for (const auto& test : STACK_CASES) {
        DYNAMIC_SECTION(test.name) { verifyInstruction(test); }
    }
}
