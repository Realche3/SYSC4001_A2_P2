/*
Extend the processes above once more. Use the wait system call. Process 1 starts as in 2, and when
Process 2 starts, it waits for it. Process 2 runs until it reaches a value lower than -500. When this
happens, Process 1 should end too.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); exit(EXIT_FAILURE); }

    if (pid == 0) {
        // Child Process (Process 2)
        printf("[Process 2] PID=%d (Parent=%d)\n", getpid(), getppid());
        long value = 0;
        unsigned long long cycle = 0;
        while (value > -500) {
            printf("Cycle %llu", cycle);
            if (value % 3 == 0) printf(" – %ld is a multiple of 3\n", value);
            else printf("\n");

            value--;
            cycle++;
            usleep(200000);
        }
        printf("[Process 2] Value below -500. Exiting.\n");
        exit(EXIT_SUCCESS);
    } else {
        // Parent Process (Process 1)
        printf("[Process 1] Waiting for Process 2 to finish...\n");
        wait(NULL);
        printf("[Process 1] Process 2 finished. Exiting.\n");
    }
}
