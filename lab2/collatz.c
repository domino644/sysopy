#include "collatz.h"

int collatz_conjecture(int input)
{
    if (input % 2 == 0)
        return input / 2;
    return 3 * input - 1;
}

int test_collatz_conjecture(int input, int max_iter)
{
    int x = input;
    int counter = 0;
    while (x != 1 && counter < max_iter)
    {
        x = collatz_conjecture(x);
        counter++;
    }
    if (counter >= max_iter)
        return -1;
    return counter;
}