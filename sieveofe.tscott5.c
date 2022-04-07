#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define LOWER_LIMIT 2
#define UPPER_LIMIT 30
#define N_THREADS 5 // Program does not work if threads aren't >= sValue

// Structs
typedef struct ThreadStruct {

    int lowerLimit;
    int upperLimit;
    int sValue;
    char numArray[UPPER_LIMIT];
    pthread_mutex_t mutex;
    //static *int nextInt;

} ThreadData;

void *primesInRange(void *param);
int waitChild();

int nextInt = LOWER_LIMIT;
int childDone; 

int main() {

    // Threads (Maybe thread for each number not crossed under sValue, so like if length = 100, only need 4 thread for 2-3-5-7)
    ThreadData *threadData = (ThreadData *) malloc(sizeof(ThreadData));
    threadData->upperLimit = UPPER_LIMIT;
    threadData->lowerLimit = LOWER_LIMIT;
    threadData->sValue = sqrt(UPPER_LIMIT) + 1;

    // Mutex
    pthread_mutex_t mutex = threadData->mutex;
    pthread_t threadId[N_THREADS];
    pthread_mutex_init(&mutex, NULL);

    // Fill array with numbers to be checked 
    for (int i = 1; i <= threadData->upperLimit; i++) {
        threadData->numArray[i] = 1;
    }

    for (int i = 1; i <= N_THREADS; i++) {
        childDone = 0;
        pthread_create(&threadId[i], NULL, primesInRange, threadData);
            
        // Prints out all numbers crossed off so far
        printf("\nThread Number: %d\n", i);
        while(!childDone);

    }

    for (int i = 1; i <= N_THREADS; i++) {
        pthread_join(threadId[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    printf("List of all primes in range %d - %d: \n", threadData->lowerLimit, threadData->upperLimit);
    for (int i = threadData->lowerLimit; i <= threadData->upperLimit; i++) {

        // True prints out all primes, false prints out all composites
        if (threadData->numArray[i] == 1) {
            printf("%d\n", i);
        }
    } 

    return 0;
}

void *primesInRange(void *param) {

    ThreadData *threadData = (ThreadData *) malloc(sizeof(ThreadData));
    threadData = (ThreadData *) param;
    pthread_mutex_t mutex = threadData->mutex;

    printf("Candidate Integer: %d\n", nextInt);
    printf("Composite Numbers Checked So Far: \n");
    if (nextInt <= threadData->sValue) {
        
        // If current number is not crossed off
        if (threadData->numArray[nextInt] == 1) {

            // Check all multiples of the current number until the end of array and cross off with a 0
            for (int j = nextInt * nextInt; j <= threadData->upperLimit; j += nextInt) {
                threadData->numArray[j] = 0;
            }

            // Prints out all elements crossed off in thread
            for (int j = threadData->lowerLimit; j <= threadData->upperLimit; j++) {

                // True prints out all primes, false prints out all composites
                if (threadData->numArray[j] == 0) {
                    printf("%d\n", j);
                }
            }
        }

        else if (threadData->numArray[nextInt] == 0) {
            printf("Already Checked: %d\n\n", nextInt);
        }
    }

    //pthread_mutex_lock(&mutex);
    nextInt = nextInt + 1;
    childDone = 1;
    //pthread_mutex_unlock(&mutex);
    pthread_exit(0);
}

int waitChild() {
    childDone = 1;
}
