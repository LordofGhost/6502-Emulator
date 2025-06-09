#ifndef W65C02_H
#define W65C02_H

#define READ true
#define WRITE false

#include <format>
#include <functional>
#include <list>
#include <map>
#include <queue>

#include "../../Main.h"
#include "../Component.h"

/* CPU
 * Diagram: https://davidmjc.github.io/6502/bd.svg
 * Datasheet: https://eater.net/datasheets/w65c02s.pdf
 * Instruction Set: https://www.masswerk.at/6502/6502_instruction_set.html
 *                  https://www.pagetable.com/c64ref/6502
 */

class W65C02 : public Component {
   public:
    W65C02() : registers(*this) {}

    // This methode represents the reset button
    void reset() noexcept;

    // make RW flag visible to memory
    [[nodiscard]] bool getRW() const { return registers.RW; }  // make RW flag visible to memory

    void onClockCycle(Phase phase) override;
    std::string toStringMD() const;

   private:
    void fetch();

    // This queue consist of arrays with 2 Elements, these represent the Ph1 and Ph2
    std::queue<std::array<std::function<void()>, 2>,
               std::list<std::array<std::function<void()>, 2>>>
        callQueue;

    void callInstruction();

    struct Registers {
        // This reference is needed to be able to access non-static members of W65C02
        explicit Registers(W65C02& parent) : CPU(parent) {}

        Byte A;  // Accumulator

        // Index registers
        Byte X;  // X index
        Byte Y;  // Y index
        Byte S;  // Stack pointer

        Word PC;  // Program Counter

        bool RW;  // true = Read; false = Write

        Byte IR;  // Instruction Register

        // Processor flags NV-BDIZC (7-0)
        struct s_P {
            bool N;               // Negative
            bool V;               // Overflow
            const bool B = true;  // Break (always set)
            bool D;               // Decimal
            bool I;               // Interrupt Disable
            bool Z;               // Zero
            bool C;               // Carry

            Byte getByte() const {
                return (N << 7) + (V << 6) + (B << 4) + (D << 3) + (I << 2) + (Z << 1) + C;
            }

            std::string toStringMD() const {
                return "### Processor flags\n"
                       "| Register | Value (bin) |\n"
                       "|----------|-------------|\n" +
                       std::format("| N (Negative)          | {:B} |\n", N) +
                       std::format("| V (Overflow)          | {:B} |\n", V) +
                       std::format("| B (Break)             | {:B} |\n", B) +
                       std::format("| D (Decimal)           | {:B} |\n", D) +
                       std::format("| I (Interrupt Disable) | {:B} |\n", I) +
                       std::format("| Z (Zero)              | {:B} |\n", Z) +
                       std::format("| C (Carry)             | {:B} |\n", C);
            }
        } P;

        std::string toStringMD() const {
            return "## Registers\n"
                   "| Register | Value (bin) | Value (dec) | Value (hex)|\n"
                   "|----------|-------------|-------------|------------|\n" +
                   std::format("| A (Accumulator)           | {:B} | {:d} | {:X} |\n", A, A, A) +
                   std::format("| X (X index)               | {:B} | {:d} | {:X} |\n", X, X, X) +
                   std::format("| Y (Y index)               | {:B} | {:d} | {:X} |\n", Y, Y, Y) +
                   std::format("| S (Stack pointer)         | {:B} | {:d} | {:X} |\n", S, S, S) +
                   std::format("| PC (Program Counter)      | {:B} | {:d} | {:X} |\n", PC, PC, PC) +
                   std::format("| RW (Read/Write)           | {:B} | {:d} | {:X} |\n", RW, RW, RW) +
                   std::format("| IR (Instruction Register) | {:B} | {:d} | {:X} |\n", IR, IR, IR) +
                   P.toStringMD();
        }

       private:
        W65C02& CPU;
    } registers;
};

#endif  // W65C02_H
