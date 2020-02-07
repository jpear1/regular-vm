#pragma once

// worst case is add r20 r21 r22\n
#define LONGEST_INSTRUCTION_LENGTH 16

void disassembleBinary(char const *inputBinary, int inputSize, char **output, int *outputSize);
void disassembleFile(char const *inFileName, char const *outFileName);
void disassembleWord(const char *word, char *outString);