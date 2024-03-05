.DEFAULT_GOAL := countdown.c
CC=gcc
targets=countdown.o

all: $(targets)

countdown.o:
	CC -o ./lab1/countdown.o ./lab1/countdown.c
	./lab1/countdown.o

run: ./lab1/countdown.c
	./lab1/countdown.o

test: ./lab1/countdown.c
	./lab1/countdown.o

clean:
	rm -rf ./lab1/countdown.o

