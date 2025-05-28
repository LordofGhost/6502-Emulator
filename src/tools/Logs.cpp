#include "Logs.h"

#include <filesystem>
#include <sstream>
#include <fstream>

#include "../components/AS6C62256.h"
#include "../components/AT28C256.h"
#include "../components/CrystalOscillator.h"
#include "../components/W65C02.h"
#include "../components/W65C22.h"

extern W65C02 CPU;
extern AS6C62256 RAM;
extern AT28C256 ROM;
extern W65C22 IO;
extern CrystalOscillator Clock;

void Logs::all() {
    std::string content = "";

    content += CPU.toString();
    //content += RAM.toString();
    //content += ROM.toString();
    content += IO.toString();
    content += Clock.toString();

    createFile(content);
}
void Logs::component(Component component) {
    std::string content = "";

    switch (component) {
        case e_CPU:
            content += CPU.toString();
            break;
        case e_RAM:
            content += RAM.toString();
            break;
        case e_ROM:
            content += ROM.toString();
            break;
        case e_IO:
            content += IO.toString();
            break;
        case e_Clock:
            content += Clock.toString();
            break;
        default:
            content += "This not a component or it is not available in logs.";
            break;
    }

    createFile(content);
}

std::string Logs::getFileName() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&time);

    std::ostringstream oss;
    oss << std::put_time(localTime, "T:%H_%M_%S_D:%Y-%m-%d");
    return "C:" + std::to_string(Clock.getCycles()) + "_" + oss.str() + ".txt";
}

void Logs::createFile(const std::string& content) {
    const std::filesystem::path folder = "./logs/";
    const std::filesystem::path fileName = getFileName();

    // Check if logs folder exists
    if (!std::filesystem::exists(folder)) std::filesystem::create_directory(folder);

    // Open file
    if (std::ofstream file(folder.string() + fileName.string()); file.is_open()) {
        file << content << std::endl;
        file.close();
    } else {
        // TODO
        throw 1;
    }
}