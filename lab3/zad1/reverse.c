#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

unsigned long fsize(FILE *file)
{
    fseek(file, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(file);
    return len;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("%d arguments provided while 4 are required", argc);
        return 1;
    }

    unsigned int blocksize;
    FILE *source = fopen(argv[1], "r");
    FILE *target = fopen(argv[2], "w");

    if (!source)
    {
        printf("Error occured while opening file %s", argv[1]);
        return 1;
    }

    if (!target)
    {
        printf("Error occured while opening file %s", argv[2]);
        return 1;
    }

    if (!atoi(argv[3]))
    {
        printf("Impossible to convert %s into an integer", argv[3]);
        return 1;
    }

    blocksize = atoi(argv[3]);
    char buffer[blocksize];
    fseek(source, 0, SEEK_END);
    size_t read_bytes = fread(buffer, sizeof(char), blocksize, source);
    printf("%s", buffer);
    return 0;
}