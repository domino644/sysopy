all: compile_static compile_shared main_dynamic main_static main_shared

compile_static:
	gcc -c collatz.c
	ar -rcs collatz.a collatz.o

compile_shared:
	gcc -fPIC -o collatz.o -c collatz.c
	gcc -shared collatz.o -o libcollatz.so

main_dynamic:
	gcc main2.c -o main_dynamic -ldl -O0

main_static:
	gcc main.c collatz.a -o main_stat -O0

main_shared:
	gcc main.c -L. -lcollatz -Wl, -o main_shared -O0

test: all
	./main_dynamic
	./main_stat
	./main_shared

clean:
	rm -rf *.o
	rm -rf main_dynamic main_shared main_stat