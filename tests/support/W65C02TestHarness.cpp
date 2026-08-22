#include "support/W65C02TestHarness.h"

#include <stdexcept>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "components/Bus.h"
#include "components/clock/CrystalOscillator.h"

Bus bus;
CrystalOscillator Clock;
std::vector<EmulatorException> EmulatorExceptions;

namespace cpu_test {
namespace {

constexpr Word RESET_VECTOR_LOW = 0xFFFC;
constexpr Word RESET_VECTOR_HIGH = 0xFFFD;
constexpr std::size_t RESET_CYCLES = 7;
constexpr std::size_t MAX_INSTRUCTION_CYCLES = 16;

void runCycle(W65C02& cpu, TestMemory& memory) {
    cpu.onClockCycle(Ph1);

    const bool reading = cpu.getRW();
    const Word address = bus.getAddress();
    if (reading) {
        bus.setData(memory.read(address));
    }

    cpu.onClockCycle(Ph2);
    if (!reading) {
        memory.write(address, bus.getData());
    }
}

Byte expectedByte(const std::optional<Byte>& expected, Byte initial) {
    return expected.value_or(initial);
}

bool expectedFlag(const std::optional<bool>& expected, bool initial) {
    return expected.value_or(initial);
}

}  // namespace

InstructionResult executeInstruction(const InstructionCase& test) {
    if (test.cycles == 0) {
        throw std::invalid_argument("An instruction must use at least one cycle");
    }

    EmulatorExceptions.clear();
    bus.setAddress(0);
    bus.setData(0);

    TestMemory memory;
    memory.write(RESET_VECTOR_LOW, static_cast<Byte>(PROGRAM_START & 0x00FF));
    memory.write(RESET_VECTOR_HIGH, static_cast<Byte>(PROGRAM_START >> 8));

    Word cursor = PROGRAM_START;
    memory.write(cursor++, test.opcode);
    for (const Byte operand : test.operands) {
        memory.write(cursor++, operand);
    }

    if (test.setup) {
        test.setup(memory);
    }

    W65C02 cpu;
    cpu.reset();
    for (std::size_t cycle = 0; cycle < RESET_CYCLES; ++cycle) {
        runCycle(cpu, memory);
    }
    cpu.setStateForTesting(test.initial);

    std::size_t instructionCycles = 0;
    do {
        runCycle(cpu, memory);
        ++instructionCycles;
        if (instructionCycles > MAX_INSTRUCTION_CYCLES) {
            throw std::runtime_error("Instruction did not finish within the cycle limit");
        }
    } while (cpu.hasPendingCyclesForTesting());

    return {
        cpu.getStateForTesting(), std::move(memory), instructionCycles,
    };
}

void verifyInstruction(const InstructionCase& test) {
    INFO("Instruction case: " << test.name);
    const InstructionResult result = executeInstruction(test);
    const auto& state = result.state;

    REQUIRE(result.cycles == test.cycles);

    CHECK(state.A == expectedByte(test.expected.A, test.initial.A));
    CHECK(state.X == expectedByte(test.expected.X, test.initial.X));
    CHECK(state.Y == expectedByte(test.expected.Y, test.initial.Y));
    CHECK(state.S == expectedByte(test.expected.S, test.initial.S));

    const Word defaultPC = static_cast<Word>(PROGRAM_START + 1 + test.operands.size());
    CHECK(state.PC == test.expected.PC.value_or(defaultPC));
    CHECK(state.IR == test.expected.IR.value_or(test.opcode));

    CHECK(state.N == expectedFlag(test.expected.N, test.initial.N));
    CHECK(state.V == expectedFlag(test.expected.V, test.initial.V));
    CHECK(state.D == expectedFlag(test.expected.D, test.initial.D));
    CHECK(state.I == expectedFlag(test.expected.I, test.initial.I));
    CHECK(state.Z == expectedFlag(test.expected.Z, test.initial.Z));
    CHECK(state.C == expectedFlag(test.expected.C, test.initial.C));

    if (test.expected.ADL) CHECK(state.ADL == *test.expected.ADL);
    if (test.expected.ADH) CHECK(state.ADH == *test.expected.ADH);
    if (test.expected.RW) CHECK(state.RW == *test.expected.RW);

    for (const auto& expectation : test.expectedMemory) {
        CHECK(result.memory.read(expectation.address) == expectation.value);
    }
}

const std::vector<EmulatorException>& recordedExceptions() { return EmulatorExceptions; }

}  // namespace cpu_test
