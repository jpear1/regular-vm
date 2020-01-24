#include <stdio.h>
#include <stdlib.h>
#include "disassembler.h"

int main() {
    char instructions[] = {
        0x00, 0, 0, 0, // This should be nop
        0x01, 1, 2, 1, // This should be add r1 r2 r1
        0x05, 20, 21, 22, // This should be xor r20 r21 r22
    };
    char *result;
    int resultSize;
    disassemble(instructions, sizeof(instructions), &result, &resultSize);
    printf("%s", result);
    free(result);
}