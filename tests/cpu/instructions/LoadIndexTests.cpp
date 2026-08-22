#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "support/W65C02TestHarness.h"

namespace {
using namespace cpu_test;

const std::vector<InstructionCase> LOAD_INDEX_CASES = {
    {.name = "LDX immediate loads X and clears N/Z",
     .opcode = 0xA2,
     .operands = {0x42},
     .cycles = 2,
     .initial = {.X = 0x80, .N = true, .Z = true},
     .expected = {.X = 0x42, .N = false, .Z = false}},
    {.name = "LDX zero page loads X and sets N",
     .opcode = 0xA6,
     .operands = {0x44},
     .cycles = 3,
     .setup = [](TestMemory& memory) { memory.write(0x0044, 0x80); },
     .expected = {.X = 0x80, .N = true}},
    {.name = "LDX zero page Y wraps within page zero",
     .opcode = 0xB6,
     .operands = {0xFE},
     .cycles = 4,
     .initial = {.Y = 0x05},
     .setup = [](TestMemory& memory) { memory.write(0x0003, 0x31); },
     .expected = {.X = 0x31}},
    {.name = "LDX absolute loads X",
     .opcode = 0xAE,
     .operands = {0x34, 0x12},
     .cycles = 4,
     .setup = [](TestMemory& memory) { memory.write(0x1234, 0x66); },
     .expected = {.X = 0x66}},
    {.name = "LDX absolute Y uses four cycles without page crossing",
     .opcode = 0xBE,
     .operands = {0x20, 0x12},
     .cycles = 4,
     .initial = {.Y = 0x0F},
     .setup = [](TestMemory& memory) { memory.write(0x122F, 0xA2); },
     .expected = {.X = 0xA2, .N = true}},
    {.name = "LDX absolute Y adds a cycle on page crossing",
     .opcode = 0xBE,
     .operands = {0xF8, 0x12},
     .cycles = 5,
     .initial = {.Y = 0x10},
     .setup = [](TestMemory& memory) { memory.write(0x1308, 0x43); },
     .expected = {.X = 0x43}},
    {.name = "LDY immediate loads Y and sets Z",
     .opcode = 0xA0,
     .operands = {0x00},
     .cycles = 2,
     .initial = {.Y = 0x80, .N = true},
     .expected = {.Y = 0x00, .N = false, .Z = true}},
    {.name = "LDY zero page loads Y and sets N",
     .opcode = 0xA4,
     .operands = {0x55},
     .cycles = 3,
     .setup = [](TestMemory& memory) { memory.write(0x0055, 0x81); },
     .expected = {.Y = 0x81, .N = true}},
    {.name = "LDY zero page X wraps within page zero",
     .opcode = 0xB4,
     .operands = {0xFC},
     .cycles = 4,
     .initial = {.X = 0x08},
     .setup = [](TestMemory& memory) { memory.write(0x0004, 0x52); },
     .expected = {.Y = 0x52}},
    {.name = "LDY absolute loads Y",
     .opcode = 0xAC,
     .operands = {0x78, 0x56},
     .cycles = 4,
     .setup = [](TestMemory& memory) { memory.write(0x5678, 0x73); },
     .expected = {.Y = 0x73}},
    {.name = "LDY absolute X uses four cycles without page crossing",
     .opcode = 0xBC,
     .operands = {0x40, 0x20},
     .cycles = 4,
     .initial = {.X = 0x0E},
     .setup = [](TestMemory& memory) { memory.write(0x204E, 0x92); },
     .expected = {.Y = 0x92, .N = true}},
    {.name = "LDY absolute X adds a cycle on page crossing",
     .opcode = 0xBC,
     .operands = {0xF0, 0x20},
     .cycles = 5,
     .initial = {.X = 0x20},
     .setup = [](TestMemory& memory) { memory.write(0x2110, 0x24); },
     .expected = {.Y = 0x24}},
};

const InstructionCaseRegistration LOAD_INDEX_REGISTRATION{"LDX/LDY", LOAD_INDEX_CASES};

}  // namespace

TEST_CASE("LDX and LDY support all official addressing modes", "[cpu][instruction][load]") {
    for (const auto& test : LOAD_INDEX_CASES) {
        DYNAMIC_SECTION(std::string(test.name)) { verifyInstruction(test); }
    }
}
