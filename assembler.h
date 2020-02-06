#pragma once

#include <stdio.h>

// NOTE: This program assumes system is little endian
#define BINARY_INSTRUCTION_LENGTH 4

void assemble(char const *inFileName, char const *outFileName);

void processSetInstruction(char const *instruction, char const *opName, char *b1, char *b2, char *b3);
void processTwoArgInstruction(char const *instruction, char const *opName, char *b1, char *b2, char *b3);
void processThreeArgInstruction(char const *instruction, char const *opName, char *b1, char *b2, char *b3);