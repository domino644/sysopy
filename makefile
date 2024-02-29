.DEFAULT_GOAL := countdown.c
CC=gcc
targets=countdown.o

all: $(targets)

countdown.o:
	CC -o ./lab0/countdown.o ./lab0/countdown.c
	./lab0/countdown.o

run: ./lab0/countdown.c
	./lab0/countdown.o

test: ./lab0/countdown.c
	./lab0/countdown.o

clean:
	rm -rf ./lab0/countdown.o

