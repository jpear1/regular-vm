#include <stdio.h>

int preprocessRegFile(char const *inFileName, char const *outFileName);

int doArithmeticPass(FILE *inFile, FILE *outFile);

int extractArgs(const char *line);

int getNumArgs(const char *line);