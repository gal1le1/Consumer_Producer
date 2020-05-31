#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
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
    if (argc != 1)
    {
        printf("Please follow the rules");
        exit(EXIT_FAILURE);
    }

    sem_t *empty; // counts number of empty places
    sem_t *full; // counts number of full places
    sem_t *mutex; //ensures mutual exclusion

    // creates named semaphores so it can be used by other processes with same name
    empty = sem_open("empty", O_CREAT , 0644, SIZE);
    full = sem_open("full", O_CREAT , 0644, 0);
    mutex = sem_open("mutex", O_CREAT , 0644, 1);

    struct container *shm_addr;  //declaration of shared memory
    int shm_id; //shared memory id
    
    shm_id = shmget(SHM_KEY_BUF, sizeof(struct container), 0644|IPC_CREAT); // gets the identifier of the shared memory segment with the size of given struct
    if (shm_id == -1){
        perror("Shared memory");
        exit(EXIT_FAILURE);
    }
   
    // Attaches to the segment to get a pointer to it
    shm_addr = shmat(shm_id, NULL, 0);
    if (shm_addr == (void *) -1) {
        perror("Shared memory attach");
        exit(EXIT_FAILURE);
    }   

    sem_wait(full); // decreases full places
    sem_wait(mutex); // ensures mutual exclusion

    // reads first available number from array
    printf("%d is the next value\n", shm_addr->array[shm_addr->position]);
    
    if (shm_addr->position == SIZE-1)
    {
        shm_addr->position = 0; // at the end of array returns index back to 0
    }
    else
    {
        shm_addr->position++; // increases next available place index
    }

    sem_post(mutex); // ensures mutual exclusion
    sem_post(empty); //increases empty places

    return 0;
}