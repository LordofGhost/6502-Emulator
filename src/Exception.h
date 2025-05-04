#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

#include "Main.h"

enum Exception_t { e_INFO, e_WARNING, e_CRITICAL };

class Exception : std::exception {
   private:
    Component component;
    Exception_t type;
    unsigned int code;
    std::string description;

   public:
    Exception(Component component, Exception_t type, unsigned int code, const char* description);
    std::string toString() const;

   private:
    std::string componentString() const;
    std::string typeString() const;
};

#endif  // EXCEPTION_H
