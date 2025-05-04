#include "Main.h"

#include <iostream>

#include "Exception.h"
#include "components/AS6C62256.h"
#include "components/AT28C256.h"
#include "components/W65C02.h"
#include "components/W65C22.h"

// Create components in data segment
W65C02 CPU;
AS6C62256 RAM;
AT28C256 ROM;
W65C22 IO;

int main() {
    return 1;
}
