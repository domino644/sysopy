#include <stdio.h>
extern int test_collatz_conjecture(int, int);

int main(int argc, char *argv[])
{
    int n;
    printf("Podaj liczbe do sprawdzenia: \n");
    scanf("%d", &n);
    printf("%d\n", test_collatz_conjecture(n, 100));
    return 0;
}