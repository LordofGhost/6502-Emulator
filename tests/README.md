# CPU instruction tests

The suite separates reusable CPU execution from instruction-specific cases:

- `support/W65C02TestHarness.*` owns memory, reset, bus cycles and common assertions.
- `support/Official6502Opcodes.*` is the manifest of all 151 official opcode encodings.
- `cpu/instructions/*Tests.cpp` contains one data table per instruction family.
- `cpu/*Tests.cpp` contains dispatch and coverage checks that are not tied to one instruction.

Each `InstructionCase` declares its opcode, operands, exact cycle count, optional initial CPU
state, memory setup and expected changes. Registers and flags omitted from `expected` must remain
equal to `initial`. `PC` and `IR` are checked automatically, as is exact instruction completion.

Use one descriptive case for every addressing mode and add separate cases for timing or boundary
behavior such as page crossing and zero-page wraparound. Memory-writing instructions can use
`expectedMemory` to verify their result.

Every instruction table registers itself with `InstructionCaseRegistration`. The coverage test
rejects unknown opcodes, wrong operand sizes and incomplete coverage, and requires scenarios beyond
the 151-opcode minimum. The manifest follows the standard instruction table at
<https://www.masswerk.at/6502/6502_instruction_set.html>.
