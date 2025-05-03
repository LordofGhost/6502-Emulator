#ifndef AT28C256_H
#define AT28C256_H

#include <iostream>

#include "../Main.h"

/* 32k EEPROM
 * Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/doc0006.pdf
 */

class AT28C256 {
   private:
    Byte* memory_ptr;

   public:
    static constexpr Word size = 32768;  // in bytes

    AT28C256() {
        // TODO create load binary file at program start
        memory_ptr = new Byte[32768];
    }

    ~AT28C256() { delete[] memory_ptr; }
    void read() const;
};

#endif  // AT28C256_H
