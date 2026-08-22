#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "support/W65C02TestHarness.h"

namespace {
using namespace cpu_test;

const std::vector<InstructionCase> JUMP_SUBROUTINE_CASES = {
    {.name = "JMP absolute replaces PC with its operand",
     .opcode = 0x4C,
     .operands = {0x78, 0x56},
     .cycles = 3,
     .expected = {.PC = 0x5678}},
    {.name = "JMP indirect reads the target address",
     .opcode = 0x6C,
     .operands = {0x00, 0x30},
     .cycles = 5,
     .setup = [](TestMemory& memory) {
         memory.write(0x3000, 0x34);
         memory.write(0x3001, 0x12);
     },
     .expected = {.PC = 0x1234}},
    {.name = "JMP indirect reproduces the NMOS page-boundary wrap",
     .opcode = 0x6C,
     .operands = {0xFF, 0x30},
     .cycles = 5,
     .setup = [](TestMemory& memory) {
         memory.write(0x30FF, 0x78);
         memory.write(0x3000, 0x56);
         memory.write(0x3100, 0x99);
     },
     .expected = {.PC = 0x5678}},
    {.name = "JSR pushes the return address and jumps",
     .opcode = 0x20,
     .operands = {0x00, 0x40},
     .cycles = 6,
     .initial = {.S = 0xFF},
     .expected = {.S = 0xFD, .PC = 0x4000},
     .expectedMemory = {{0x01FF, 0x02}, {0x01FE, 0x02}}},
    {.name = "JSR wraps the stack pointer within page one",
     .opcode = 0x20,
     .operands = {0x34, 0x40},
     .cycles = 6,
     .initial = {.S = 0x00},
     .expected = {.S = 0xFE, .PC = 0x4034},
     .expectedMemory = {{0x0100, 0x02}, {0x01FF, 0x02}}},
    {.name = "RTS pulls the return address and advances it",
     .opcode = 0x60,
     .operands = {},
     .cycles = 6,
     .initial = {.S = 0xFD},
     .setup = [](TestMemory& memory) {
         memory.write(0x01FE, 0x34);
         memory.write(0x01FF, 0x12);
     },
     .expected = {.S = 0xFF, .PC = 0x1235}},
};

const InstructionCaseRegistration JUMP_SUBROUTINE_REGISTRATION{"jump/subroutine",
                                                               JUMP_SUBROUTINE_CASES};

}  // namespace

TEST_CASE("Jump and subroutine instructions preserve their control-flow contracts",
          "[cpu][instruction][jump][stack]") {
    for (const auto& test : JUMP_SUBROUTINE_CASES) {
        DYNAMIC_SECTION(test.name) { verifyInstruction(test); }
    }
}
