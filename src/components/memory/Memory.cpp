#include "Memory.h"

#include "../../exceptions/EmulatorException.h"
#include "../../tools/Convert.h"

std::string Memory::dump(Word begin, Word end) const noexcept {
    if (begin >= size || end >= size || begin > end) return "Invalid range arguments\n";

    char result[(end - begin) * 2 + 1];  // 3 letters are needed to display one byte
    int resultIndex = 0;

    for (Word index = begin; index <= end; index++) {
        // Convert 4 bit number to hex value
        result[resultIndex++] = Convert::binToHex((memory_ptr[index] & 0b11110000) >> 4);
        result[resultIndex++] = Convert::binToHex(memory_ptr[index] & 0b00001111);
    }

    result[resultIndex] = 0;

    return result;
}

std::string Memory::dumpMD(Word begin, Word end) const noexcept {
    if (begin >= size || end >= size || begin > end) return "Invalid range arguments\n";

    if (begin % 16 != 0) return "First argument needs to be dividable by 16\n";

    std::string result =
        "|    |0 |1 |2 |3 |4 |5 |6 |7 |8 |9 |A |B |C |D |E |F |\n"
        "|----|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|\n";

    for (Word index = begin; index < end;) {
        char* row = new char[16 * 3 + 7];
        int rowIndex = 0;  // this index keeps track where the next char inside row is written

        row[rowIndex++] = '|';
        row[rowIndex++] = Convert::binToHex((index & 0b1111 << 12) >> 12);
        row[rowIndex++] = Convert::binToHex((index & 0b1111 << 8) >> 8);
        row[rowIndex++] = Convert::binToHex((index & 0b1111 << 4) >> 4);
        row[rowIndex++] = Convert::binToHex(index & 0b1111);
        row[rowIndex++] = '|';

        for (int i = 0; i < 16; i++) {
            // Convert 4 bit number to hex value
            row[rowIndex++] = Convert::binToHex((memory_ptr[index] & 0b11110000) >> 4);
            row[rowIndex++] = Convert::binToHex(memory_ptr[index] & 0b00001111);
            index++;

            if (rowIndex == 53) {
                row[rowIndex++] = '|';
                row[rowIndex++] = '\n';
            } else {
                row[rowIndex++] = '|';
            }
        }

        result += row;
        delete[] row;
    }

    return result + '\n';
}