#ifndef AT28C256_H
#define AT28C256_H

#include <filesystem>

#include "../Main.h"

/* 32k EEPROM
 * Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/doc0006.pdf
 */

class AT28C256 {
   private:
    Byte* memory_ptr;
 /* $0000 - $00FF Zero Page
  * $0100 - $01FF Stack */

   public:
    static constexpr Word size = 32768;  // In bytes, 0x8000 in hex

    AT28C256() { memory_ptr = new Byte[size]; }
    ~AT28C256() { delete[] memory_ptr; }

    void load(const std::filesystem::path& file);

    void read() const;

    std::string dump(Word begin = 0, Word end = size - 1);  // Only ment for debugging features
};

#endif  // AT28C256_H
