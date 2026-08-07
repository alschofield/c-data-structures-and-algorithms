# Queue

Generic FIFO collection that stores caller-owned `void *` values, including
`NULL`.

## Required API

```c
typedef struct Queue Queue;

Queue *queue_create(void);
void queue_destroy(Queue *queue);
bool queue_enqueue(Queue *queue, void *item);
bool queue_dequeue(Queue *queue, void **out_item);
bool queue_peek(const Queue *queue, void **out_item);
size_t queue_size(const Queue *queue);
bool queue_is_empty(const Queue *queue);
```

## Contract

- `enqueue` adds an item at the back; `dequeue` and `peek` return the oldest
  item.
- `dequeue` removes its item; `peek` does not.
- Failed dequeues/peeks leave their output parameter unchanged.
- Allocation failure during `enqueue` leaves the queue unchanged.
- `destroy` frees only queue-owned memory.

## Complexity Targets

- `enqueue`, `dequeue`, `peek`, `size`, `is_empty`: O(1)
