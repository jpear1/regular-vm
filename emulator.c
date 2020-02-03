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
                registers[b1] = *(int16_t*)(&b2);
                break;
            // mov rA rB
            case 0x0C:
                registers[b1] = registers[b2];
                break;
            // ldw rA rB
            case 0x0D:
                registers[b1] = *(uint32_t*)(&memory[b2]);
                break;
            // stw rA rB
            case 0x0E:
                *(uint32_t*)(&memory[b1]) = registers[b2];
                break;
            // ldb rA rB
            case 0x0F:
                *((char*)(&registers[b1]) + 3) = memory[b2];
                break;
            // stb rA rB
            case 0x10:
                memory[b1] = *((char*)(&registers[b2]) + 3);
                break;
        }
        printState();
        fread(instruction, 4, 1, inFile);
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