#include "AT28C256.h"

#include "../Exception.h"
#include "W65C02.h"

extern W65C02 CPU;

void AT28C256::read() const {
    if (CPU.addressBus < 0x8000) throw Exception(e_ROM, e_CRITICAL, 1);  // Address not inside ROM

    Word address = CPU.addressBus - 0x8000;  // ROM goes from 0x8000 to 0xFFFF

    if (address > this->size - 1) {
        CPU.dataBus = this->memory_ptr[address];
    } else {
        throw Exception(e_ROM, e_CRITICAL, 1);  // Address not inside ROM
    }
}