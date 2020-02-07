#include <stdio.h>
#include <stdlib.h>

#include "emulator.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./emu <binary>");
        exit(-1);
    }
    return runFile(argv[1]);
}