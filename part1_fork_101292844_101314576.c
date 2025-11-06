// part1_fork_101292844_101314576.c
/*
    This program demonstrates the use of fork() to create a child process.
    Both the parent and child processes increment a counter independently
    and print their progress to the console.
*/
/*
 Run two concurrent processes in C/C++ under Linux
Using the fork system call (page 472 of the Linux book), create two independent processes, which run
indefinitely. Process 1 will run forever, will initialize a counter at 0, and will increment it in each cycle of
an infinite loop. Process 2 will run forever, will initialize a counter at 0, and will increment it in each cycle
of an infinite loop. Use delay functions to slow the display speed.
To finish the program, use the kill command (man pages), find the PID of both processes (ps) and kill
them.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0); // disable stdout buffering

    pid_t pid = fork(); // create a second process
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    unsigned long long counter = 0;

    if (pid == 0) {
        // Child Process (Process 2)
        printf("[Process 2] PID=%d (Parent=%d)\n", getpid(), getppid());
        while (1) {
            counter++;
            if (counter % 1000000ULL == 0ULL) {
                printf("[Process 2] counter=%llu\n", counter);
                usleep(200000);
            }
        }
    } else {
        // Parent Process (Process 1)
        printf("[Process 1] PID=%d (Child=%d)\n", getpid(), pid);
        while (1) {
            counter++;
            if (counter % 1000000ULL == 0ULL) {
                printf("[Process 1] counter=%llu\n", counter);
                usleep(300000);
            }
        }
    }
}
