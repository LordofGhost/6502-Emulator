#ifndef W65C02_TEST_HARNESS_H
#define W65C02_TEST_HARNESS_H

#include <array>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "components/cpu/W65C02.h"
#include "exceptions/EmulatorException.h"

namespace cpu_test {

constexpr Word PROGRAM_START = 0x0200;

class TestMemory {
   public:
    [[nodiscard]] Byte read(Word address) const { return bytes[address]; }
    void write(Word address, Byte value) { bytes[address] = value; }

   private:
    std::array<Byte, 0x10000> bytes{};
};

using MemorySetup = std::function<void(TestMemory&)>;
using InitialState = W65C02::TestInitialState;

struct ExpectedState {
    std::optional<Byte> A;
    std::optional<Byte> X;
    std::optional<Byte> Y;
    std::optional<Byte> S;
    std::optional<Word> PC;
    std::optional<Byte> IR;
    std::optional<Byte> ADL;
    std::optional<Byte> ADH;
    std::optional<bool> RW;
    std::optional<bool> N;
    std::optional<bool> V;
    std::optional<bool> D;
    std::optional<bool> I;
    std::optional<bool> Z;
    std::optional<bool> C;
};

struct MemoryExpectation {
    Word address;
    Byte value;
};

struct InstructionCase {
    std::string name;
    Byte opcode;
    std::vector<Byte> operands;
    std::size_t cycles;
    Word programStart = PROGRAM_START;
    InitialState initial{};
    MemorySetup setup{};
    ExpectedState expected{};
    std::vector<MemoryExpectation> expectedMemory{};
};

struct InstructionResult {
    W65C02::TestState state;
    TestMemory memory;
    std::size_t cycles;
};

struct InstructionCaseGroup {
    std::string_view name;
    const std::vector<InstructionCase>* cases;
};

class InstructionCaseRegistration {
   public:
    InstructionCaseRegistration(std::string_view name,
                                const std::vector<InstructionCase>& cases);
};

InstructionResult executeInstruction(const InstructionCase& test);
void verifyInstruction(const InstructionCase& test);
[[nodiscard]] const std::vector<InstructionCaseGroup>& registeredInstructionCases();
[[nodiscard]] const std::vector<EmulatorException>& recordedExceptions();

}  // namespace cpu_test

#endif  // W65C02_TEST_HARNESS_H
