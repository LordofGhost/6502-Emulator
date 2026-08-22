#ifndef OFFICIAL_6502_OPCODES_H
#define OFFICIAL_6502_OPCODES_H

#include <cstddef>
#include <span>
#include <string_view>

#include "Main.h"

namespace cpu_test {

enum class AddressingMode {
    Accumulator,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    Immediate,
    Implied,
    Indirect,
    IndexedIndirect,
    IndirectIndexed,
    Relative,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
};

struct OpcodeSpec {
    Byte opcode;
    std::string_view mnemonic;
    AddressingMode mode;
};

constexpr std::size_t OFFICIAL_OPCODE_COUNT = 151;
constexpr std::size_t OFFICIAL_MNEMONIC_COUNT = 56;

[[nodiscard]] std::span<const OpcodeSpec> official6502Opcodes();
[[nodiscard]] std::size_t operandSize(AddressingMode mode);

}  // namespace cpu_test

#endif  // OFFICIAL_6502_OPCODES_H
