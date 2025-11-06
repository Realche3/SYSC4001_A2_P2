// process2.c
// Mohamed Cherif Bah

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0); // unbuffered stdout

    printf("[Process 2] PID=%d (parent=%d). Starting decrement loop...\n", getpid(), getppid());

    unsigned long long cycle = 0; // iteration number (display every loop)
    long long value = 0;          // start at 0 and go negative

    while (1) {
        // always show cycle number
        printf("Cycle number: %llu", cycle);

        // multiples of 3 check (works for negatives in C: -3 % 3 == 0)
        if (value % 3 == 0) {
            printf(" – %lld is a multiple of 3\n", value);
        } else {
            printf("\n");
        }

        // stop condition
        if (value < -500) {
            printf("[Process 2] Reached value %lld (< -500). Exiting.\n", value);
            break;
        }

        // prepare next iteration
        value--;   // decrementing
        cycle++;

        usleep(200000); // 200 ms delay to slow display
    }

    return 0; // exit status 0; parent will see this in waitpid
}
