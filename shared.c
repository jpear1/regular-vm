#include <string.h>

char identifyOpCode(char opCode) {
    // nop
    if (opCode == 0x00)
        return 0;
    // set
    if (opCode == 0x0b)
        return 1;
    // two arg op
    if (opCode == 0x06 ||
            opCode == 0x0c ||
            opCode == 0x0d ||
            opCode == 0x0e ||
            opCode == 0x0f ||
            opCode == 0x10)
        return 2;
    // three arg op
    if (opCode == 0x01 ||
            opCode == 0x02 ||
            opCode == 0x03 ||
            opCode == 0x04 ||
            opCode == 0x05 ||
            opCode == 0x07 ||
            opCode == 0x08 ||
            opCode == 0x09 ||
            opCode == 0x0a)
        return 3;
    
}


char getOpCode(char const *instruction) {
    if (strcmp(instruction, "nop") == 0)
        return 0x0;
    if (strcmp(instruction, "add") == 0)
        return 0x1;
    if (strcmp(instruction, "sub") == 0)
        return 0x2;
    if (strcmp(instruction, "and") == 0)
        return 0x3;
    if (strcmp(instruction, "orr") == 0)
        return 0x4;
    if (strcmp(instruction, "xor") == 0)
        return 0x5;
    if (strcmp(instruction, "not") == 0)
        return 0x6;
    if (strcmp(instruction, "lsh") == 0)
        return 0x7;
    if (strcmp(instruction, "ash") == 0)
        return 0x8;
    if (strcmp(instruction, "tcu") == 0)
        return 0x9;
    if (strcmp(instruction, "tcs") == 0)
        return 0xA;
    if (strcmp(instruction, "set") == 0)
        return 0xB;
    if (strcmp(instruction, "mov") == 0)
        return 0xC;
    if (strcmp(instruction, "ldw") == 0)
        return 0xD;
    if (strcmp(instruction, "stw") == 0)
        return 0xE;
    if (strcmp(instruction, "ldb") == 0)
        return 0xF;
    if (strcmp(instruction, "stb") == 0)
        return 0x10;
    return 0xFF;
}