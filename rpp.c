#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "preprocessor.h"

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
            strcpy(outFileName, "rpp.a");
        } else {
            strcpy(period+1, "a");
        }
    } else if (argc == 3) {
        strcpy(outFileName, argv[2]);
    }
    preprocessRegFile(inFileName, outFileName);
}
