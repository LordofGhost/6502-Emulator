#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "support/W65C02TestHarness.h"

namespace {
using namespace cpu_test;

const std::vector<InstructionCase> STORE_CASES = {
    {.name = "STA indexed indirect wraps the zero page pointer",
     .opcode = 0x81,
     .operands = {0xFE},
     .cycles = 6,
     .initial = {.A = 0xA1, .X = 0x04, .N = true, .C = true},
     .setup = [](TestMemory& memory) {
         memory.write(0x0002, 0x34);
         memory.write(0x0003, 0x12);
     },
     .expectedMemory = {{0x1234, 0xA1}}},
    {.name = "STA zero page writes A",
     .opcode = 0x85,
     .operands = {0x44},
     .cycles = 3,
     .initial = {.A = 0xA2},
     .expectedMemory = {{0x0044, 0xA2}}},
    {.name = "STA zero page X wraps within page zero",
     .opcode = 0x95,
     .operands = {0xFC},
     .cycles = 4,
     .initial = {.A = 0xA3, .X = 0x08},
     .expectedMemory = {{0x0004, 0xA3}}},
    {.name = "STA absolute writes A",
     .opcode = 0x8D,
     .operands = {0x78, 0x56},
     .cycles = 4,
     .initial = {.A = 0xA4},
     .expectedMemory = {{0x5678, 0xA4}}},
    {.name = "STA absolute X always uses five cycles across a page",
     .opcode = 0x9D,
     .operands = {0xF8, 0x20},
     .cycles = 5,
     .initial = {.A = 0xA5, .X = 0x10},
     .expectedMemory = {{0x2108, 0xA5}}},
    {.name = "STA absolute Y writes the indexed address",
     .opcode = 0x99,
     .operands = {0x30, 0x20},
     .cycles = 5,
     .initial = {.A = 0xA6, .Y = 0x0F},
     .expectedMemory = {{0x203F, 0xA6}}},
    {.name = "STA indirect indexed wraps pointer lookup and crosses a page",
     .opcode = 0x91,
     .operands = {0xFF},
     .cycles = 6,
     .initial = {.A = 0xA7, .Y = 0x20},
     .setup = [](TestMemory& memory) {
         memory.write(0x00FF, 0xF0);
         memory.write(0x0000, 0x30);
     },
     .expectedMemory = {{0x3110, 0xA7}}},
    {.name = "STX zero page writes X",
     .opcode = 0x86,
     .operands = {0x45},
     .cycles = 3,
     .initial = {.X = 0xB1},
     .expectedMemory = {{0x0045, 0xB1}}},
    {.name = "STX zero page Y wraps within page zero",
     .opcode = 0x96,
     .operands = {0xFD},
     .cycles = 4,
     .initial = {.X = 0xB2, .Y = 0x08},
     .expectedMemory = {{0x0005, 0xB2}}},
    {.name = "STX absolute writes X",
     .opcode = 0x8E,
     .operands = {0x34, 0x22},
     .cycles = 4,
     .initial = {.X = 0xB3},
     .expectedMemory = {{0x2234, 0xB3}}},
    {.name = "STY zero page writes Y",
     .opcode = 0x84,
     .operands = {0x46},
     .cycles = 3,
     .initial = {.Y = 0xC1},
     .expectedMemory = {{0x0046, 0xC1}}},
    {.name = "STY zero page X wraps within page zero",
     .opcode = 0x94,
     .operands = {0xFA},
     .cycles = 4,
     .initial = {.X = 0x0A, .Y = 0xC2},
     .expectedMemory = {{0x0004, 0xC2}}},
    {.name = "STY absolute writes Y",
     .opcode = 0x8C,
     .operands = {0x56, 0x23},
     .cycles = 4,
     .initial = {.Y = 0xC3},
     .expectedMemory = {{0x2356, 0xC3}}},
};

const InstructionCaseRegistration STORE_REGISTRATION{"STA/STX/STY", STORE_CASES};

}  // namespace

TEST_CASE("Store instructions support every official addressing mode", "[cpu][instruction][store]") {
    for (const auto& test : STORE_CASES) {
        DYNAMIC_SECTION(std::string(test.name)) { verifyInstruction(test); }
    }
}
