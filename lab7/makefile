CC=gcc
FLAGS=-Wall

.PHONY = clean all server kill_icps

all:
	$(CC) $(FLAGS) -o server server.c
	$(CC) $(FLAGS) -o client client.c

server:
	$(CC) $(FLAGS) -o server server.c

clean:
	rm server client
	./cleaner.sh

kill_icps:
	./cleaner.sh