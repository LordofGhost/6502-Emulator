#include "EmulatorException.h"

EmulatorException::EmulatorException(Component component, Exception_t type, unsigned int code,
                     const char* description) noexcept {
    this->component = component;
    this->type = type;
    this->code = code;
    description == nullptr ? this->description = "Unknown Error"
                           : this->description = std::string(description);
}

std::string EmulatorException::toString() const noexcept {
    return std::string("-- Exception --\nComponent: " + componentString() +
                       "\nType: " + typeString() + "\nCode: " + std::to_string(this->code) +
                       "\nDescription: " + this->description +
                       "\n---------------");
}

std::string EmulatorException::componentString() const  noexcept {
    switch (this->component) {
        case e_CPU:
            return "CPU";
        case e_RAM:
            return "RAM";
        case e_ROM:
            return "ROM";
        case e_IO:
            return "IO";
        default:
            return "unknown";
    }
}

std::string EmulatorException::typeString() const noexcept {
    switch (this->type) {
        case e_INFO:
            return "info";
        case e_WARNING:
            return "warning";
        case e_CRITICAL:
            return "critical";
        default:
            return "unknown";
    }
}