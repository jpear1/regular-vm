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
#define RETVAL "r29"
#define MACRO1 "r28" // used by push, pop, 
#define MACRO2 "r27" // used by arithmetic
#define MACRO3 "r26" // used by function call
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

    doJzPass(inString, outString);
    refreshInOutStrings(&inString, &outString);
    doFunctionDefPass(inString, outString);
    refreshInOutStrings(&inString, &outString);
    doFunctionCallPass(inString, outString);
    refreshInOutStrings(&inString, &outString);
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

        int willGenerateInstruction = 1;
        for (int i = 0; i < sizeof(args)/sizeof(args[0]) && args[i] != NULL; i++) {
            char* pIndex = index(args[i], '+');
            char* mIndex = index(args[i], '-');
            if (pIndex || mIndex) {
                willGenerateInstruction = 0;
                insertArithmeticForArg(args[i], &writePtr);
                if (pIndex)
                    writePtr += sprintf(writePtr, "add");
                else
                    writePtr += sprintf(writePtr, "sub");
                for (int j = 0; j < i; j++) {
                    writePtr += sprintf(writePtr, " %s", args[j]);
                }
                writePtr += sprintf(writePtr, " "MACRO2);
                for (int j = i+1; j < sizeof(args)/sizeof(args[0]) && args[j]; j++) {
                    writePtr += sprintf(writePtr, " %s", args[j]);
                }
                writePtr += sprintf(writePtr, "\n");
                break;
            }
        }
        if (willGenerateInstruction) {
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
}

void insertArithmeticForArg(char *arg, char **writePtr) {
    char op;
    int reg, num;
    sscanf(arg, "r%d%c%d", &reg, &op, &num);
    *writePtr += sprintf(*writePtr, "set "MACRO1" %d\n", num);
    if (op == '+')
            *writePtr += sprintf(*writePtr, "add "MACRO2" r%d "MACRO1"\n", reg);
    else
            *writePtr += sprintf(*writePtr, "sub "MACRO2" r%d "MACRO1"\n", reg);
    *strchr(arg, op) = '\0';
}

void refreshInOutStrings(char **inString, char **outString) {
    free(*inString);
    *inString = *outString;
    int len = strlen(*inString); 
    *outString = malloc(10 * len);
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
                writePtr += sprintf(writePtr, "set "PC" %d\n",  (*(int *)entry.dptr)-1); // subtract one because the pc will automatically increment
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

int doFunctionDefPass(char *inString, char *outString) {
    char *instruction, *saveptr;
    char *writePtr = outString;
    char *args[3] = {0};
    char spilledRegisters[32];
    int len = strlen(inString), argc, lineNum = 0;

    for (char *line = strtok_r(inString, "\n", &saveptr); line != NULL; line = strtok_r(NULL, "\n", &saveptr)) {
        instruction = strtok(line, " ");
        argc = getArgcForInstruction(instruction);

        for (int i = 0; i < sizeof(args)/sizeof(args[0]); i++) {
            args[i] = i >= argc ? NULL : strtok(NULL, " ");
        }

        if (strcmp(instruction, "func") == 0) {
            writePtr += sprintf(writePtr, "jmp func_%s_end\n", args[0]);
            writePtr += sprintf(writePtr, "lab func_%s_start\n", args[0]);
            writePtr += sprintf(writePtr, "mov "FP" "SP"\n");
            identifySpilledRegisters(saveptr, spilledRegisters);
            for (int i = 0; i < 32; i++) {
                if (spilledRegisters[i])
                    writePtr += sprintf(writePtr, "push r%i\n", i);
            }
        } else if (strcmp(instruction, "endfunc") == 0) {
            writePtr += sprintf(writePtr, "lab func_%s_exit\n", args[0]);
            for (int i = 31; i >= 0; i--) {
                if (spilledRegisters[i] && i != 29) // r29 is RETVAL register
                    writePtr += sprintf(writePtr, "pop r%i\n", i);
            }
            writePtr += sprintf(writePtr, "pop "FP"\n");
            writePtr += sprintf(writePtr, "pop "PC"\n");
            writePtr += sprintf(writePtr, "lab func_%s_end\n", args[0]);
        } else if (strcmp(instruction, "ret") == 0) {
            writePtr += sprintf(writePtr, "jmp func_%s_exit\n", args[0]);
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

int identifySpilledRegisters(const char *start, char *spilledRegisters) {
    memset(spilledRegisters, 0, 32);
    char *instruction, *saveptr;
    char *args[3] = {0};
    size_t len = strlen(start) + 1;
    char *code = malloc(len);
    memcpy(code, start, len);
    for (char *line = strtok_r(code, "\n", &saveptr); line != NULL; line = strtok_r(NULL, "\n", &saveptr)) {
        instruction = strtok(line, " ");
        int argc = getArgcForInstruction(instruction);

        for (int i = 0; i < sizeof(args)/sizeof(args[0]); i++) {
            args[i] = i >= argc ? NULL : strtok(NULL, " ");
        }

        if (strcmp(instruction, "endfunc") == 0) {
            free(code);
            return 0;
        }

        int reg;
        if (args[0]) {
            int isRegister = sscanf(args[0], "r%i", &reg);
            if (isRegister && reg != 29 && reg != 0) // r29 is RETVAL and r0 is PC
                spilledRegisters[reg] = 1;
        }
    }
    printf("unclosed function definition\n");
    exit(-1);
}

int doFunctionCallPass(char *inString, char *outString) {
    char *instruction, *saveptr;
    char *writePtr = outString;
    char *args[3] = {0};
    char spilledRegisters[32];
    int len = strlen(inString), argc, lineNum = 0;

    for (char *line = strtok_r(inString, "\n", &saveptr); line != NULL; line = strtok_r(NULL, "\n", &saveptr)) {
        instruction = strtok(line, " ");
        argc = getArgcForInstruction(instruction);

        for (int i = 0; i < sizeof(args)/sizeof(args[0]); i++) {
            args[i] = i >= argc ? NULL : strtok(NULL, " ");
        }

        if (strcmp(instruction, "call") == 0) {
            writePtr += sprintf(writePtr, "mov "MACRO1" "PC"\n");
            writePtr += sprintf(writePtr, "set "MACRO3" 10\n"); // 10 is the length of a fully expanded call function
            writePtr += sprintf(writePtr, "add "MACRO3" "MACRO3" "MACRO1"\n");
            writePtr += sprintf(writePtr, "push "MACRO3"\n");
            writePtr += sprintf(writePtr, "push "FP"\n");
            writePtr += sprintf(writePtr, "jmp func_%s_start\n", args[0]);
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

int doJzPass(char *inString, char *outString) {
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

        if (strcmp(instruction, "jz") == 0) {
            writePtr += sprintf(writePtr, "set "MACRO1" 0\n");
            writePtr += sprintf(writePtr, "tcu "MACRO1" %s "MACRO1"\n", args[0]);
            writePtr += sprintf(writePtr, "set "MACRO2" 1\n");
            writePtr += sprintf(writePtr, "and "MACRO1" "MACRO2" "MACRO1"\n");
            writePtr += sprintf(writePtr, "add "PC" "PC" "MACRO1"\n");
            writePtr += sprintf(writePtr, "jmp %s\n", args[1]);
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