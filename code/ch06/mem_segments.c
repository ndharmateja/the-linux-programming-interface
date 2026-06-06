#include <stdlib.h>
#include <stdio.h>

char global_buf[65536];      /* Uninitialized data segment */
int primes[] = {2, 3, 5, 7}; /* Initialized data segment */

static int square(int x) /* Allocated in frame for square() */
{
    int result; /* Allocated in frame for square() */

    result = x * x;
    return result; /* Return value passed via register */
}

static void do_calculation(int val) /* Allocated in frame for do_calculation() */
{
    printf("The square of %d is %d\n", val, square(val));

    if (val < 1000)
    {
        int t; /* Allocated in frame for do_calculation() */

        t = val * val * val;
        printf("The cube of %d is %d\n", val, t);
    }
}

int main(int argc, char *argv[]) /* Allocated in frame for main() */
{
    static int key = 9973;          /* Initialized data segment */
    static char m_buffer[10240000]; /* Uninitialized data segment */

    char *p;          /* Allocated in frame for main() */
    p = malloc(1024); /* Points to memory in heap */
    do_calculation(key);
    free(p);

    exit(EXIT_SUCCESS);
}