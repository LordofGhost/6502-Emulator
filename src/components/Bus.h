#ifndef BUS_H
#define BUS_H

#include <format>

#include "../Main.h"

class Bus {
    Byte dataBus = 0;
    Word addressBus = 0;

   public:
    [[nodiscard]] Byte getData() const { return dataBus; }
    void setData(const Byte& data) { dataBus = data; };

    [[nodiscard]] Word getAddress() const { return addressBus; }
    void setAddress(const Word& address) { addressBus = address; }

    std::string toStringMD() const {
        return "## Bus\n"
               "| Bus | Value (bin) | Value (dec) | Value (hex) |\n"
               "|-----|-------------|-------------|-------------|\n"
               "| Data | " +
               std::format("{:B}", dataBus) + " | " + std::to_string(dataBus) + " | " +
               std::format("{:X}", dataBus) + " |\n" + "| Address | " +
               std::format("{:B}", addressBus) + " | " + std::to_string(addressBus) + " | " +
               std::format("{:X}", addressBus) + " |\n";
    }
};

#endif  // BUS_H
