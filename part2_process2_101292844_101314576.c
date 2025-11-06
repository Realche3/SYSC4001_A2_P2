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

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    long value = 0;
    unsigned long long cycle = 0;

    printf("[Process 2] PID=%d (Parent=%d)\n", getpid(), getppid());
    while (1) {
        printf("Cycle %llu", cycle);
        if (value % 3 == 0) printf(" – %ld is a multiple of 3\n", value);
        else printf("\n");

        value--;
        cycle++;
        usleep(250000);
    }
}
