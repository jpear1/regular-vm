#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gdbm.h>

#include "preprocessor.h"
#include "shared.h"

#define PC "r0"
#define SP "r31"
#define FP "r30"
#define MACRO1 "r29" // reserved for arithmetic macro
#define MACRO2 "r28" // reserved for push and pop macros
#define ST_NAME "symbolTable.db"

GDBM_FILE symbolTable;

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
    refreshInOutStrings(&inString, &outString);
    doArithmeticPass(inString, outString);
    refreshInOutStrings(&inString, &outString);
    doLabelPass(&inString, &outString);

    fputs(outString, outFile);
    free(outString);
    free(inString);
    fclose(inFile);
    fclose(outFile);
    return 0;
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
        argc = getArgcForInstruction(instruction);

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
    int len = strlen(*inString); 
    *outString = malloc(4 * len);
}

int doPushPopPass(char *inString, char *outString) {
    char *instruction, *saveptr;
    char *writePtr = outString;
    char *args[3] = {0};
    int len = strlen(inString), argc;

    for (char *line = strtok_r(inString, "\n", &saveptr); line != NULL; line = strtok_r(NULL, "\n", &saveptr)) {
        instruction = strtok(line, " ");
        argc = getArgcForInstruction(instruction);

        for (int i = 0; i < sizeof(args)/sizeof(args[0]); i++) {
            args[i] = i >= argc ? NULL : strtok(NULL, " ");
        }

        if (strcmp(instruction, "push") == 0) {
            writePtr += sprintf(writePtr, "stw "SP" %s \n", args[0]);
            writePtr += sprintf(writePtr, "set "MACRO2" 4\n");
            writePtr += sprintf(writePtr, "add "SP" "SP" "MACRO2"\n");
        } else if (strcmp(instruction, "pop") == 0) {
            writePtr += sprintf(writePtr, "set "MACRO2" 4\n");
            writePtr += sprintf(writePtr, "sub "SP" "SP" "MACRO2"\n");
            writePtr += sprintf(writePtr, "ldw %s "SP"\n", args[0]);
        } else {
            writePtr += sprintf(writePtr, "%s", instruction);
            for (int i = 0; i < sizeof(args)/sizeof(args[0]) && args[i] != NULL; i++) {
                writePtr += sprintf(writePtr, " %s", args[i]);
            }
            writePtr += sprintf(writePtr, "\n");
        }
        args[0] = args[1] = args[2] = 0;
    }
}

void dbOpenError() {
    printf("error opening database\n");
    exit(1);
}

int doLabelPass(char **inString, char **outString) {
    if (access(ST_NAME, F_OK) != -1)
        remove(ST_NAME);
    symbolTable = gdbm_open(ST_NAME, 0, GDBM_WRCREAT, 0, dbOpenError);
    if (!symbolTable)
        dbOpenError();
    populateSymbolTable(*inString, *outString);
    refreshInOutStrings(inString, outString);
    substituteSymbols(*inString, *outString);
    gdbm_close(symbolTable);
    remove(ST_NAME);
}

int populateSymbolTable(char *inString, char *outString) {
    char *instruction, *saveptr;
    char *writePtr = outString;
    char *args[3] = {0};
    int len = strlen(inString), argc, lineNum = 0;

    for (char *line = strtok_r(inString, "\n", &saveptr); line != NULL; line = strtok_r(NULL, "\n", &saveptr)) {
        instruction = strtok(line, " ");
        argc = getArgcForInstruction(instruction);

        for (int i = 0; i < sizeof(args)/sizeof(args[0]); i++) {
            args[i] = i >= argc ? NULL : strtok(NULL, " ");
        }

        if (strcmp(instruction, "lab") == 0) {
            datum key = {args[0], strlen(args[0])};
            int labelVal = lineNum - 1;
            datum entry = {(char *)&lineNum, sizeof(lineNum)};
            int didCollide = gdbm_store(symbolTable, key, entry, GDBM_INSERT);
            if (didCollide) {
                printf("duplicate symbol: %s\n", args[0]);
                exit(1);
            }
        } else {
            writePtr += sprintf(writePtr, "%s", instruction);
            for (int i = 0; i < sizeof(args)/sizeof(args[0]) && args[i] != NULL; i++) {
                writePtr += sprintf(writePtr, " %s", args[i]);
            }
            writePtr += sprintf(writePtr, "\n");
            lineNum++;
        }
        args[0] = args[1] = args[2] = 0;
    }
}
int substituteSymbols(char *inString, char *outString) {
    char *instruction, *saveptr;
    char *writePtr = outString;
    char *args[3] = {0};
    int len = strlen(inString), argc, lineNum = 0;

    for (char *line = strtok_r(inString, "\n", &saveptr); line != NULL; line = strtok_r(NULL, "\n", &saveptr)) {
        instruction = strtok(line, " ");
        argc = getArgcForInstruction(instruction);

        for (int i = 0; i < sizeof(args)/sizeof(args[0]); i++) {
            args[i] = i >= argc ? NULL : strtok(NULL, " ");
        }

        int recreateInstruction = 1;

        if (strcmp(instruction, "jmp") == 0) {
            if (atoi(args[0]) == 0) {
                recreateInstruction = 0;
                datum key = {args[0], strlen(args[0])};
                datum entry = gdbm_fetch(symbolTable, key);
                if (entry.dptr == NULL) {
                    exit(-1);
                }
                writePtr += sprintf(writePtr, "set "PC" %d\n",  * (int *)entry.dptr);
                free(entry.dptr);
            }
        } 
        if (recreateInstruction) {
            writePtr += sprintf(writePtr, "%s", instruction);
            for (int i = 0; i < sizeof(args)/sizeof(args[0]) && args[i] != NULL; i++) {
                writePtr += sprintf(writePtr, " %s", args[i]);
            }
            writePtr += sprintf(writePtr, "\n");
            lineNum++;
        }
        args[0] = args[1] = args[2] = 0;
    }
}