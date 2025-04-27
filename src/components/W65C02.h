#ifndef W65C02_H
#define W65C02_H

#include <iostream>

#include "../Main.h"

/* CPU
 * Diagram: https://davidmjc.github.io/6502/bd.svg
 */

class W65C02 {
   public:
    Byte dataBus;
    Word addressBus;

   private:
    // Main registers
    Byte A;  // Accumulator

    // Index registers
    Byte X;  // X index
    Byte Y;  // Y index
    Byte S;  // Stack pointer

    Word PC;  // Program Counter

    Byte P;  // Processor flags
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

    void decodeLogic(Byte IR);  // IR = Instruction Register

   public:
    void reset();
};

#endif  // W65C02_H
