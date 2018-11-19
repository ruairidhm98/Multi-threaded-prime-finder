#ifndef _MIN_HEAP_
#define _MIN_HEAP_

typedef struct minheap MinHeap;

/* Returns a pointer to a min heap if successfull, NULL otherwise */
MinHeap *min_heap_create(int);
/* Returns 0/1 of item was inserted/not inserted */
int min_heap_insert(MinHeap *, int);
/* Returns the item deleted in min heap at the root */
int min_heap_delete_root(MinHeap *);
/* Prints the min heap contents */
void print_min_heap(MinHeap *);
/* Destorys a min heap object */
void min_heap_destroy(MinHeap *);


#endif