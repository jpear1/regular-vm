CC=gcc
CFLAGS= -g -fsanitize=address -fsanitize=undefined 

BINARIES=disassemblerTests 

tests: disassemblerTests
	./disassemblerTests


disassemblerTests: disassemblerTests.c disassembler.c
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm $(BINARIES)