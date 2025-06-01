#include "AS6C62256.h"

#include "../../exceptions/EmulatorException.h"
#include "../W65C02.h"

extern W65C02 CPU;

void AS6C62256::reset() {
    delete[] memory_ptr;
    memory_ptr = new Byte[size];
}

void AS6C62256::read() const {
    // Due to addressing restrictions, only half of the RAM can be used.
    if (CPU.bus.getAddress() > 0x4000)
        throw EmulatorException(e_RAM, e_CRITICAL, 1,
                                "Address not inside RAM, higher than 0x4000.");

    CPU.bus.setData(memory_ptr[CPU.bus.getAddress()]);
}

void AS6C62256::write() const {
    if (CPU.bus.getAddress() > 0x4000)
        throw EmulatorException(e_RAM, e_CRITICAL, 1,
                                "Address not inside RAM, higher than 0x4000.");

    if (CPU.getRW() == true)
        throw EmulatorException(e_RAM, e_CRITICAL, 2,
                                "Cannot write to ram, because RW is set to read.");

    memory_ptr[CPU.bus.getAddress()] = CPU.bus.getData();
}

std::string AS6C62256::toStringMD(Word begin, Word end) const noexcept {
    // TODO
    return "# RAM\n" + dumpMD(begin, end);
}
std::string AS6C62256::toString(Word begin, Word end) const noexcept {
    return "-- RAM --"
           // TODO
           + dump(begin, end) + "---------";
}