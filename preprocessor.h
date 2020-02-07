#include <stdio.h>

int preprocessRegFile(char const *inFileName, char const *outFileName);

int doArithmeticPass(char *inString, char *outString);
void insertArithmeticForArg(char *arg, char **writePtr);

int doPushPopPass(FILE *inFile, FILE *outFile);

void swapInOutStrings(char **inString, char **outString);

int extractArgs(const char *line);

int getNumArgs(const char *line);