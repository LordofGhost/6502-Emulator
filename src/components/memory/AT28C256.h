#ifndef AT28C256_H
#define AT28C256_H

#include <filesystem>

#include "Memory.h"

/* 32k EEPROM
 * Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/doc0006.pdf
 */

class AT28C256 final : Memory {
   public:
    void load(const std::filesystem::path& file) const;
    void read() const override;
    [[nodiscard]] std::string toStringMD(Word begin, Word end) const noexcept override;
    [[nodiscard]] std::string toString(Word begin, Word end) const noexcept override;
};

#endif  // AT28C256_H
