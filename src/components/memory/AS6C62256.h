#ifndef AS6C62256_H
#define AS6C62256_H

#include "../../Main.h"
#include "Memory.h"

/* 32k SRAM
 * Datasheet: https://www.mouser.de/datasheet/2/12/AS6C62256_23_March_2016_rev1_2-1288423.pdf
 */

class AS6C62256 final : Memory {
   public:
    void reset();
    void read() const override;
    void write() const;
    [[nodiscard]] std::string toStringMD(Word begin, Word end) const noexcept override;
    [[nodiscard]] std::string toString(Word begin, Word end) const noexcept override;
};

#endif  // AS6C62256_H
