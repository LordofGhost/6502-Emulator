#include "W65C02.h"

#include <iostream>

#include "memory/AT28C256.h"
#include "CrystalOscillator.h"

extern AT28C256 ROM;
extern CrystalOscillator Clock;

void W65C02::reset() noexcept {
    b_stop = false;
    std::function<void()> nextCall;

    // Reset cycle
    registers.PC = 0xFFFC;  // $FFFC, $FFFD Reset vector
    Clock.increment(6);

    // CPU loop
    while (!b_stop) {
        try {
            // Fetch
            registers.RW = true;
            bus.setAddress(registers.PC);
            ROM.read();

            // Decode & Check if op code exists
            if (decodeLogic.count(bus.getData()))
                nextCall = decodeLogic.at(bus.getData());
            else
                throw EmulatorException(e_CPU, e_CRITICAL, 50, "Op code does not exist.");

            // Execute
            nextCall();
        } catch (EmulatorException e) {
            std::cout << e.toString() << std::endl;
            b_stop = true;
        }
    }
}

void W65C02::Instructions::I_00() { CPU.registers.P.I = true; }

std::string W65C02::toStringMD() const {
    return "# CPU\n" + bus.toStringMD() + registers.toStringMD();
}