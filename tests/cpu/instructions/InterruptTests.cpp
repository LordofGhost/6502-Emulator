#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "support/W65C02TestHarness.h"

namespace {
using namespace cpu_test;

const std::vector<InstructionCase> INTERRUPT_CASES = {
    {.name = "BRK pushes the return address and status before loading the IRQ vector",
     .opcode = 0x00,
     .operands = {},
     .cycles = 7,
     .initial = {
         .S = 0xFF,
         .N = true,
         .V = true,
         .D = true,
         .I = false,
         .Z = true,
         .C = true,
     },
     .setup = [](TestMemory& memory) {
         memory.write(0xFFFE, 0x78);
         memory.write(0xFFFF, 0x56);
     },
     .expected = {.S = 0xFC, .PC = 0x5678, .D = false, .I = true},
     .expectedMemory = {{0x01FF, 0x02}, {0x01FE, 0x02}, {0x01FD, 0xFB}}},
    {.name = "BRK wraps stack writes within page one",
     .opcode = 0x00,
     .operands = {},
     .cycles = 7,
     .initial = {.S = 0x01, .I = false},
     .setup = [](TestMemory& memory) {
         memory.write(0xFFFE, 0x00);
         memory.write(0xFFFF, 0x80);
     },
     .expected = {.S = 0xFE, .PC = 0x8000, .I = true},
     .expectedMemory = {{0x0101, 0x02}, {0x0100, 0x02}, {0x01FF, 0x30}}},
    {.name = "RTI restores status and program counter from the stack",
     .opcode = 0x40,
     .operands = {},
     .cycles = 6,
     .initial = {.S = 0xFC, .I = true},
     .setup = [](TestMemory& memory) {
         memory.write(0x01FD, 0xCB);
         memory.write(0x01FE, 0x34);
         memory.write(0x01FF, 0x12);
     },
     .expected = {
         .S = 0xFF,
         .PC = 0x1234,
         .N = true,
         .V = true,
         .D = true,
         .I = false,
         .Z = true,
         .C = true,
     }},
    {.name = "RTI ignores reserved and break status bits",
     .opcode = 0x40,
     .operands = {},
     .cycles = 6,
     .initial = {.S = 0xFC, .N = true, .V = true, .D = true, .I = true, .Z = true, .C = true},
     .setup = [](TestMemory& memory) {
         memory.write(0x01FD, 0x30);
         memory.write(0x01FE, 0x00);
         memory.write(0x01FF, 0x40);
     },
     .expected = {
         .S = 0xFF,
         .PC = 0x4000,
         .N = false,
         .V = false,
         .D = false,
         .I = false,
         .Z = false,
         .C = false,
     }},
};

const InstructionCaseRegistration INTERRUPT_REGISTRATION{"interrupt", INTERRUPT_CASES};

}  // namespace

TEST_CASE("Software interrupt instructions preserve stack and status semantics",
          "[cpu][instruction][interrupt]") {
    for (const auto& test : INTERRUPT_CASES) {
        DYNAMIC_SECTION(test.name) { verifyInstruction(test); }
    }
}
