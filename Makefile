CC=gcc
CFLAGS= -g -fsanitize=address -fsanitize=undefined 

BINARIES=asmDisasmTest interactiveCalc

tests: asmDisasmTest
	./asmDisasmTest

asmDisasmTest: asmDisasmTest.c assembler.c disassembler.c
	$(CC) $(CFLAGS) -o $@ $^ 

# THIS WILL NOT COMPILE WITH $(CFLAGS) BECAUSE IT IS BAD
interactiveCalc: interactiveCalc.c calc.c utility.c intStack.c stringStack.c
	$(CC) -o $@ $^ 

clean:
	rm $(BINARIES)