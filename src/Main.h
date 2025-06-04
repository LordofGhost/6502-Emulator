#ifndef MAIN_H
#define MAIN_H

#define Byte unsigned char
#define Word unsigned short

#include <string>

enum e_Component { e_CPU, e_RAM, e_ROM, e_IO, e_Clock };

struct Arguments {
    std::string programPath;
    bool help = false;
    bool logs = false;
    bool logsAll = false;
};

#endif  // MAIN_H
