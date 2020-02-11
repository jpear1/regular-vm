#include <stdio.h>

typedef struct {
    char *name;
    unsigned int line;
} Label;

int preprocessRegFile(char const *inFileName, char const *outFileName);

int doArithmeticPass(char *inString, char *outString);
void insertArithmeticForArg(char *arg, char **writePtr);

int doPushPopPass(char *inString, char *outString);

int doLabelPass(char **inString, char **outString);
int doJmpPass(char *inString, char *outString);

int populateSymbolTable(char *inString, char *outString);
int substituteSymbols(char *inString, char *outString);

void refreshInOutStrings(char **inString, char **outString);

int extractArgs(const char *line);

int getNumArgs(const char *line);