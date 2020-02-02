#pragma once

#include <stdio.h>

// NOTE: This program assumes system is little endian
#define BINARY_INSTRUCTION_LENGTH 4

void assemble(char const *inFileName, char const *outFileName);

char getOpCode(char const *instruction);
char identifyOpCode(char opCode);

void processSetInstruction(FILE *inFile, char *b1, char *b2, char *b3);
void processTwoArgInstruction(FILE *inFile, char *b1, char *b2, char *b3);
void processThreeArgInstruction(FILE *inFile, char *b1, char *b2, char *b3);