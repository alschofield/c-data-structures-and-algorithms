#include <stdbool.h>
#include <stddef.h>

typedef struct Queue Queue;

Queue *queue_create(void);
void queue_destroy(Queue *queue);
bool queue_enqueue(Queue *queue, void *item);
bool queue_dequeue(Queue *queue, void **out_item);
bool queue_peek(const Queue *queue, void **out_item);
size_t queue_size(const Queue *queue);
bool queue_is_empty(const Queue *queue);
