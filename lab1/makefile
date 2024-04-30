.DEFAULT_GOAL := countdown.c
CC=gcc
targets=countdown.o

all: $(targets)

countdown.o:
	CC -o ./countdown.o ./countdown.c
	./countdown.o

run: ./countdown.c
	./countdown.o

test: ./countdown.c
	./countdown.o

clean:
	rm -rf ./countdown.o

