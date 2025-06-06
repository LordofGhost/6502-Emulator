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

    void callInstruction(Byte opCode);

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
                       "|----------|-------------|\n"
                       "| N (Negative) | " +
                       std::format("{:B}", N) + " |\n" + "| V (Overflow) | " +
                       std::format("{:B}", V) + " |\n" + "| B (Break) | " + std::format("{:B}", B) +
                       " |\n" + "| D (Decimal) | " + std::format("{:B}", D) + " |\n" +
                       "| I (Interrupt Disable) | " + std::format("{:B}", I) + " |\n" +
                       "| Z (Zero) | " + std::format("{:B}", Z) + " |\n" + "| C (Carry) |" +
                       std::format("{:B}", C) + " |\n";
            }
        } P;

        std::string toStringMD() const {
            return "## Registers\n"
                   "| Register | Value (bin) | Value (dec) | Value (hex)|\n"
                   "|----------|-------------|-------------|------------|\n"
                   "| A (Accumulator) | " +
                   std::format("{:B}", A) + " | " + std::to_string(A) + " | " +
                   std::format("{:X}", A) + " |\n" + "| X (X index) | " + std::format("{:B}", X) +
                   " | " + std::to_string(X) + " | " + std::format("{:X}", X) + " |\n" +
                   "| Y (Y index) | " + std::format("{:B}", Y) + " | " + std::to_string(Y) + " | " +
                   std::format("{:X}", Y) + " |\n" + "| S (Stack pointer) |" +
                   std::format("{:B}", S) + " | " + std::to_string(S) + " | " +
                   std::format("{:X}", S) + " |\n" + "| PC (Program Counter) | " +
                   std::format("{:B}", PC) + " | " + std::to_string(PC) + " | " +
                   std::format("{:X}", PC) + " |\n" + "| RW (Read/Write) | " +
                   std::format("{:B}", RW) + " | " + std::to_string(RW) + " | " +
                   std::format("{:X}", RW) + " |\n" + P.toStringMD();
        }

       private:
        W65C02& CPU;
    } registers;
};

#endif  // W65C02_H
