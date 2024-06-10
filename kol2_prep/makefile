CC=gcc
FLAGS=-Wall -fsanitize=address -pthread

.PHONY: all clean

all: server client
	
client:
	$(CC) $(FLAGS) -o cli cli.c

server:
	$(CC) $(FLAGS) -o srv srv.c

clean:
	rm -f srv cli


