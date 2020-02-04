#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>

#include "assembler.h"

int main(int argc, char **argv) {
    if (argc > 4 || argc == 1) {
        printf("Usage: ./ass <inFile> [outFile]\n");
        exit(-1);
    }
    char *inFileName = argv[1];
    char outFileName[50];
    if (argc == 2) {
        strcpy(outFileName, inFileName);
        char *period = strchr(outFileName, '.');
        if (period == NULL) {
            strcpy(outFileName, "ass.out");
        } else {
            strcpy(period+1, "out");
        }
    } else if (argc == 3) {
        strcpy(outFileName, argv[2]);
    }
    assemble(inFileName, outFileName);
}

void assemble(char const *inFileName, char const *outFileName) {
    FILE *inFile = fopen(inFileName, "r");
    FILE *outFile = fopen(outFileName, "w+");

    char line[100];
    char opName[100];
    char opCode;
    char id;
    char b1, b2, b3;

    while (!feof(inFile)) {
        char *indicator = fgets(line, 100, inFile);
        if (indicator == NULL)
            continue;
        if (ferror(inFile))
            exit(-1);

        sscanf(line, "%s", opName);
        opCode = getOpCode(opName);
        id = identifyOpCode(opCode);
        switch (id) {
            case 0: // nop
                b1 = b2 = b3 = 0;
                break;
            case 1: // set
                processSetInstruction(line, opName, &b1, &b2, &b3);
                break;
            case 2: // two args
                processTwoArgInstruction(line, opName, &b1, &b2, &b3);
                break;
            case 3: // three args
                processThreeArgInstruction(line, opName, &b1, &b2, &b3);
                break;
        }
        fwrite(&opCode, 1, 1, outFile);
        fwrite(&b1, 1, 1, outFile);
        fwrite(&b2, 1, 1, outFile);
        fwrite(&b3, 1, 1, outFile);
    }

    fclose(inFile);
    fclose(outFile);
}

void processSetInstruction(char const *instruction, char const *opName, char *b1, char *b2, char *b3) {
    int i1, immHolder;
    char formatString[50];
    strcpy(formatString, opName);
    strcat(formatString, " r%i %i");
    sscanf(instruction, formatString, &i1, &immHolder);
    int16_t imm = (int16_t) immHolder;
    *b1 = (char) i1; // assign lsb to byte 1
    *b2 = (char) imm; // assign lsb to byte 2
    *b3 = * (((char*) &imm) + 1); // assign msb to byte 3
}

void processTwoArgInstruction(char const *instruction, char const *opName, char *b1, char *b2, char *b3) {
    int i1, i2;
    char formatString[50];
    strcpy(formatString, opName);
    strcat(formatString, " r%i r%i");
    sscanf(instruction, formatString, &i1, &i2);
    *b1 = (char) i1; // assign lsb to byte 1
    *b2 = (char) i2; // assign lsb to byte 2
    *b3 = 0;
}

void processThreeArgInstruction(char const *instruction, char const *opName, char *b1, char *b2, char *b3) {
    int i1, i2, i3;
    char formatString[50];
    strcpy(formatString, opName);
    strcat(formatString, " r%i r%i r%i");
    int i = sscanf(instruction, formatString, &i1, &i2, &i3);
    *b1 = (char) i1; // assign lsb to byte 1
    *b2 = (char) i2; // assign lsb to byte 2
    *b3 = (char) i3; // assign lsb to byte 3
}

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
