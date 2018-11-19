#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "minheap.h"

/* Returns the minimum between two values */
#define MIN(a, b) ((a < b) ? (a) : (b))
/* Returns the maximum between two values */
#define MAX(a, b) ((a > b) ? (a) : (b))

/* Moves nodes in case of insertion */
static void heapify_up(MinHeap *);
/* Moves nodes in case of deletion */
static void heapify_down(MinHeap *);
/* Helper function */
static int get_left_child_index(int);
/* Helper function */
static int get_right_child_index(int);
/* Helper function */
static int get_parent_index(int);
/* Helper function */
static int has_left_child(MinHeap *, int);
/* Helper function */
static int has_right_child(MinHeap *, int);
/* Helper function */
static int has_parent(int);
/* Helper function */
static int get_parent(MinHeap *, int);
/* Helper function */
static int get_left_child(MinHeap *, int);
/* Helper function */
static int get_right_child(MinHeap *, int);
/* Helper function */
static void swap(int *, int, int);

/* Min heap structure */
struct minheap {
    pthread_mutex_t mutex;
    int *elements;
    int capacity;
    int size;
};

/* Returns a pointer to a min heap object if successfull, NULL otherwise */
MinHeap *min_heap_create(int capacity) {

    MinHeap *mh;

    mh = (MinHeap *) malloc(sizeof(MinHeap));
    /* Print error message and return NULL if memory allocation fails */
    if (!mh) {
        fprintf(stderr, "Error: memory allocation failed\n");
        return NULL;
    }
    mh -> capacity = capacity;
    mh -> size = 0;
    mh -> elements = (int *) malloc(sizeof(int) * capacity);
    if (pthread_mutex_init(&mh -> mutex, NULL)) {
        fprintf(stderr, "Error: failed to create mutex\n");
        free(mh);
        mh = NULL;
        return mh;
    }
    if (!mh -> elements) {
        fprintf(stderr, "Error: memory allocation failed\n");
        free(mh);
        mh = NULL;
    }

    return mh;
}

/* Removes the root node. Returns the root node if successfull, -1 otherwise */
int min_heap_delete_root(MinHeap *heap) {

    int root;
    /* Check if heap if empty first, print error message and return -1 if so */
    if (heap -> size == 0) {
        fputs("Error: heap is empty.\n", stderr);
        return -1;
    }
    root = heap -> elements[0];
    /* Critical region */
    pthread_mutex_lock(&heap -> mutex);
    heap -> elements[0] = heap -> elements[--heap -> size];
    heapify_down(heap);
    pthread_mutex_unlock(&heap -> mutex);

    return root;
}

/* Inserts data into Min Heap */
int min_heap_insert(MinHeap *heap, int data) {

    /* Print error message and return from function if heap is full */
    if (heap -> size == heap -> capacity) {
        fprintf(stderr, "Error: heap is full!\n");
        return 1;
    }
    pthread_mutex_lock(&heap -> mutex);
    /* Insert data */
    heap -> elements[heap -> size++] = data;
    /* Impose heap property */
    heapify_up(heap);
    pthread_mutex_unlock(&heap -> mutex);

    return 0;
}   

/* Prints the min heap */
void print_min_heap(MinHeap *mh) {
    int i;
    for (i = 0; i < mh -> size; i++) printf("%d ", mh -> elements[i]);
    printf("\n");
}

/* Destorys a min heap object */
void min_heap_destroy(MinHeap *mh) {
    free(mh);
    free(mh -> elements);
    pthread_mutex_destroy(&mh->mutex);
    mh = NULL;
}

/* Bubbles elements up */
static void heapify_up(MinHeap *heap) {

    int index;

    index = heap -> size - 1;
    /* Loop until correct position has been found */
    while (has_parent(index) && get_parent(heap, index) > heap -> elements[index]) {
        /* Swaps with parent node if parent is greater than node being bubbled */
        swap(heap -> elements, get_parent_index(index), index);
        index = get_parent_index(index);
    }

}

/* Bubbles elements down */
static void heapify_down(MinHeap *heap) {

    int index, smaller_child_index;

    index = 0;
    /* Loop until node is in correct place */
    while (has_left_child(heap, index)) {

        smaller_child_index = get_left_child_index(index);
        /* The right child is smaller */
        if (has_right_child(heap, index) && get_right_child(heap, index) < get_left_child(heap, index))
            smaller_child_index = get_right_child_index(index);

        /* Correct place has been found */
        if (heap -> elements[index] <= heap -> elements[smaller_child_index])
            break;
        else swap(heap -> elements, index, smaller_child_index);
        
        index = smaller_child_index;
    }

}


/* Helper function, gets index of left child */
static int get_left_child_index(int parent_index) { return 2 * parent_index + 1; }

/* Helper function, gets index of right child */
static int get_right_child_index(int parent_index) { return 2 * parent_index + 2; }

/* Helper function, gets index of parent node */
static int get_parent_index(int child_index) { return (child_index - 1) / 2; }

/* Helper function, checks if parent node has a left child */
static int has_left_child(MinHeap *heap, int index) { return get_left_child_index(index) < heap->size; }

/* Helper function, checks if parent node has a right child */
static int has_right_child(MinHeap *heap, int index) { return get_right_child_index(index) < heap->size; }

/* Helper function, checks if a given node has a parent */
static int has_parent(int index) { return get_parent_index(index) >= 0; }

/* Helper function, gets the left child of a parent node */
static int get_left_child(MinHeap *heap, int index) { return heap -> elements[get_left_child_index(index)]; }

/* Helper function, gets the right child of a parent node */
static int get_right_child(MinHeap *heap, int index) { return heap -> elements[get_right_child_index(index)]; }

/* Helper function, gets the parent of a given node */
static int get_parent(MinHeap *heap, int index) { return heap -> elements[get_parent_index(index)]; }

/* Helper function, swaps two elements */
static void swap(int *elements, int i, int j) {

    int temp;

    temp = elements[i];
    elements[i] = elements[j];
    elements[j] = temp;

}

