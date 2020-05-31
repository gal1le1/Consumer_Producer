#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#define SHM_KEY_BUF 1234
#define SIZE 5

struct container
{
    int next;
    int position;
    int array[SIZE];
};

int main(int argc, char const *argv[])
{
    sem_t *empty; // counts number of empty places
    sem_t *full; // counts number of full places
    sem_t *mutex; //ensures mutual exclusion

    empty = sem_open("empty", O_CREAT , 0644, SIZE);
    full = sem_open("full", O_CREAT , 0644, 0);
    mutex = sem_open("mutex", O_CREAT , 0644, 1);

    int shm_id;
    struct container *shm_addr;

    shm_id = shmget(SHM_KEY_BUF, sizeof(struct container), 0644|IPC_CREAT);
    if (shm_id == -1){
        perror("Shared memory");
        exit(EXIT_FAILURE);
    }

    // Attach to the segment to get a pointer to it
    shm_addr = shmat(shm_id, NULL, 0);
    if (shm_addr == (void *) -1) {
        perror("Shared memory attach");
        exit(EXIT_FAILURE);
    }

    sem_destroy(empty);
    sem_destroy(full);
    sem_destroy(mutex);

    //-----------
    if (shmdt(shm_addr) == -1) {
        perror("shmdt1");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shm_id, IPC_RMID, 0) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    //-----------             
  

    return 0;
}
