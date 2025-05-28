#include "Convert.h"

char Convert::binToHex(Byte i) {
    if (i >= 0 && i <= 9) return i + 48;
    if (i >= 10 && i <= 15) return i + 55;
    return '?';
}
