CC=gcc
CFLAGS= -g -fsanitize=address -fsanitize=undefined 

BINARIES=asmDisasmTest interactiveCalc emulator ass

default: runAsmDisasmTest emulator

runAsmDisasmTest: asmDisasmTest
	./asmDisasmTest

emulatorTest: emulator runAsmDisasmTest
	./emulator test.out

asmDisasmTest: asmDisasmTest.c assembler.c disassembler.c
	$(CC) $(CFLAGS) -o $@ $^

emulator: emulator.c
	$(CC) $(CFLAGS) -o $@ $^

ass: assembler.c
	$(CC) $(CFLAGS) -o $@ $^

dsm: disassembler.c
	$(CC) $(CFLAGS) -o $@ $^

# THIS WILL NOT COMPILE WITH $(CFLAGS) BECAUSE IT IS BAD
interactiveCalc: interactiveCalc.c calc.c utility.c intStack.c stringStack.c
	$(CC) -o $@ $^ 

clean:
	rm $(BINARIES)