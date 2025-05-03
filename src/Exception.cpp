#include "Exception.h"

Exception::Exception(Component component, Exception_t type, unsigned int code) {
    this->component = component;
    this->type = type;
    this->code = code;
}

std::string Exception::toString() const {
    return std::string("-- Exception --\nComponent: " + componentString() +
                       "\nType: " + typeString() + "\nCode: " + std::to_string(this->code));
}

std::string Exception::componentString() const {
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

std::string Exception::typeString() const {
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