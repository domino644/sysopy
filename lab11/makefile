CC=gcc
FLAGS=-Wall -fsanitize=address

.PHONY: all clean

all: server client
	
client:
	$(CC) $(FLAGS) -o client client.c

server:
	$(CC) $(FLAGS) -o server server.c

clean:
	rm -f server client


