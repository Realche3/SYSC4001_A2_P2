// Mohamed Cherif Bah
// processes.c

/*
    The fork() system call is used to create a new process by duplicating the calling process.
    The new process is referred to as the child process. The child process and the parent process
    run concurrently. After a fork() call, both processes will execute the next instruction following
    the fork() call. The fork() function returns a value that helps to distinguish between the
    parent and child processes:
    - In the parent process, fork() returns the Process ID (PID) of the newly created child process.
    - In the child process, fork() returns 0.
    - If the fork() call fails, it returns -1 in the parent process, and no child process is created.

    The exec() system call is used to replace the current process image with a new process image.
    It loads a new program into the current process's memory space and starts its execution. The exec()
    family of functions (such as execl(), execp(), execv(), etc.) allows you to specify the new program to be executed
    along with its arguments and environment.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
   setvbuf(stdout, NULL, _IONBF, 0); // Disable buffering for stdout

   pid_t pid = fork(); // Create a new process
   if (pid<0){
        perror("Fork failed");
        exit(EXIT_FAILURE);
     }
     else if (pid==0){
        // Child process
        printf("Child Process: PID = %d, Parent PID = %d\n", getpid(), getppid());
        char *args[] = {"/bin/ls", "-l", NULL}; // Arguments for exec
        execv(args[0], args); // Replace child process with 'ls -l'
        // If execv returns, it means it failed
        perror("execv failed");
        exit(EXIT_FAILURE);
     }
     else {
        // Parent process
        printf("Parent Process: PID = %d, Child PID = %d\n", getpid(), pid);
        wait(NULL); // Wait for child process to finish
        printf("Child process completed.\n");
   }
}
