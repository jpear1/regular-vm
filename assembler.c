#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>

#include "shared.h"
#include "assembler.h"

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
