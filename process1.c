// process1.c
// Mohamed Cherif Bah

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0); // unbuffered stdout for immediate prints

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child: replace itself with process2
        printf("[Process 2 launcher] PID=%d exec'ing ./process2 ...\n", getpid());
        char *argv[] = {"process2", NULL};
        execv("./process2", argv);
        perror("execv ./process2");   // only runs if execv fails
        exit(EXIT_FAILURE);
    }

    // Parent: wait for child to finish
    printf("[Process 1] PID=%d (child PID=%d). Waiting for Process 2...\n", getpid(), pid);

    int status = 0;
    pid_t w = waitpid(pid, &status, 0);  // blocking wait
    if (w == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status)) {
        printf("[Process 1] Process 2 exited with status %d. Exiting as well.\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("[Process 1] Process 2 killed by signal %d. Exiting as well.\n", WTERMSIG(status));
    } else {
        printf("[Process 1] Process 2 ended. Exiting.\n");
    }

    return 0;
}
