#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>

#include "defs.h"



// Just used to send the index of the id
struct tdata {
    int tid;
};

int counter = 0;
int padding[64]; /* To avoid false sharing with counter */
int mutex = 0;

inline void lock() {
    while(__atomic_exchange_n(&mutex, 1, __ATOMIC_ACQUIRE|__ATOMIC_HLE_ACQUIRE)) {
        PAUSE;
    }
}

inline void unlock() {
     __atomic_store_n(&mutex, 0, __ATOMIC_RELEASE|__ATOMIC_HLE_RELEASE);
}

inline void operation(int i) {
    int c;

    if (i % 10) {
        // 90% of probability of being a reader
        c = counter; // Just copy the value
    } else {
        counter += 1;
    }
}

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;

    for (i=0; i < max; i++) {
        if (! mutex && _xbegin() == _XBEGIN_STARTED) {
            if (mutex) {
                _xabort(1); /* Lock busy */
            }
            operation(i);
            _xend();
        } else {
            lock();
            operation(i);
            unlock();
        }
    }
    printf("End %d counter: %d\n", tid, counter);
}

int main (int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc, i;
    struct tdata id[NUM_THREADS];

    for(i=0; i<NUM_THREADS; i++){
        id[i].tid = i;
        rc = pthread_create(&threads[i], NULL, count, (void *) &id[i]);
    }

    for(i=0; i<NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT/10);
    return 0;
}