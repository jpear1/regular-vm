#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>

#include "disassembler.h"


void disassembleFile(char const *inFileName, char const *outFileName) {
    FILE* inFile = fopen(inFileName, "r");
    FILE* outFile = fopen(outFileName, "w+");
    char inputBinary[1000];
    int i = 0;
    while (!feof(inFile)) {
        fread(&inputBinary[i], 1, 1, inFile);
        i++;
    }
    i--; // to remove EOF from array
    char *outputString;
    int outputSize;
    disassembleBinary(inputBinary, i, &outputString, &outputSize);
    fwrite(outputString, 1, outputSize, outFile);
    free(outputString); // allocated in dissassembleString
    fclose(inFile);
    fclose(outFile);
}

void disassembleBinary(char const *inputBinary, int inputSize, char **output, int *outputSize) {
    assert(inputSize % 4 == 0);
    char *result = malloc(LONGEST_INSTRUCTION_LENGTH * inputSize/4 + 1);
    int rIndex = 0;
    char instruction[LONGEST_INSTRUCTION_LENGTH + 1]; // +1 for the null byte sprintf will insert
    int instLen;
    for (int i = 0; i < inputSize; i += 4) {
        disassembleWord(&inputBinary[i], instruction);
        instLen = strlen(instruction);
        strncpy(result+rIndex, instruction, instLen); // use strncopy to avoid copying the null byte on the end of instruction
        rIndex += instLen;
    }
    result[rIndex] = '\0';
    *output = result;
    *outputSize = rIndex;
}

// word must be 4 bytes
void disassembleWord(const char *word, char *outString) {
    int16_t imm;
    switch (word[0]) {
        // nop
        case 0x00: 
            sprintf(outString, "nop\n");
            break;
        // add rA rB rC
        case 0x01:
            sprintf(outString, "add r%d r%d r%d\n", word[1], word[2], word[3]);
            break;
        // sub rA rB rC
        case 0x02:
            sprintf(outString, "sub r%d r%d r%d\n", word[1], word[2], word[3]);
            break;
        // and rA rB rC
        case 0x03:
            sprintf(outString, "and r%d r%d r%d\n", word[1], word[2], word[3]);
            break;
        // orr rA rB rC
        case 0x04:
            sprintf(outString, "orr r%d r%d r%d\n", word[1], word[2], word[3]);
            break;
        // xor rA rB rC
        case 0x05:
            sprintf(outString, "xor r%d r%d r%d\n", word[1], word[2], word[3]);
            break;
        // not rA rB
        case 0x06:
            sprintf(outString, "not r%d r%d\n", word[1], word[2]);
            break;
        // lsh rA rB rC
        case 0x07:
            sprintf(outString, "lsh r%d r%d r%d\n", word[1], word[2], word[3]);
            break;
        // ash rA rB rC
        case 0x08:
            sprintf(outString, "ash r%d r%d r%d\n", word[1], word[2], word[3]);
            break;
        // tcu rA rB rC
        case 0x09:
            sprintf(outString, "tcu r%d r%d r%d\n", word[1], word[2], word[3]);
            break;
        // tcs rA rB rC
        case 0x0A:
            sprintf(outString, "tcs r%d r%d r%d\n", word[1], word[2], word[3]);
            break;
        // set rA imm
        case 0x0B:
            *(char*)(&imm) = word[2]; // set lsb of imm to word[2]
            *((char*)(&imm)+1) = word[3]; // set lsb of imm to word[3]
            sprintf(outString, "set r%d %d\n", word[1], imm);
            break;
        // mov rA rB
        case 0x0C:
            sprintf(outString, "mov r%d r%d\n", word[1], word[2]);
            break;
        // ldw rA rB
        case 0x0D:
            sprintf(outString, "ldw r%d r%d\n", word[1], word[2]);
            break;
        // stw rA rB
        case 0x0E:
            sprintf(outString, "stw r%d r%d\n", word[1], word[2]);
            break;
        // ldb rA rB
        case 0x0F:
            sprintf(outString, "ldb r%d r%d\n", word[1], word[2]);
            break;
        // stb rA rB
        case 0x10:
            sprintf(outString, "stb r%d r%d\n", word[1], word[2]);
            break;
    }
}