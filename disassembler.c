#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "disassembler.h"

void disassembleFile(char const *inFileName, char const *outFileName) {
    FILE* inFile = fopen(inFileName, "r");
    FILE* outFile = fopen(outFileName, "w+");
    char inputBinary[100000];
    int i = 0;
    while (!feof(inFile)) {
        fread(&inputBinary[i], 1, 1, inFile);
        i++;
    }
    char *outputString;
    int outputSize;
    disassembleString(inputBinary, i, &outputString, &outputSize);
    fwrite(outputString, 1, outputSize, outFile);
    free(outputString); // allocated in dissassembleString
    fclose(inFile);
    fclose(outFile);
}

void disassembleString(char const *inputBinary, int inputSize, char **output, int *outputSize) {
    assert(inputSize % 4 == 0);
    char *result = malloc(LONGEST_INSTRUCTION_LENGTH * inputSize/4 + 1);
    int rIndex = 0;
    char instruction[LONGEST_INSTRUCTION_LENGTH + 1]; // +1 for the null byte sprintf will insert
    int instLen;
    for (int i = 0; i < inputSize; i += 4) {
        switch (inputBinary[i]) {
            // nop
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
            // not rA rB
            case 0x06:
                sprintf(instruction, "not r%d r%d\n", inputBinary[i+1], inputBinary[i+2]);
                break;
            // lsh rA rB rC
            case 0x07:
                sprintf(instruction, "lsh r%d r%d r%d\n", inputBinary[i+1], inputBinary[i+2], inputBinary[i+3]);
                break;
            // ash rA rB rC
            case 0x08:
                sprintf(instruction, "ash r%d r%d r%d\n", inputBinary[i+1], inputBinary[i+2], inputBinary[i+3]);
                break;
            // tcu rA rB rC
            case 0x09:
                sprintf(instruction, "tcu r%d r%d r%d\n", inputBinary[i+1], inputBinary[i+2], inputBinary[i+3]);
                break;
            // tcs rA rB rC
            case 0x0A:
                sprintf(instruction, "tcs r%d r%d r%d\n", inputBinary[i+1], inputBinary[i+2], inputBinary[i+3]);
                break;
            // set rA imm
            case 0x0B:
                sprintf(instruction, "set r%d %d\n", inputBinary[i+1], inputBinary[i+2]);
                break;
            // mov rA rB
            case 0x0C:
                sprintf(instruction, "mov r%d r%d\n", inputBinary[i+1], inputBinary[i+2]);
                break;
            // ldw rA rB
            case 0x0D:
                sprintf(instruction, "ldw r%d r%d\n", inputBinary[i+1], inputBinary[i+2]);
                break;
            // stw rA rB
            case 0x0E:
                sprintf(instruction, "stw r%d r%d\n", inputBinary[i+1], inputBinary[i+2]);
                break;
            // ldb rA rB
            case 0x0F:
                sprintf(instruction, "ldb r%d r%d\n", inputBinary[i+1], inputBinary[i+2]);
                break;
            // stb rA rB
            case 0x10:
                sprintf(instruction, "stb r%d r%d\n", inputBinary[i+1], inputBinary[i+2]);
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