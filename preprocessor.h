#include <stdio.h>

int preprocessRegFile(char const *inFileName, char const *outFileName);

int doArithmeticPass(char *inString, char *outString);
void insertArithmeticForArg(char *arg, char **writePtr);

int doPushPopPass(char *inString, char *outString);

void refreshInOutStrings(char **inString, char **outString);

int extractArgs(const char *line);

int getNumArgs(const char *line);