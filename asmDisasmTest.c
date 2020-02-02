#include "assembler.h"
#include "disassembler.h"

int main() {
    assemble("test.reg", "test.out");
    disassembleFile("test.out", "test.dsm");
}