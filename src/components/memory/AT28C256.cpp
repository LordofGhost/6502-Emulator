#include "AT28C256.h"

#include <fstream>

#include "../../exceptions/EmulatorException.h"
#include "../W65C02.h"

extern W65C02 CPU;

void AT28C256::load(const std::filesystem::path& file) const {
    // Check if file exists
    if (!std::filesystem::exists(file) || std::filesystem::is_directory(file))
        throw EmulatorException(
            e_ROM, e_CRITICAL, 3000,
            "Either the file does not exist or a folder has been specified as the path.");

    // Check file size
    if (std::filesystem::file_size(file) < size || std::filesystem::file_size(file) > size)
        throw EmulatorException(
            e_ROM, e_CRITICAL, 3001,
            "File size does not match ROM size. The size of the input file needs be "
            "exactly 32768 bytes.");

    // Load file
    std::ifstream input(file);
    if (!input) throw EmulatorException(e_ROM, e_CRITICAL, 3002, "Input file could not be loaded.");

    input.read(reinterpret_cast<char*>(memory_ptr), size);

    if (!input)
        throw EmulatorException(
            e_ROM, e_CRITICAL, 3003,
            "Error while reading the file. A possible problem is that the file is "
            "smaller than 32768 bytes.");
}

void AT28C256::read() const {
    if (CPU.bus.getAddress() < 0x8000)
        throw EmulatorException(e_ROM, e_CRITICAL, 3400, "Address not inside ROM, lower than 0x8000.");
    if (CPU.bus.getAddress() > 0xFFFF)
        throw EmulatorException(e_ROM, e_CRITICAL, 3401,
                                "Address not inside ROM, higher than 0xFFFF.");

    Word address = CPU.bus.getAddress() - 0x8000;  // ROM goes from 0x8000 to 0xFFFF

    CPU.bus.setData(this->memory_ptr[address]);
}

std::string AT28C256::toStringMD(Word begin, Word end) const noexcept {
    return "# ROM\n" + dumpMD(begin, end);
}

std::string AT28C256::toString(unsigned short begin, unsigned short end) const noexcept {
    return "-- ROM --"
           "---------";
}