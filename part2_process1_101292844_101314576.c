/*
 Extend the Processes. Now, process 1 will display only multiples of 3. That is, increment the counter in
an infinite loop, calculate if it’s a multiple of 3, and display that number. Also, display the number of
cycles in each iteration. Example:
Cycle number: 0 – 0 is a multiple of 3
Cycle number: 1
Cycle number: 2
Cycle number: 3 – 3 is a multiple of 3
...
Process 2 will do the same but decrementing the value of the counter.
Use the exec system call to launch Process 2 (i.e., Process 2 should be a different program/executable).
Use delay functions to slower the display speed.
To finish the program execution, use the kill command (man pages), find the pid of both processes (ps)
and kill them.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); exit(EXIT_FAILURE); }

    if (pid == 0) {
        printf("[Process 1] Launching Process 2 using exec...\n");
        char *argv[] = {"process2", NULL};
        execv("./process2", argv);
        perror("execv failed");
        exit(EXIT_FAILURE);
    }

    unsigned long long cycle = 0;
    long value = 0;

    while (1) {
        printf("Cycle %llu", cycle);
        if (value % 3 == 0) printf(" – %ld is a multiple of 3\n", value);
        else printf("\n");

        value++;
        cycle++;
        usleep(200000);
    }
}
