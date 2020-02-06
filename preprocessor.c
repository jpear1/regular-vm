#include <stdio.h>

#include "preprocessor.h"

int preprocessRegFile(char const *inFileName, char const *outFileName) {
    FILE *inFile = fopen(inFileName, "r");
    FILE *outFile = fopen(outFileName, "w");
    doArithmeticPass(inFile, outFile);
    fclose(inFile);
    fclose(outFile);
}

int doArithmeticPass(FILE *inFile, FILE *outFile) {
    char line[300] = {0};
    while (fgets(line, sizeof(line), inFile)) {
        
    }
}