#ifndef AT28C256_H
#define AT28C256_H

#include <filesystem>

#include "../Component.h"
#include "Memory.h"

/* 32k EEPROM
 * Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/doc0006.pdf
 */

class AT28C256 final : public Component, Memory {
    void read() const override;

   public:
    void load(const std::filesystem::path& file) const;
    void onClockCycle(Phase phase) override;
    [[nodiscard]] std::string toStringMD(Word begin, Word end) const noexcept override;
    [[nodiscard]] std::string toString(Word begin, Word end) const noexcept override;
};

#endif  // AT28C256_H
