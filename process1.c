// process1.c
// Mohamed Cherif Bah

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>

#define SHM_KEY 0x12345  // Both programs must use the same key

typedef struct {
    int multiple;      // e.g., 3 (configurable via argv)
    long counter;      // owned/updated by Process 1; read by Process 2
} shmdata_t;

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0); // unbuffered stdout for immediate prints

    // Configure the multiple from argv (default: 3)
    int multiple = 3;
    if (argc >= 2) {
        multiple = atoi(argv[1]);
        if (multiple == 0) multiple = 3;
    }

    // Create shared memory segment
    int shmid = shmget(SHM_KEY, sizeof(shmdata_t), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach it
    shmdata_t *shmp = (shmdata_t *)shmat(shmid, NULL, 0);
    if (shmp == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Initialize shared variables
    shmp->multiple = multiple;
    shmp->counter  = 0;

    // Fork: child will exec process2
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        shmdt(shmp);
        shmctl(shmid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child -> exec ./process2
        printf("[Process 1] Launching Process 2 via exec... (multiple=%d)\n", multiple);
        char *argv2[] = {"process2", NULL};
        execv("./process2", argv2);
        perror("execv ./process2");
        _exit(127);
    }

    // Parent (Process 1) loop: increment counter, show multiples, stop when > 500
    printf("[Process 1] PID=%d (child PID=%d). multiple=%d. Starting...\n", getpid(), pid, shmp->multiple);

    unsigned long long cycle = 0;
    while (1) {
        // Display every cycle number
        printf("[Process 1] cycle=%llu, counter=%ld", cycle, shmp->counter);

        if (shmp->counter % shmp->multiple == 0) {
            printf("  -> %ld is a multiple of %d\n", shmp->counter, shmp->multiple);
        } else {
            printf("\n");
        }

        // Stop condition for both processes
        if (shmp->counter > 500) {
            printf("[Process 1] counter=%ld (>500). Finishing.\n", shmp->counter);
            break;
        }

        // Increment shared counter
        shmp->counter++;
        cycle++;

        usleep(150000); // slow things down a bit (150 ms)
    }

    // Reap child (it should exit when it sees counter > 500)
    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
    }

    // Detach and remove shared memory
    if (shmdt(shmp) == -1) {
        perror("shmdt");
    }
    // Remove the segment (marked for deletion; disappears when last attach detaches)
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID");
    }

    return 0;
}
