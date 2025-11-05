// process1.c
// Mohamed Cherif Bah 101292844
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0); // unbuffered stdout

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child -> replace itself with process2
        printf("[Process 2 launcher] PID=%d exec'ing ./process2 ...\n", getpid());
        char *argv[] = {"process2", NULL};
        execv("./process2", argv);
        // If we get here, exec failed
        perror("execv ./process2");
        exit(EXIT_FAILURE);
    }

    // Parent -> Process 1: incrementing
    printf("[Process 1] PID=%d (child PID=%d). Starting increment loop...\n", getpid(), pid);

    unsigned long long cycle = 0;     // iteration count (display every loop)
    long long value = 0;              // the value we test & display when multiple of 3

    while (1) {
        // Show the cycle number every time
        printf("Cycle number: %llu", cycle);

        // Show multiple-of-3 message only when true
        if (value % 3 == 0) {
            printf(" – %lld is a multiple of 3\n", value);
        } else {
            printf("\n");
        }

        // next iteration: increment value, cycle++
        value++;
        cycle++;

        usleep(200000); // 200 ms to slow display
    }
}
