#include "Main.h"

#include <iostream>

#include "components/clock/CrystalOscillator.h"
#include "components/cpu/W65C02.h"
#include "components/io/W65C22.h"
#include "components/memory/AS6C62256.h"
#include "components/memory/AT28C256.h"
#include "exceptions/EmulatorException.h"
#include "tools/Logs.h"

// Create components in data segment
W65C02 CPU;
AS6C62256 RAM;
AT28C256 ROM;
W65C22 IO;
CrystalOscillator Clock;

// Create exception vector
std::vector<EmulatorException> EmulatorExceptions;

Arguments arguments;

int main(int argc, char* argv[]) {
    // Check if there are any additional arguments
    if (argc > 1) {
        // Check the additional arguments
        for (int i = 1; argc > i; i++) {
            // Check for help
            if (!strcasecmp(argv[i], "-h") || !strcasecmp(argv[i], "--help")) arguments.help = true;

            // Check for the ROM file
            if (!strcasecmp(argv[i], "-p") || !strcasecmp(argv[i], "--path")) {
                if (argv[i + 1] != nullptr) arguments.programPath = argv[i + 1];
            }

            // Check for logs flag
            if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--log")) arguments.logs = true;
            if (!strcmp(argv[i], "-L") || !strcmp(argv[i], "--LOG")) arguments.logsAll = true;
        }
    }

    // When help flag is set only the help message is shown
    if (arguments.help) {
        std::cout << "Welcome to 6502 emulator!\n\n"
                     "Usage: emulator [-h | --help] [-p | --path <Path>] [-l | --log]\n\n"
                     "Flags:\n"
                     "\t-h --help\t\t\tDisplays help with available flags and value parameters.\n"
                     "\t-p --path\t\t\tSpecifies the path to the ROM file.\n"
                     "\t-l --log\t\t\tCreate log file when process terminates."
                     "\t-L --LOG\t\t\tCreate log file on every CPU execution."
                     ""
                  << std::endl;
        return 0;
    }

    // Load ROM
    try {
        ROM.load(arguments.programPath);
    } catch (EmulatorException &e) {
        std::cout << e.toString() << std::endl;
    }

    // Start CPU
    CPU.reset();

    // Create logs on termination
    if (arguments.logs && !arguments.logsAll) {
        Logs::log();
    }

    // Debug
    std::cout << "---- Debug ----" << std::endl;
    std::cout << Clock.toString() << std::endl;
    // end Debug
}
