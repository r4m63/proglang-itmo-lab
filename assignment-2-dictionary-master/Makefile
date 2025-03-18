.PHONY: remove main test

all: main

main.o: main.asm lib.inc word.inc dict.inc
dict.o: dict.asm lib.inc
word.inc: colon.inc


%.o: %.asm
	nasm -g $< -felf64 -o $@  

main: lib.o dict.o main.o
	ld -o $@ $^

test: main
	python3 test.py

remove:	
	rm main.o lib.o dict.o
