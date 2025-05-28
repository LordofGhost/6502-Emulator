#include "AS6C62256.h"

#include "../Exception.h"
#include "../Tools.h"
#include "W65C02.h"

extern W65C02 CPU;

void AS6C62256::reset() {
    delete[] memory_ptr;
    memory_ptr = new Byte[size];
}

void AS6C62256::read() const {
    // Due to addressing restrictions, only half of the RAM can be used.
    if (CPU.getAddressBus() > 0x4000)
        throw Exception(e_RAM, e_CRITICAL, 1, "Address not inside RAM, higher than 0x4000.");

    CPU.setDataBus(memory_ptr[CPU.getAddressBus()]);
}

void AS6C62256::write() const {
    if (CPU.getAddressBus() > 0x4000)
        throw Exception(e_RAM, e_CRITICAL, 1, "Address not inside RAM, higher than 0x4000.");

    if (CPU.getRW() == true)
        throw Exception(e_RAM, e_CRITICAL, 2, "Cannot write to ram, because RW is set to read.");

    memory_ptr[CPU.getAddressBus()] = CPU.getDataBus();
}

std::string AS6C62256::dump(Word begin, Word end) const {
    if (begin >= size || end >= size || begin > end)
        throw Exception(e_RAM, e_WARNING, 30, "Invalid range arguments");

    char result[(end - begin) * 3 + 1];  // 3 letters are needed to display one byte
    int resultIndex = 0;

    for (Word index = begin; index <= end; index++) {
        // Convert 4 bit number to hex value
        result[resultIndex++] = Tools::binToHex((memory_ptr[index] & 0b11110000) >> 4);
        result[resultIndex++] = Tools::binToHex(memory_ptr[index] & 0b00001111);
        result[resultIndex++] = ' ';
    }

    result[resultIndex++] = 0;

    return result;
}
