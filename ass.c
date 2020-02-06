#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"

int main(int argc, char **argv) {
    if (argc > 4 || argc == 1) {
        printf("Usage: ./ass <inFile> [outFile]\n");
        exit(-1);
    }
    char *inFileName = argv[1];
    char outFileName[50];
    if (argc == 2) {
        strcpy(outFileName, inFileName);
        char *period = strchr(outFileName, '.');
        strcpy(period+1, "out");
        if (period == NULL) {
            strcpy(outFileName, "ass.out");
        } else {
            strcpy(period+1, "out");
        }
    } else if (argc == 3) {
        strcpy(outFileName, argv[2]);
    }
    assemble(inFileName, outFileName);
}
