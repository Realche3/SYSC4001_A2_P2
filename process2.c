// process2.c
// Mohamed Cherif Bah

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

#define SHM_KEY  0x12345
#define SEM_KEY  0x23456

typedef struct {
    int  multiple;
    long counter;
} shmdata_t;

union semun {
    int              val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};

static void sem_lock(int semid) {
    struct sembuf op = { .sem_num = 0, .sem_op = -1, .sem_flg = 0 }; // P()
    if (semop(semid, &op, 1) == -1) {
        perror("[Process 2] semop - lock");
        exit(EXIT_FAILURE);
    }
}
static void sem_unlock(int semid) {
    struct sembuf op = { .sem_num = 0, .sem_op = +1, .sem_flg = 0 }; // V()
    if (semop(semid, &op, 1) == -1) {
        perror("[Process 2] semop - unlock");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    /* 1) Attach shared memory and semaphore (created by process1) */
    int shmid = shmget(SHM_KEY, sizeof(shmdata_t), 0666);
    if (shmid < 0) { perror("[Process 2] shmget"); exit(EXIT_FAILURE); }

    shmdata_t *shmp = (shmdata_t *)shmat(shmid, NULL, 0);
    if (shmp == (void *)-1) { perror("[Process 2] shmat"); exit(EXIT_FAILURE); }

    int semid = semget(SEM_KEY, 1, 0666);
    if (semid < 0) { perror("[Process 2] semget"); exit(EXIT_FAILURE); }

    int multiple;
    long snapshot;

    /* Read initial multiple under lock (not strictly necessary every time) */
    sem_lock(semid);
    multiple = shmp->multiple;
    snapshot = shmp->counter;
    sem_unlock(semid);

    printf("[Process 2] PID=%d (parent=%d). multiple=%d. Waiting for counter > 100...\n",
           getpid(), getppid(), multiple);

    /* 2) Wait until counter > 100 */
    while (1) {
        sem_lock(semid);
        snapshot = shmp->counter;
        multiple = shmp->multiple; // multiple could change; adapt if needed
        sem_unlock(semid);

        if (snapshot > 100) break;

        printf("[Process 2] waiting... counter=%ld\n", snapshot);
        usleep(200000);
    }

    /* 3) Main loop: read/print until counter > 500 */
    while (1) {
        sem_lock(semid);
        snapshot = shmp->counter;
        multiple  = shmp->multiple;

        if (snapshot > 500) {
            printf("[Process 2] counter=%ld (>500). Finishing.\n", snapshot);
            sem_unlock(semid);
            break;
        }

        if (snapshot % multiple == 0) {
            printf("[Process 2] sees counter=%ld  -> %ld is a multiple of %d\n",
                   snapshot, snapshot, multiple);
        } else {
            printf("[Process 2] sees counter=%ld\n", snapshot);
        }
        sem_unlock(semid);

        usleep(220000);
    }

    if (shmdt(shmp) == -1) perror("[Process 2] shmdt");
    return 0;
}
