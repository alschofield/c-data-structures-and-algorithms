# Queue

An opaque FIFO queue over a circular, doubling pointer buffer. Enqueue appends at the tail; dequeue removes from the head.

## C API
```c
Queue *queue_create(void); void queue_destroy(Queue *queue);
bool queue_enqueue(Queue *queue, void *item);
bool queue_dequeue(Queue *queue, void **out_item);
bool queue_peek(const Queue *queue, void **out_item);
size_t queue_size(const Queue *queue); bool queue_is_empty(const Queue *queue);
```

## Contract
- Dequeue/peek fail for a null or empty queue and for null output storage; failures preserve state and output.
- Payload pointers are borrowed, including null. `destroy` is null-safe and never frees payloads.
- On growth, live entries are copied into FIFO order before the old buffer is freed. Capacity/byte-size overflow and allocation failures leave the queue unchanged. Null size is zero and null is empty.

## Complexity and Verification
Enqueue is amortized O(1); dequeue, peek, size, and empty are O(1). Verify with `make test NAME=data-structures/linear/queues/queue`.

Generic FIFO collection that stores caller-owned `void *` values, including
`NULL`.

## How It Works

First in, first out — a checkout line. Enqueue joins the back, dequeue
leaves from the front. The array-backed version keeps head and tail indexes
that wrap around the buffer (a ring), so neither operation ever shifts
elements — both are O(1). The FIFO discipline is BFS's frontier and every
producer-consumer handoff.

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

- `enqueue`: O(1) amortized
- `dequeue`, `peek`, `size`, `is_empty`: O(1)
- Space: O(n) contiguous
