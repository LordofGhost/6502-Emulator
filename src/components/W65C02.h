#ifndef W65C02_H
#define W65C02_H

#include <iostream>

// CPU

class W65C02 {
public:
    std::uint8_t dataBus;
    std::uint16_t addressBus;

private:
    // Main registers
    std::uint8_t A; // Accumulator

    // Index registers
    std::uint8_t X; // X index
    std::uint8_t Y; // Y index
    std::uint8_t S; // Stack pointer

    std::uint16_t PC; // Program Counter

    std::uint8_t P; // Processor flags
    /* NV-BDIZC (7-0)
     * N = Negative
     * V = Overflow
     * - = (always set)
     * B = Break (always set)
     * D = Decimal
     * I = Interrupt Disable
     * Z = Zero
     * C = Carry
     * Source: https://www.nesdev.org/wiki/Status_flags */
};

#endif  // W65C02_H
