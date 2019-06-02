#include "bounded_buffer.h"
#include "isprime.h"
#include <stdio.h>
#include <stdlib.h>

/* Bounded buffer implemented as a circular array */
struct boundedbuffer {
  uint32_t done;
  uint32_t start;
  uint32_t end;
  uint32_t size;
  uint32_t capacity;
  uint32_t *elements;
  pthread_mutex_t mutex;
  pthread_cond_t insert;
  pthread_cond_t delete;
};

/* Returns a pointer to a bounded buffer if successfulll, NULL otherwise */
BoundedBuffer *bb_create(uint32_t capacity) {

  BoundedBuffer *bb;

  bb = (BoundedBuffer *)malloc(sizeof(BoundedBuffer));
  /* Print error message and return NULL if memory allocation fails */
  if (!bb) {
    fprintf(stderr, "Error: memory allocation failed. bb_create failed\n");
    return NULL;
  }
  bb->done = 0;
  bb->start = 0;
  bb->size = 0;
  bb->end = capacity - 1;
  bb->capacity = capacity;
  bb->elements = (uint32_t *)malloc(sizeof(uint32_t) * capacity);
  if (!bb->elements) {
    free((void *)bb);
    bb = NULL;
    fprintf(stderr, "Error: memory allocation failed. bb_create failed\n");
    return bb;
  }
  /* Print error message and return NULL if mutex fails to create */
  if (pthread_mutex_init(&bb->mutex, NULL)) {
    fprintf(stderr, "Error: mutex failed to create. bb_create failed\n");
    free((void *)bb);
    free((void *)bb->elements);
    bb = NULL;
    return bb;
  }
  /* Print error message and return NULL if condition variable fails to create
   */
  if (pthread_cond_init(&bb->insert, NULL)) {
    fprintf(stderr,
            "Error: condition variable failed to create. bb_create failed\n");
    free((void *)bb);
    free((void *)bb->elements);
    pthread_mutex_destroy(&bb->mutex);
    bb = NULL;
    return bb;
  }
  if (pthread_cond_init(&bb->delete, NULL)) {
    fprintf(stderr,
            "Error: condition variable failed to create. bb_create failed\n");
    free((void *)bb);
    free((void *)bb->elements);
    pthread_mutex_destroy(&bb->mutex);
    pthread_cond_destroy(&bb->insert);
    bb = NULL;
  }

  return bb;
}

/* Inserts into bounded buffer, returns 0/1 for success/failure */
int bb_insert(BoundedBuffer *bb, uint32_t num) {

  if (!bb) {
    fprintf(stderr, "Error: bounded buffer is NULL. bb_insert failed\n");
    return 1;
  }
  /* Critical region */
  pthread_mutex_lock(&bb->mutex);
  /* Bounded buffer is full, or all elements currently being printed */
  while ((bb->size == bb->capacity) && (!bb->done)) {
    printf("== Buffer is full ==\n");
    pthread_cond_wait(&bb->insert, &bb->mutex);
  }
  /* We are not done yet */
  if (bb->size != bb->capacity) {
    bb->elements[bb->start] = num;
    bb->start = (bb->start + 1) % bb->capacity;
    bb->size++;
  }
  pthread_mutex_unlock(&bb->mutex);
  pthread_cond_signal(&bb->delete);

  return 0;
}

/* Removes and returns the last element stored in the buffer */
int bb_remove(BoundedBuffer *bb) {

  int item = -1;
  if (!bb)
    return -1;

  pthread_mutex_lock(&bb->mutex);
  /* Buffer is empty */
  while (!bb->size && !bb->done) {
    printf("== Buffer is empty ==\n");
    pthread_cond_wait(&bb->delete, &bb->mutex);
  }
  if (!bb->done && bb->size) {
    bb->end = (bb->end + 1) % bb->capacity;
    item = bb->elements[bb->end];
    bb->size--;
  }
  pthread_mutex_unlock(&bb->mutex);
  pthread_cond_signal(&bb->insert);

  return item;
}

/* Print the contents of the buffer */
void bb_print(BoundedBuffer *bb) {
  int i;
  for (i = 0; i < bb->size; i++)
    printf("%d\n", bb->elements[i]);
}

/* Returns the size of the bounded buffer */
int bb_get_size(BoundedBuffer *bb) {

  int result;

  pthread_mutex_lock(&bb->mutex);
  result = bb ? bb->size : -1;
  pthread_mutex_unlock(&bb->mutex);
  return result;
}

/* Destroys bounded buffer */
void bb_destroy(BoundedBuffer *bb) {
  free((void *)bb);
  free((void *)bb->elements);
  pthread_mutex_destroy(&bb->mutex);
  pthread_cond_destroy(&bb->insert);
  pthread_cond_destroy(&bb->delete);
}

/* Set done to true, used to help thread synch */
void set_done(BoundedBuffer *bb) {
  pthread_mutex_lock(&bb->mutex);
  bb->done = 1;
  pthread_mutex_unlock(&bb->mutex);
  pthread_cond_broadcast(&bb->insert);
  pthread_cond_broadcast(&bb->delete);
}