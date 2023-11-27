#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t myCond1, myCond2;
int myCount, oldCount;

void* perform_work(void* arg) {

    printf("CONSUMER THREAD CREATED\n");

    while (1) {
        pthread_mutex_lock(&myMutex); // Lock the mutex
        printf("CONSUMER: myMutex locked\n");

        while (myCount == oldCount) { // counter has yet to be incremented by producer
            printf("CONSUMER: waiting on myCond2\n");
            pthread_cond_wait(&myCond2, &myMutex);
        }
        
        printf("myCount: %d -> %d\n", oldCount, myCount);
        oldCount++; // increment 2nd counter


        pthread_mutex_unlock(&myMutex); // Unlock the mutex
        printf("CONSUMER: myMutex unlocked\n");

        printf("CONSUMER: signaling myCond1\n");
        pthread_cond_signal(&myCond1); // Signal 
        
        // end condition
        if (oldCount == 10) {
            return NULL;
        }
    }
}

int main() {
    myCount = 0;
    oldCount = 0;
    pthread_t consumer;

    printf("PROGRAM START\n");

    int result_code = pthread_create(&consumer, NULL, perform_work, NULL);

    while (1) {
        pthread_mutex_lock(&myMutex); // Lock the mutex
        printf("PRODUCER: myMutex locked\n");

        // wait for consumer to process the previous incrementation (and increment 2nd counter)
        while (myCount != oldCount) {
            printf("PRODUCER: waiting on myCond1\n");
            pthread_cond_wait(&myCond1, &myMutex);
        }

        if (oldCount == 10) {
            printf("PROGRAM END\n");
            result_code = pthread_join(consumer, NULL); // clean up thread
            exit(0);
        }

        myCount++; // increment counter

        pthread_mutex_unlock(&myMutex); // Unlock the mutex
        printf("PRODUCER: myMutex unlocked\n");

        printf("PRODUCER: signaling myCond2\n");
        pthread_cond_signal(&myCond2); // Signal consumer that the count has been incremented
    }
}