// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Names the opaque binary heap.
typedef struct BinaryHeap BinaryHeap;
// Compares two caller-owned items using qsort-style ordering.
typedef int (*BinaryHeapCompareFn)(const void *left, const void *right);

// Allocates an empty heap with its priority comparison callback.
BinaryHeap *binary_heap_create(BinaryHeapCompareFn compare);
// Frees heap storage without freeing caller-owned items.
void binary_heap_destroy(BinaryHeap *heap);
// Inserts one item while preserving the heap property.
bool binary_heap_push(BinaryHeap *heap, void *item);
// Removes and returns the highest-priority item.
bool binary_heap_pop(BinaryHeap *heap, void **out_item);
// Returns the highest-priority item without removing it.
bool binary_heap_peek(const BinaryHeap *heap, void **out_item);
// Returns the number of stored items.
size_t binary_heap_size(const BinaryHeap *heap);
// Reports whether the heap contains no items.
bool binary_heap_is_empty(const BinaryHeap *heap);
