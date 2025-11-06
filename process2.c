// process2.c
// Mohamed Cherif Bah

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define SHM_KEY 0x12345  // Must match process1.c

typedef struct {
    int multiple;      // e.g., 3
    long counter;      // owned by Process 1; read-only here
} shmdata_t;

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0); // unbuffered stdout

    // Find the shared segment created by Process 1
    int shmid = shmget(SHM_KEY, sizeof(shmdata_t), 0666);
    if (shmid < 0) {
        perror("[Process 2] shmget");
        exit(EXIT_FAILURE);
    }

    // Attach it
    shmdata_t *shmp = (shmdata_t *)shmat(shmid, NULL, 0);
    if (shmp == (void *)-1) {
        perror("[Process 2] shmat");
        exit(EXIT_FAILURE);
    }

    printf("[Process 2] PID=%d (parent=%d). Attached to shared memory. multiple=%d\n",
           getpid(), getppid(), shmp->multiple);

    // Wait until counter > 100 before starting
    while (shmp->counter <= 100) {
        printf("[Process 2] waiting... counter=%ld (start when >100)\n", shmp->counter);
        usleep(200000); // 200 ms
    }

    // Now react to the shared counter and multiple; stop when counter > 500
    while (1) {
        long c = shmp->counter;  // read a snapshot (no locking needed for this demo)

        if (c > 500) {
            printf("[Process 2] counter=%ld (>500). Finishing.\n", c);
            break;
        }

        // Identify ourselves and what we see
        if (c % shmp->multiple == 0) {
            printf("[Process 2] sees counter=%ld  -> %ld is a multiple of %d\n",
                   c, c, shmp->multiple);
        } else {
            printf("[Process 2] sees counter=%ld\n", c);
        }

        usleep(220000); // a different delay so outputs interleave nicely
    }

    // Detach (segment will be removed by Process 1 after both detach)
    if (shmdt(shmp) == -1) {
        perror("[Process 2] shmdt");
    }

    return 0;
}
