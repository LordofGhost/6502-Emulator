#ifndef AS6C62256_H
#define AS6C62256_H

#include <string>

#include "../Main.h"

/* 32k SRAM
 * Datasheet: https://www.mouser.de/datasheet/2/12/AS6C62256_23_March_2016_rev1_2-1288423.pdf
 */

class AS6C62256 {
   private:
    Byte* memory_ptr;
    /* $0000 - $00FF Zero Page
     * $0100 - $01FF Stack */

   public:
    static constexpr Word size = 32768;  // In bytes, 0x8000 in hex

    AS6C62256() { memory_ptr = new Byte[size]; }

    ~AS6C62256() { delete[] memory_ptr; }

    void reset();

    void read() const;
    void write() const;

    std::string dump(Word begin = 0,
                     Word end = size - 1) const;  // Only ment for debugging features

    std::string toString() const;
};

#endif  // AS6C62256_H
