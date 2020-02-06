#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "disassembler.h"

int main(int argc, char **argv) {
    if (argc > 4 || argc == 1) {
        printf("Usage: ./dsm <inFile> [outFile]\n");
        exit(-1);
    }
    char *inFileName = argv[1];
    char outFileName[50];
    if (argc == 2) {
        strcpy(outFileName, inFileName);
        char *period = strchr(outFileName, '.');
        if (period == NULL) {
            strcpy(outFileName, "dsm.dsm");
        } else {
            strcpy(period+1, "dsm");
        }
    } else if (argc == 3) {
        strcpy(outFileName, argv[2]);
    }
    disassembleFile(inFileName, outFileName);
}