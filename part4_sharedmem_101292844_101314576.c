/*
Extend the processes above once more. They should now share memory. The primary functions are listed
in the book and course materials. Using shmget, shmctl, shmat, and shmdt, add two common variables
shared between the two processes. The first variable contains the value of the multiple (in the example
above: 3; that number can be changed and then the program should adapt and display the multiples of the
chosen number). The second variable contains the value of the counter used by Process 1, which is now
shared with Process 2. Process 2 starts only when the value of this variable is larger than 100.
Each of the processes should now react to the value of the shared variable and display a message
identifying themselves and numbers in shared memory. Both processes finish when the value of the
shared variable is larger than 500
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SHM_KEY 0x12345

typedef struct {
    int multiple;
    long counter;
} shmdata_t;

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    int shmid = shmget(SHM_KEY, sizeof(shmdata_t), IPC_CREAT | 0666);
    if (shmid < 0) { perror("shmget"); exit(EXIT_FAILURE); }

    shmdata_t *shmp = shmat(shmid, NULL, 0);
    if (shmp == (void *)-1) { perror("shmat"); exit(EXIT_FAILURE); }

    shmp->multiple = 3;
    shmp->counter = 0;

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); exit(EXIT_FAILURE); }

    if (pid == 0) {
        // Process 2
        while (shmp->counter <= 100) {
            printf("[Process 2] waiting... counter=%ld\n", shmp->counter);
            usleep(200000);
        }

        while (shmp->counter <= 500) {
            printf("[Process 2] counter=%ld\n", shmp->counter);
            usleep(250000);
        }

        printf("[Process 2] Finished (counter=%ld)\n", shmp->counter);
        shmdt(shmp);
        exit(EXIT_SUCCESS);
    } else {
        // Process 1
        while (shmp->counter <= 500) {
            printf("[Process 1] counter=%ld", shmp->counter);
            if (shmp->counter % shmp->multiple == 0)
                printf(" – multiple of %d\n", shmp->multiple);
            else
                printf("\n");

            shmp->counter++;
            usleep(150000);
        }

        wait(NULL);
        shmdt(shmp);
        shmctl(shmid, IPC_RMID, NULL);
        printf("[Process 1] Done.\n");
    }
}
