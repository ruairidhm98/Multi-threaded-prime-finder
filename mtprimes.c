/* Multi-threaded program which finds a certain number of prime numbers concurrently */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include "isprime.h"
#include "minheap.h"
#include "bounded_buffer.h"

static int primesFound = 0;

/* Parameters passed to search_region function */
typedef struct args_1 {
    int end;
    int start;
    int block_gap;
    BoundedBuffer *bb;
    unsigned long limit;
} Args;

/* Parameters passed into print_primes */
typedef struct args_2 {
    MinHeap *mh;
    BoundedBuffer *bb;
    unsigned long limit;
} Args_t;

/* Finds all primes in a particular region */
void *search_region(void *arg) {

    unsigned long i, n;
    Args *args;
    
    args = (Args *) arg;
    n = 0;
    /* Search for primes in given range */
    for (i = args -> start; primesFound < args -> limit; i++) {
        if (n++ == args -> end) {
            n = 0;
            i += args -> block_gap * (args -> end - args -> start);
        }
        if (is_prime(i)) {
            if (bb_get_size(args -> bb) > args -> limit) set_done(args -> bb);
            else {
                bb_insert(args -> bb, i);
                primesFound++;
            }
        }
    }
                     
    pthread_exit((void *) 0);
}

/* Prints all elements in bounded buffer */
void *print_primes(void *arg) { 

    Args_t *args;
    int count;

    args = (Args_t *) arg;
    count = 0;
    while (count++ < args -> limit) 
        min_heap_insert(args -> mh, bb_remove(args -> bb));
    
    /* Notify other threads we are done */
    set_done(args -> bb);
    
    pthread_exit((void *) 0);
 }

void print_sorted(MinHeap *mh, int limit) {
    int i;
    for (i = 0; i < limit; i++) printf("%d\n", min_heap_delete_root(mh));
}

int main(int argc, char **argv) {

    pthread_t collectorThread, *generatorThreads;
    unsigned long block, limit, temp, msec;
    double msperprime, t1, t2;
    int i, j, nthread;
    BoundedBuffer *bb;
    Args_t *args1;
    MinHeap *mh;
    Args *args;
    
    block = 1;
    limit = 100;
    nthread = 1;
    
    /* Process command line arguments */
    for (i = 1; i < argc;) {
        if ((j = i + 1) == argc) {
            fprintf(stderr, "usage: ./mtprimes [-b block] [-l limit] [-t nthread]\n");
            return -1;
        }
        if (strcmp(argv[i], "-b") == 0)
            sscanf(argv[j], "%lu", &block);
        else if (strcmp(argv[i], "-l") == 0)
            sscanf(argv[j], "%lu", &limit);
        else if (strcmp(argv[i], "-t") == 0)
            sscanf(argv[j], "%d", &nthread);
        else {
            fprintf(stderr, "Unknown flag: %s %s\n", argv[i], argv[j]);
            return -1;
        }
        i = j + 1;
    }

    /* Create bounded buffer object */
    bb = bb_create(limit);
    if (!bb) return -1;

    /* Create min heap object */
    mh = min_heap_create(limit);
    if (!mh) {
        bb_destroy(bb);
        return -1;
    } 

    /* Create generator threads */
    generatorThreads = (pthread_t *) malloc(sizeof(pthread_t) * nthread);
    if (!generatorThreads) {
        min_heap_destroy(mh);
        bb_destroy(bb);
        free((void *) generatorThreads);
        return -1;
    }
    /* Create argument array */
    args = (Args *) malloc(sizeof(Args) * nthread);
    if (!args) {
        free((void *) args);
        return -1;
    }
    temp = 0;
    for (i = 0; i < nthread; i++) {
        args[i].bb = bb;
        args[i].start = temp;
        temp += block;
        args[i].end = temp;
        args[i].limit = limit;
        args[i].block_gap = nthread;
    }
    
    /* Create arguments for print_primes */
    args1 = (Args_t *) malloc(sizeof(Args_t));
    if (!args1) {
        min_heap_destroy(mh);
        bb_destroy(bb);
        free((void *) generatorThreads);
        free((void *) args);
        free((void *) args1);
        return -1;
    }
    args1 -> bb = bb;
    args1 -> limit = limit;
    args1 -> mh = mh;

    t1 = clock();
    /* Check if collector thread was spawned successfully */
    if (pthread_create(&collectorThread, NULL, print_primes, (void *) args1)) {
        fprintf(stderr, "Error: collector thread failed to create\n");
        min_heap_destroy(mh);
        bb_destroy(bb);
        free((void *) generatorThreads);
        free((void *) args);
        free((void *) args1);
        return -1;
    }

    /* Start all generator threads */
    for (i = 0; i < nthread; i++) 
        /* Check if each generator thread spawned succesfully */
        if (pthread_create(&generatorThreads[i], NULL, search_region, (void *) &args[i])) {
            fprintf(stderr, "Error: generator thread %d failed to create\n", i);
            min_heap_destroy(mh);
            bb_destroy(bb);
            free((void *) generatorThreads);
            free((void *) args);
            free((void *) args1);
            return -1;
        }
        
    /* Wait for generators to finish */
    for (i = 0; i < nthread; i++) pthread_join(generatorThreads[i], NULL);

    /* Wait for collector thread to finish */
    pthread_join(collectorThread, NULL); 
    t2 = clock();
    /* Print primes in sorted order */
    print_sorted(mh, limit);
    fprintf(stderr, "%lu primes computed in %.4f seconds, %.4f ms/prime\n", 
            limit, (t2-t1)/CLOCKS_PER_SEC/nthread, 1000 * ((t2-t1)/CLOCKS_PER_SEC)/limit/nthread);
    /* Free heap memory */
    free((void *) args);
    free((void *) args1);
    bb_destroy(bb);
    min_heap_destroy(mh);
    
    return 0;
}
