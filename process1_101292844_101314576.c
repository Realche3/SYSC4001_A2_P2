/*
Extend the processes above once more. They should now protect concurrent access to the shared
memory positions. On top of the shm instructions, you should protect the shared memory access using
semaphores. Use semget, semop, semctl to protect the shared memory section
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <errno.h>

#define SHM_KEY  0x12345      // must match in both programs
#define SEM_KEY  0x23456      // must match in both programs

typedef struct {
    int  multiple;            // e.g., 3
    long counter;             // updated by Process 1, read by Process 2
} shmdata_t;

/* SysV semctl requires union semun on many systems */
union semun {
    int              val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};

static void sem_lock(int semid) {
    struct sembuf op = { .sem_num = 0, .sem_op = -1, .sem_flg = 0 }; // P()
    if (semop(semid, &op, 1) == -1) {
        perror("semop - lock");
        exit(EXIT_FAILURE);
    }
}
static void sem_unlock(int semid) {
    struct sembuf op = { .sem_num = 0, .sem_op = +1, .sem_flg = 0 }; // V()
    if (semop(semid, &op, 1) == -1) {
        perror("semop - unlock");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);

    /* 1) Create shared memory */
    int shmid = shmget(SHM_KEY, sizeof(shmdata_t), IPC_CREAT | 0666);
    if (shmid < 0) { perror("shmget"); exit(EXIT_FAILURE); }

    shmdata_t *shmp = (shmdata_t *)shmat(shmid, NULL, 0);
    if (shmp == (void *)-1) { perror("shmat"); exit(EXIT_FAILURE); }

    /* 2) Create semaphore set with 1 semaphore (mutex) and init to 1 */
    int semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (semid < 0) { perror("semget"); exit(EXIT_FAILURE); }

    union semun arg;
    arg.val = 1; // binary semaphore unlocked
    if (semctl(semid, 0, SETVAL, arg) == -1) { perror("semctl SETVAL"); exit(EXIT_FAILURE); }

    /* 3) Initialize shared data (under lock for cleanliness) */
    int multiple = 3;
    if (argc >= 2) {
        int m = atoi(argv[1]);
        if (m != 0) multiple = m;
    }

    sem_lock(semid);
    shmp->multiple = multiple;
    shmp->counter  = 0;
    sem_unlock(semid);

    /* 4) Fork and exec process2 */
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        shmdt(shmp);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        printf("[Process 1] Launching process2...\n");
        char *argv2[] = {"process2", NULL};
        execv("./process2", argv2);
        perror("execv ./process2");
        _exit(127);
    }

    printf("[Process 1] PID=%d (child=%d), multiple=%d. Starting loop...\n",
           getpid(), pid, multiple);

    /* 5) Main loop: increment counter, print multiples, stop when > 500 */
    unsigned long long cycle = 0;
    while (1) {
        long current;
        int cur_multiple;

        sem_lock(semid);
        current = shmp->counter;
        cur_multiple = shmp->multiple;

        printf("[Process 1] cycle=%llu, counter=%ld", cycle, current);
        if (current % cur_multiple == 0) {
            printf("  -> %ld is a multiple of %d\n", current, cur_multiple);
        } else {
            printf("\n");
        }

        if (current > 500) {
            printf("[Process 1] counter=%ld (>500). Finishing.\n", current);
            sem_unlock(semid);
            break;
        }

        /* update shared counter */
        shmp->counter = current + 1;
        sem_unlock(semid);

        cycle++;
        usleep(150000);
    }

    /* 6) Wait for child, then cleanup */
    int status = 0;
    if (waitpid(pid, &status, 0) < 0) perror("waitpid");

    if (shmdt(shmp) == -1) perror("shmdt");
    if (shmctl(shmid, IPC_RMID, NULL) == -1) perror("shmctl IPC_RMID");
    if (semctl(semid, 0, IPC_RMID) == -1) perror("semctl IPC_RMID");

    return 0;
}
