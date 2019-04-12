#ifndef _BOUNDED_BUFFER_
#define _BOUNDED_BUFFER_

#include <pthread.h>

typedef struct boundedbuffer BoundedBuffer;

/* Returns a pointer to a bounded buffer object */
BoundedBuffer *bb_create(uint32_t);
/* Insert a given unsigned integer into the buffer */
int bb_insert(BoundedBuffer *, uint32_t);
/* Removes an element from the buffer */
int bb_remove(BoundedBuffer *);
/* Returns the size of the buffer */
int bb_get_size(BoundedBuffer *);
/* Destorys bounded buffer */
void bb_destroy(BoundedBuffer *);
/* Prints the contents of the buffer */
void bb_print(BoundedBuffer *);
/* Set done to true */
void set_done(BoundedBuffer *);

#endif