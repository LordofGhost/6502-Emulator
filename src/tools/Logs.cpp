#include "Logs.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../components/CrystalOscillator.h"
#include "../components/W65C02.h"
#include "../components/W65C22.h"
#include "../components/memory/AS6C62256.h"
#include "../components/memory/AT28C256.h"

extern W65C02 CPU;
extern AS6C62256 RAM;
extern AT28C256 ROM;
extern W65C22 IO;
extern CrystalOscillator Clock;
extern std::vector<EmulatorException> EmulatorExceptions;

static nlohmann::json config;

void Logs::log() {
    std::string content;

    // Load logs config file
    if (config.empty()) {
        try {
            static std::ifstream f("./logsConfig.json");
            config = nlohmann::json::parse(f);
        } catch ([[maybe_unused]] std::exception &e) {
            content += "logsConfig.json could not be loaded!";
            createFile(content);
            return;
        }
    }

    try {
        if (config["CPU"]["active"]) content += CPU.toStringMD();
        if (config["RAM"]["active"]) {
            content += RAM.toStringMD(config["RAM"]["start"], config["RAM"]["end"]);
        }
        if (config["ROM"]["active"]) {
            content += ROM.toStringMD(config["ROM"]["start"], config["ROM"]["end"]);
        }
        if (config["IO"]["active"]) content += IO.toStringMD();
        if (config["Clock"]["active"]) content += Clock.toStringMD();
        if (config["Exceptions"]["active"]) content += exceptions();
    } catch ([[maybe_unused]] std::exception &e) {
        content +=
            "Error occurred during log file creation, please check the logsConfig.json for syntax "
            "errors!\n";
        createFile(content);
        return;
    }

    createFile(content);
}

std::string Logs::exceptions() {
    std::string content = "# Exceptions\n\n";

    for (const EmulatorException &exception : EmulatorExceptions) content += exception.toStringMD();

    return content;
}

std::string Logs::getFileName() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    const std::tm* localTime = std::localtime(&time);

    std::ostringstream oss;
    oss << std::put_time(localTime, "%H:%M:%S:_%Y:%m:%d");
    return std::to_string(Clock.getCycles()) + "_" + oss.str() + ".md";
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
        std::cout << "Log file" << std::endl;
    } else {
        std::cerr << "Log file could not be created!" << std::endl;
    }
}