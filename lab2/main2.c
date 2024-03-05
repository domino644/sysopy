#include <dlfcn.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    void *hook = dlopen("./libcollatz.so", RTLD_LAZY);
    if (!hook)
    {
        printf("Error when opening library\n");
        return 1;
    }

    int (*fun)(int, int);
    fun = dlsym(hook, "test_collatz_conjecture");
    if (dlerror() != 0)
    {
        printf("Error with function");
        return 1;
    }
    int n;
    printf("Podaj liczbe do sprawdzenia: \n");
    scanf("%d", &n);
    printf("%d\n", fun(n, 100));
    return 0;
}