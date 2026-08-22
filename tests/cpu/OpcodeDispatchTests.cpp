#include <string>

#include <catch2/catch_test_macros.hpp>

#include "support/W65C02TestHarness.h"

TEST_CASE("Unknown opcodes throw an emulator exception", "[cpu][opcode]") {
    const cpu_test::InstructionCase unknownOpcode = {
        .name = "unknown opcode FF",
        .opcode = 0xFF,
        .operands = {},
        .cycles = 1,
    };

    REQUIRE_THROWS_AS(cpu_test::executeInstruction(unknownOpcode), EmulatorException);
    REQUIRE(cpu_test::recordedExceptions().size() == 1);
    CHECK(cpu_test::recordedExceptions().back().toString().find("Code: 1400") !=
          std::string::npos);
}
