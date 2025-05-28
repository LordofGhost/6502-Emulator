#include "AS6C62256.h"

#include "../exceptions/EmulatorException.h"
#include "../tools/Convert.h"
#include "W65C02.h"

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

std::string AS6C62256::dump(Word begin, Word end) const {
    if (begin >= size || end >= size || begin > end)
        throw EmulatorException(e_RAM, e_WARNING, 30, "Invalid range arguments");

    char result[(end - begin) * 3 + 1];  // 3 letters are needed to display one byte
    int resultIndex = 0;

    for (Word index = begin; index <= end; index++) {
        // Convert 4 bit number to hex value
        result[resultIndex++] = Convert::binToHex((memory_ptr[index] & 0b11110000) >> 4);
        result[resultIndex++] = Convert::binToHex(memory_ptr[index] & 0b00001111);
        result[resultIndex++] = resultIndex % 59 == 0 ? '\n' : ' ';
    }

    result[resultIndex++] = 0;

    return result;
}
std::string AS6C62256::toString() const {
    return "-- RAM --\n" + dump() + "---------\n";
}
