// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Names the opaque FIFO queue.
typedef struct Queue Queue;

// Allocates an empty queue.
Queue *queue_create(void);
// Frees queue storage without freeing caller-owned items.
void queue_destroy(Queue *queue);
// Appends one item to the queue tail.
bool queue_enqueue(Queue *queue, void *item);
// Removes and returns the queue head item.
bool queue_dequeue(Queue *queue, void **out_item);
// Returns the queue head item without removing it.
bool queue_peek(const Queue *queue, void **out_item);
// Returns the number of queued items.
size_t queue_size(const Queue *queue);
// Reports whether the queue contains no items.
bool queue_is_empty(const Queue *queue);
