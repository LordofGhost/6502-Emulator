#ifndef W65C02_H
#define W65C02_H

#include <iostream>

#include "../Exception.h"
#include "../Main.h"

/* CPU
 * Diagram: https://davidmjc.github.io/6502/bd.svg
 */

class W65C02 {
   public:
    W65C02() : bus(*this), registers(*this), instructions(*this) {
        // All these methods are already implemented
        // decodeLogic[0x00] = [this]() -> void { return instructions.I_00(); };
    }

    // This methode represents the reset button, which also starts the computer
    void reset() noexcept;

    // This methode stops the CPU from executing
    void stop() { b_stop = true; }

    struct Bus {
       private:
        Byte dataBus = 0;
        Word addressBus = 0;
        W65C02& CPU;

       public:
        // This reference is needed to be able to access non-static members of W65C02
        explicit Bus(W65C02& parentRef) : CPU(parentRef) {}

        [[nodiscard]] Byte getData() const { return dataBus; }
        void setData(const Byte& data) {
            if (CPU.registers.RW == false)
                throw EmulatorException(e_ROM, e_CRITICAL, 2,
                                        "Cannot write to databus, because RW is set to false.");
            dataBus = data;
        };

        [[nodiscard]] Word getAddress() const { return addressBus; }
        void setAddress(const Word& address) { addressBus = address; }
    } bus;
    // make RW flag visible to memory
    [[nodiscard]] bool getRW() const { return registers.RW; }  // make RW flag visible to memory

   private:
    bool b_stop = false;
    struct Registers {
        Byte A;  // Accumulator

        // Index registers
        Byte X;  // X index
        Byte Y;  // Y index
        Byte S;  // Stack pointer

        Word PC;  // Program Counter

        // Processor flags NV-BDIZC (7-0)
        struct s_P {
            bool N;               // Negative
            bool V;               // Overflow
            const bool B = true;  // Break (always set)
            bool D;               // Decimal
            bool I;               // Interrupt Disable
            bool Z;               // Zero
            bool C;               // Carry

            Byte getByte() {
                return (N << 7) + (V << 6) + (B << 4) + (D << 3) + (I << 2) + (Z << 1) + C;
            }
        } P;

        bool RW;  // true = Read; false = Write

        // This reference is needed to be able to access non-static members of W65C02
        explicit Registers(W65C02& parent) : CPU(parent) {}

       private:
        W65C02& CPU;
    } registers;

   public:
    // Getter & Setter

    Byte getDataBus() const { return dataBus; }
    void setDataBus(const Byte& data) {
        if (RW == false)
            throw Exception(e_ROM, e_CRITICAL, 2,
                            "Cannot write to databus, because RW is set to false.");
        dataBus = data;
    };

    Word getAddressBus() const { return addressBus; }

    bool getRW() const { return RW; };
};

#endif  // W65C02_H
