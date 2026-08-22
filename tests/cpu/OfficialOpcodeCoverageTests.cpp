#include <algorithm>
#include <array>
#include <set>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "support/Official6502Opcodes.h"
#include "support/W65C02TestHarness.h"

namespace {

const cpu_test::OpcodeSpec* findOpcode(const Byte opcode) {
    for (const auto& spec : cpu_test::official6502Opcodes()) {
        if (spec.opcode == opcode) return &spec;
    }
    return nullptr;
}

}  // namespace

TEST_CASE("The official 6502 opcode manifest is complete", "[cpu][coverage]") {
    const auto specs = cpu_test::official6502Opcodes();
    REQUIRE(specs.size() == cpu_test::OFFICIAL_OPCODE_COUNT);

    std::array<bool, 0x100> seenOpcodes{};
    std::set<std::string_view> mnemonics;
    for (const auto& spec : specs) {
        INFO("Opcode: " << static_cast<unsigned int>(spec.opcode));
        REQUIRE_FALSE(seenOpcodes[spec.opcode]);
        seenOpcodes[spec.opcode] = true;
        mnemonics.insert(spec.mnemonic);
    }

    CHECK(mnemonics.size() == cpu_test::OFFICIAL_MNEMONIC_COUNT);
}

TEST_CASE("Registered instruction cases match the official opcode manifest", "[cpu][coverage]") {
    std::array<bool, 0x100> coveredOpcodes{};
    std::size_t scenarioCount = 0;

    for (const auto& group : cpu_test::registeredInstructionCases()) {
        INFO("Instruction group: " << group.name);
        REQUIRE(group.cases != nullptr);
        REQUIRE_FALSE(group.cases->empty());
        scenarioCount += group.cases->size();

        for (const auto& test : *group.cases) {
            INFO("Instruction case: " << test.name);
            const auto* spec = findOpcode(test.opcode);
            REQUIRE(spec != nullptr);
            CHECK(test.operands.size() == cpu_test::operandSize(spec->mode));
            coveredOpcodes[test.opcode] = true;
        }
    }

    const std::size_t coveredCount =
        static_cast<std::size_t>(std::count(coveredOpcodes.begin(), coveredOpcodes.end(), true));
    CHECK(coveredCount == cpu_test::OFFICIAL_OPCODE_COUNT);
    CHECK(scenarioCount > cpu_test::OFFICIAL_OPCODE_COUNT);
}
