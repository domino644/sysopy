CC=gcc
FLAGS=-Wall -lc -lpthread -D_POSIX_C_SOURCE=199309L

all:
	$(CC) $(FLAGS) -o catcher catcher.c
	$(CC) $(FLAGS) -o sender sender.c

clean:
	rm -rf catcher sender