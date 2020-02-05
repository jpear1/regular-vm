#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <inttypes.h>

#include "emulator.h"

uint32_t registers[32] = {0};

char memory[16384] = {0};

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./emulator <binary>");
        exit(-1);
    }
    return runFile(argv[1]);
}

int runFile(char const *fileName) {
    FILE *inFile = fopen(fileName, "r");
    if (inFile == NULL) {
        fprintf(stderr, "Invalid input file : %s", fileName);
        exit(-1);
    }
    char instruction[4];
    fread(instruction, 4, 1, inFile);
    char b1 = instruction[1];
    char b2 = instruction[2];
    char b3 = instruction[3];
    uint32_t tmp;
    while (!feof(inFile)) {
        switch (instruction[0]) {
            // nop
            case 0x00: 
                break;
            // add rA rB rC
            case 0x01:
                registers[b1] = registers[b2] + registers[b3];
                break;
            // sub rA rB rC
            case 0x02:
                registers[b1] = registers[b2] - registers[b3];
                break;
            // and rA rB rC
            case 0x03:
                registers[b1] = registers[b2] & registers[b3];
                break;
            // orr rA rB rC
            case 0x04:
                registers[b1] = registers[b2] | registers[b3];
                break;
            // xor rA rB rC
            case 0x05:
                registers[b1] = registers[b2] ^ registers[b3];
                break;
            // not rA rB
            case 0x06:
                registers[b1] = ~registers[b2];
                break;
            // lsh rA rB rC
            case 0x07:
                if (registers[b3] > 0) {
                    registers[b1] = registers[b2] << registers[b3];
                } else if (registers[b3] < 0) {
                    registers[b1] = registers[b2] >> -registers[b3];
                }
                break;
            // ash rA rB rC
            case 0x08:
                if (registers[b3] > 0) {
                    registers[b1] = registers[b2] << registers[b3];
                } else if (registers[b3] < 0) {
                    registers[b1] = (int32_t) registers[b2] >> -registers[b3]; // cast to a signed int to do arithmetic shift
                }
                break;
            // tcu rA rB rC
            case 0x09:
                tmp = registers[b2] - registers[b3];
                registers[b1] = tmp > registers[b2] ? UINT32_MAX : 0;
                break;
            // tcs rA rB rC
            case 0x0A:
                tmp = (int32_t) registers[b2] - (int32_t) registers[b3];
                registers[b1] = tmp & 0x80000000 ? UINT32_MAX : 0; // 
                break;
            // set rA imm
            case 0x0B:
                memcpy(&registers[b1], &b2, 1);
                memcpy(((char*)&registers[b1])+1, &b3, 1);
                memset(((char*)&registers[b1])+2, b3 >= 0? 0x00 : 0xff, 2);
                break;
            // mov rA rB
            case 0x0C:
                registers[b1] = registers[b2];
                break;
            // ldw rA rB
            case 0x0D:
                memcpy(&registers[b1], &memory[b2], 4);
                break;
            // stw rA rB
            case 0x0E:
                memcpy(&memory[b1], &registers[b2], 4);
                break;
            // ldb rA rB
            case 0x0F:
                memcpy(&registers[b1], &memory[b2], 1);
                break;
            // stb rA rB
            case 0x10:
                memcpy(&memory[b1], &registers[b2], 1);
                break;
        }
        printState();
        fread(instruction, 4, 1, inFile);
        b1 = instruction[1];
        b2 = instruction[2];
        b3 = instruction[3];
    }
}

void printState() {
    printf("----------------------------CURRENT STATE--------------------------\n");
    char valueString[50];
    const char spaces[] = "                ";
    char printString[50];
    for (int r = 0; r < 32; r++) {
        sprintf(printString, "r%i: ", r);
        if (r < 10)
            strcat(printString, " ");
        sprintf(valueString, "%i", registers[r]);
        strcat(printString, valueString);
        strncat(printString, spaces, 15 - strlen(valueString));
        printf("%s", printString);
        if (r % 4 == 3)
            printf("\n");
    }
    printf("\n");
}