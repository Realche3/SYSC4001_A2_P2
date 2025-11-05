// process2.c
// Mohamed Cherif Bah 101292844
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0); // unbuffered stdout

    printf("[Process 2] PID=%d (parent=%d). Starting decrement loop...\n", getpid(), getppid());

    unsigned long long cycle = 0; // iteration count (display every loop)
    long long value = 0;          // start at 0, then go -1, -2, ...

    while (1) {
        // Show the cycle number every time
        printf("Cycle number: %llu", cycle);

        // multiples of 3 work for negatives too (-3 % 3 == 0 in C)
        if (value % 3 == 0) {
            printf(" – %lld is a multiple of 3\n", value);
        } else {
            printf("\n");
        }

        // next iteration: decrement value, cycle++
        value--;
        cycle++;

        usleep(250000); // 250 ms to slow display (different from Process 1 so you can tell)
    }
}
