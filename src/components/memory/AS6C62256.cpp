#include "AS6C62256.h"

#include "../../exceptions/EmulatorException.h"
#include "../Bus.h"

extern Bus bus;

void AS6C62256::reset() {
    delete[] memory_ptr;
    memory_ptr = new Byte[size];
}

void AS6C62256::read() const {
    // Due to addressing restrictions, only half of the RAM can be used.
    if (bus.getAddress() > 0x4000)
        throw EmulatorException(
            e_RAM, e_CRITICAL, 2200,
            "Address not inside RAM, higher than 0x4000. While trying to read from RAM.");

    bus.setData(memory_ptr[bus.getAddress()]);
}

void AS6C62256::write() const {
    if (bus.getAddress() > 0x4000)
        throw EmulatorException(
            e_RAM, e_CRITICAL, 2201,
            "Address not inside RAM, higher than 0x4000. While trying to write to RAM.");

    memory_ptr[bus.getAddress()] = bus.getData();
}

void AS6C62256::onClockCycle(Phase phase) {
    // TODO
}

std::string AS6C62256::toStringMD(Word begin, Word end) const noexcept {
    return "# RAM\n" + dumpMD(begin, end);
}
std::string AS6C62256::toString(Word begin, Word end) const noexcept {
    return "-- RAM --"
           + dump(begin, end) + "---------";
}