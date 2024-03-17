DIR?=.
TIME_FILE?=pomiar_zad2.txt

all:
	gcc -o my_ls my_ls.c
test:
	./my_ls $(DIR)
time:
	time -o $(TIME_FILE) ./my_ls $(DIR)
clean:
	rm -rf my_ls