.DEFAULT_GOAL := countdown.c
CC=gcc
targets=countdown

all: $(targets)

countdown:
	CC -o ./lab0/countdown ./lab0/countdown.c
	./lab0/countdown

run: ./lab0/countdown.c
	./lab0/countdown

test: ./lab0/countdown.c
	./lab0/countdown

clean:
	rm -rf ./lab0/countdown

