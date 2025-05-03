#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

#include "Main.h"

enum Exception_t { e_INFO, e_WARNING, e_CRITICAL };

class Exception {
   private:
    Component component;
    Exception_t type;
    unsigned int code;

   public:
    Exception(Component component, Exception_t type, unsigned int code);
    std::string toString() const;

   private:
    std::string componentString() const;
    std::string typeString() const;
};

#endif  // EXCEPTION_H
