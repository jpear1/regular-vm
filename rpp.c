#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "rpp.h"

int main(int argc, char **argv) {
    if (argc > 4 || argc == 1) {
        printf("Usage: ./rpp <inFile> [outFile]\n");
        exit(-1);
    }
    char *inFileName = argv[1];
    char outFileName[50];
    if (argc == 2) {
        strcpy(outFileName, inFileName);
        char *period = strchr(outFileName, '.');
        if (period == NULL) {
            strcpy(outFileName, "rpp.reg");
        } else {
            strcpy(period+1, "reg");
        }
    } else if (argc == 3) {
        strcpy(outFileName, argv[2]);
    }
    preprocessRegFile(inFileName, outFileName);
}

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
