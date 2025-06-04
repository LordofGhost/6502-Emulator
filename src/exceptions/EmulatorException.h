#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

#include "../Main.h"

enum Exception_t { e_INFO, e_WARNING, e_CRITICAL };

class EmulatorException : std::exception {
   private:
    e_Component component;
    Exception_t type;
    unsigned int code;
    std::string description;

   public:
    EmulatorException(e_Component component, Exception_t type, unsigned int code,
              const char* description) noexcept;
    std::string toString() const noexcept;
    std::string toStringMD() const noexcept;

   private:
    std::string componentString() const noexcept;
    std::string typeString() const noexcept;
};

#endif  // EXCEPTION_H
