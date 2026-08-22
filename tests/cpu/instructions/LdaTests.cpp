#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "support/W65C02TestHarness.h"

namespace {
using namespace cpu_test;

const std::vector<InstructionCase> LDA_CASES = {
    {
        .name = "LDA immediate loads a positive value and clears Z/N",
        .opcode = 0xA9,
        .operands = {0x42},
        .cycles = 2,
        .initial = {.N = true, .Z = true},
        .expected = {.A = 0x42, .N = false, .Z = false},
    },
    {
        .name = "LDA immediate sets Z for zero",
        .opcode = 0xA9,
        .operands = {0x00},
        .cycles = 2,
        .initial = {.N = true},
        .expected = {.A = 0x00, .N = false, .Z = true},
    },
    {
        .name = "LDA immediate sets N for bit 7",
        .opcode = 0xA9,
        .operands = {0x80},
        .cycles = 2,
        .initial = {.Z = true},
        .expected = {.A = 0x80, .N = true, .Z = false},
    },
    {
        .name = "LDA zero page reads the addressed byte",
        .opcode = 0xA5,
        .operands = {0x44},
        .cycles = 3,
        .setup = [](TestMemory& memory) { memory.write(0x0044, 0x7E); },
        .expected = {.A = 0x7E},
    },
    {
        .name = "LDA zero page X wraps within page zero",
        .opcode = 0xB5,
        .operands = {0xFE},
        .cycles = 4,
        .initial = {.X = 0x05},
        .setup = [](TestMemory& memory) { memory.write(0x0003, 0x91); },
        .expected = {.A = 0x91, .N = true},
    },
    {
        .name = "LDA absolute reads the addressed byte",
        .opcode = 0xAD,
        .operands = {0x34, 0x12},
        .cycles = 4,
        .setup = [](TestMemory& memory) { memory.write(0x1234, 0x66); },
        .expected = {.A = 0x66},
    },
    {
        .name = "LDA absolute X uses four cycles without page crossing",
        .opcode = 0xBD,
        .operands = {0x20, 0x12},
        .cycles = 4,
        .initial = {.X = 0x0F},
        .setup = [](TestMemory& memory) { memory.write(0x122F, 0xA2); },
        .expected = {.A = 0xA2, .N = true},
    },
    {
        .name = "LDA absolute X adds a cycle on page crossing",
        .opcode = 0xBD,
        .operands = {0xF8, 0x12},
        .cycles = 5,
        .initial = {.X = 0x10},
        .setup = [](TestMemory& memory) { memory.write(0x1308, 0xA3); },
        .expected = {.A = 0xA3, .N = true},
    },
    {
        .name = "LDA absolute Y uses four cycles without page crossing",
        .opcode = 0xB9,
        .operands = {0x30, 0x20},
        .cycles = 4,
        .initial = {.Y = 0x0E},
        .setup = [](TestMemory& memory) { memory.write(0x203E, 0xB2); },
        .expected = {.A = 0xB2, .N = true},
    },
    {
        .name = "LDA absolute Y adds a cycle on page crossing",
        .opcode = 0xB9,
        .operands = {0xF4, 0x20},
        .cycles = 5,
        .initial = {.Y = 0x20},
        .setup = [](TestMemory& memory) { memory.write(0x2114, 0xB3); },
        .expected = {.A = 0xB3, .N = true},
    },
    {
        .name = "LDA indexed indirect resolves the zero page pointer",
        .opcode = 0xA1,
        .operands = {0x20},
        .cycles = 6,
        .initial = {.X = 0x04},
        .setup = [](TestMemory& memory) {
            memory.write(0x0024, 0x78);
            memory.write(0x0025, 0x56);
            memory.write(0x5678, 0xAB);
        },
        .expected = {.A = 0xAB, .N = true},
    },
    {
        .name = "LDA indirect indexed uses five cycles without page crossing",
        .opcode = 0xB1,
        .operands = {0x30},
        .cycles = 5,
        .initial = {.Y = 0x0A},
        .setup = [](TestMemory& memory) {
            memory.write(0x0030, 0x40);
            memory.write(0x0031, 0x20);
            memory.write(0x204A, 0xBC);
        },
        .expected = {.A = 0xBC, .N = true},
    },
    {
        .name = "LDA indirect indexed adds a cycle on page crossing",
        .opcode = 0xB1,
        .operands = {0x30},
        .cycles = 6,
        .initial = {.Y = 0x20},
        .setup = [](TestMemory& memory) {
            memory.write(0x0030, 0xF0);
            memory.write(0x0031, 0x20);
            memory.write(0x2110, 0xBD);
        },
        .expected = {.A = 0xBD, .N = true},
    },
};

}  // namespace

TEST_CASE("LDA supports all implemented addressing modes", "[cpu][instruction][lda]") {
    for (const auto& test : LDA_CASES) {
        DYNAMIC_SECTION(std::string(test.name)) { verifyInstruction(test); }
    }
}
