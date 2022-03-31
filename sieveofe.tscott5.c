#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define ARRAY_SIZE 1000
#define BUFFER_SIZE 32

typedef struct ThreadStruct {

    int upperLimit;
    int lowerLimit;
    int sValue;
    char numArray[ARRAY_SIZE];

} ThreadData;

void *boss(void *param);
void *runner(void *param);

int main() {

    // Threads (Maybe thread for each number not crossed under sValue, so like if length = 100, only need 4 thread for 2-3-5-7)
    ThreadData *threadData = (ThreadData *) malloc(sizeof(ThreadData));
    threadData->upperLimit = ARRAY_SIZE;
    threadData->lowerLimit = 2;
    threadData->sValue = sqrt(ARRAY_SIZE) + 1;

    // Mutex
    pthread_mutex_t mutex;
    pthread_t tid1, tid2;
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&tid2, NULL, boss, &mutex); // Starts a new thread in the calling process
    sleep(1);
    pthread_create(&tid1, NULL, runner, &mutex);

    pthread_join(tid1, NULL); // Waits for the specified thread to terminate
    pthread_join(tid2, NULL);

    // Shared Mem (use to store the next integer that hasn't been crossed off yet. Thread 1 starts at 2, thread 2 at 3, thread 3 at 5, etc)
    int memid;
    int key = IPC_PRIVATE;
    char buffer[BUFFER_SIZE];
    char *ptr; // to shared mem
    memid = shmget(key, BUFFER_SIZE, IPC_EXCL | 0666);
    if (memid < 0) {
        printf("shmget() failed\n");
        return(8);
    }
    // TODO: Declare the ptr like this in each thread to gain access to shared memory 
    // ptr = (char *) shmat(memid, 0, 0);


    // Fill array with numbers to be checked 
    for (int i = 1; i <= threadData->upperLimit; i++) {

        threadData->numArray[i] = 1;
    }

    // Starts at lower limit, iterates only until sValue is hit
    for (int i = threadData->lowerLimit; i < threadData->sValue; i++) {

        // If current number is not crossed off
        if (threadData->numArray[i] != 0) {

            // Check all multiples of the current number until the end of array and cross off with a 0
            for (int j = i * i; j <= threadData->upperLimit; j += i) {
                threadData->numArray[j] = 0;
            }     
        }
    }

    for (int i = threadData->lowerLimit; i <= threadData->upperLimit; i++) {

        // True prints out all primes, false prints out all composites
        if (threadData->numArray[i] == 1) {
            printf("%d\n", i);
        }
    }   

    return 0;
}

void *boss(void *param) {
  pthread_mutex_t *mutex;

  mutex = (pthread_mutex_t *) param;

  printf("I am boss, locking the mutex\n");
  pthread_mutex_lock(mutex);

  printf("I am boss, sleeping for 5 seconds\n");
  sleep(5);
  printf("I am boss, unlocking the mutex\n");
  pthread_mutex_unlock(mutex);
  sleep(1);

  pthread_exit(0);
}

void *runner(void *param) {
  pthread_mutex_t *mutex;

  mutex = (pthread_mutex_t *) param;

  printf("I am runner: waiting for the mutex\n");
  pthread_mutex_lock(mutex);
  printf("I am runner: done\n");

  pthread_exit(0);
}
