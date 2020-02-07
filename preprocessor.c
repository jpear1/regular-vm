#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "preprocessor.h"
#include "shared.h"

#define PC "r31"
#define FP "r30"
#define MACRO1 "r29" // reserved for arithmetic macro
#define MACRO2 "r28" // reserved for push and pop macros

int preprocessRegFile(char const *inFileName, char const *outFileName) {
    FILE *inFile = fopen(inFileName, "r");
    FILE *outFile = fopen(outFileName, "w");

    fseek(inFile, 0, SEEK_END);
    int fileSize = ftell(inFile);
    fseek(inFile, 0, SEEK_SET);

    char *inString = malloc(fileSize+1);
    char *outString = malloc(4*fileSize+1);
    fread(inString, fileSize, 1, inFile);
    inString[fileSize] = '\0';

    doArithmeticPass(inString, outString);
    refreshInOutStrings(&inString, &outString);
    doPushPopPass(inString, outString);

    fwrite(outString, strlen(outString), 1, outFile);
    fclose(inFile);
    fclose(outFile);
}

// only supports one arithmetic macro per line
int doArithmeticPass(char *inString, char *outString) {
    char *instruction;
    char *writePtr = outString;
    char *args[3] = {0};
    int argc;
    int len = strlen(inString);
    char *saveptr;
    for (char *line = strtok_r(inString, "\n", &saveptr); line != NULL; line = strtok_r(NULL, "\n", &saveptr)) {
        instruction = strtok(line, " ");
        argc = getArgcForOpCode(getOpCode(instruction));

        for (int i = 0; i < sizeof(args)/sizeof(args[0]); i++) {
            args[i] = i >= argc ? NULL : strtok(NULL, " ");
        }

        for (int i = 0; i < sizeof(args)/sizeof(args[0]) && args[i] != NULL; i++) {
            if (index(args[i], '+') || index(args[i], '-')) {
                insertArithmeticForArg(args[i], &writePtr);
                break;
            }
        }

        writePtr += sprintf(writePtr, "%s", instruction);
        int isNewline = 0;
        for (int i = 0; i < sizeof(args)/sizeof(args[0]) && args[i] != NULL; i++) {
            writePtr += sprintf(writePtr, " %s", args[i]);
            isNewline = isNewline || index(args[i], '\n');
        }
        if (!isNewline)
            writePtr += sprintf(writePtr, "\n");
    }
}

void insertArithmeticForArg(char *arg, char **writePtr) {
    char op;
    int reg, num;
    sscanf(arg, "r%d%c%d", &reg, &op, &num);
    *writePtr += sprintf(*writePtr, "set "MACRO1" %d\n", num);
    if (op == '+')
            *writePtr += sprintf(*writePtr, "add r%d r%d "MACRO1"\n", reg, reg);
    else
            *writePtr += sprintf(*writePtr, "sub r%d r%d "MACRO1"\n", reg, reg);
    *strchr(arg, op) = '\0';
}

void refreshInOutStrings(char **inString, char **outString) {
    free(*inString);
    *inString = *outString;
    *outString = malloc(4 * strlen(*inString));
}

int doPushPopPass(char *inString, char *outString) {
    char *instruction, *saveptr;
    char *writePtr = outString;
    char *args[3] = {0};
    int len = strlen(inString), argc;

    for (char *line = strtok_r(inString, "\n", &saveptr); line != NULL; line = strtok_r(NULL, "\n", &saveptr)) {
        instruction = strtok(line, " ");
        argc = getArgcForOpCode(getOpCode(instruction));

        for (int i = 0; i < sizeof(args)/sizeof(args[0]); i++) {
            args[i] = i >= argc ? NULL : strtok(NULL, " ");
        }

        if (strcmp(instruction, "push") == 0) {
            writePtr += sprintf(writePtr, "set "MACRO2" 4\n");
            writePtr += sprintf(writePtr, "add "FP" "MACRO2"\n");
            writePtr += sprintf(writePtr, "stw "FP" r%s \n", args[0]);
        } else if (strcmp(instruction, "pop") == 0) {
            writePtr += sprintf(writePtr, "ldw r%s "FP"\n", args[0]);
            writePtr += sprintf(writePtr, "set "MACRO2" 4\n");
            writePtr += sprintf(writePtr, "sub "FP" "MACRO2"\n");
        } else {
            writePtr += sprintf(writePtr, "%s", instruction);
            for (int i = 0; i < sizeof(args)/sizeof(args[0]) && args[i] != NULL; i++) {
                writePtr += sprintf(writePtr, " %s", args[i]);
            }
            writePtr += sprintf(writePtr, "\n");
        }
    }
}