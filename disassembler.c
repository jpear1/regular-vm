#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "disassembler.h"

void disassemble(char *inputBinary, int inputSize, char **output, int *outputSize) {
    assert(inputSize % 4 == 0);
    char *result = malloc(LONGEST_INSTRUCTION_LENGTH * inputSize/4 + 1);
    int rIndex = 0;
    char instruction[LONGEST_INSTRUCTION_LENGTH + 1]; // +1 for the null byte sprintf will insert
    int instLen;
    for (int i = 0; i < inputSize; i += 4) {
        switch (inputBinary[i]) {
            //nop
            case 0x00: 
                sprintf(instruction, "nop\n");
                break;
            // add rA rB rC
            case 0x01:
                sprintf(instruction, "add r%d r%d r%d\n", inputBinary[i+1], inputBinary[i+2], inputBinary[i+3]);
                break;
            // sub rA rB rC
            case 0x02:
                sprintf(instruction, "sub r%d r%d r%d\n", inputBinary[i+1], inputBinary[i+2], inputBinary[i+3]);
                break;
            // and rA rB rC
            case 0x03:
                sprintf(instruction, "and r%d r%d r%d\n", inputBinary[i+1], inputBinary[i+2], inputBinary[i+3]);
                break;
            // orr rA rB rC
            case 0x04:
                sprintf(instruction, "orr r%d r%d r%d\n", inputBinary[i+1], inputBinary[i+2], inputBinary[i+3]);
                break;
            // xor rA rB rC
            case 0x05:
                sprintf(instruction, "xor r%d r%d r%d\n", inputBinary[i+1], inputBinary[i+2], inputBinary[i+3]);
                break;
        }
        instLen = strlen(instruction);
        strncpy(result+rIndex, instruction, instLen); // use strncopy to avoid copying the null byte on the end of instruction
        rIndex += instLen;
    }
    result[rIndex] = '\0';
    *output = result;
    *outputSize = rIndex;
}