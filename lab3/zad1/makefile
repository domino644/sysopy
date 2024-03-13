TARGET_FILE?=target.txt
SOURCE_FILE?=source.txt
BYTES?=2

all: 
	gcc -o reverse reverse.c

test_1: 
	./reverse source.txt target.txt 1

test_1024: 
	./reverse source.txt target.txt 1024

test:
	./reverse $(SOURCE_FILE) $(TARGET_FILE) $(BYTES)

clean: 
	rm -rf reverse