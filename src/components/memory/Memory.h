#ifndef MEMORY_H
#define MEMORY_H

#include <string>

#include "../../Main.h"

class Memory {
   protected:
    Byte* memory_ptr;

   public:
    static constexpr Word size = 0x8000;

    Memory() { memory_ptr = new Byte[size]; }
    virtual ~Memory() { delete[] memory_ptr; }

    virtual void read() const = 0;

    [[nodiscard]] std::string dump(
        Word begin = 0,
        Word end = size - 1) const noexcept;  // Only ment for debugging features
    [[nodiscard]] std::string dumpMD(
        Word begin = 0,
        Word end = size - 1) const noexcept;  // should only be called with begin % 16 == 0

    [[nodiscard]] virtual std::string toStringMD(Word begin, Word end) const noexcept = 0;
    [[nodiscard]] virtual std::string toString(Word begin, Word end) const noexcept = 0;
};

#endif  // MEMORY_H
