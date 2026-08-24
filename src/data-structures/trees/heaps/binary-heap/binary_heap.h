#include <stdbool.h>
#include <stddef.h>

typedef struct BinaryHeap BinaryHeap;
typedef int (*BinaryHeapCompareFn)(const void *left, const void *right);

BinaryHeap *binary_heap_create(BinaryHeapCompareFn compare);
void binary_heap_destroy(BinaryHeap *heap);
bool binary_heap_push(BinaryHeap *heap, void *item);
bool binary_heap_pop(BinaryHeap *heap, void **out_item);
bool binary_heap_peek(const BinaryHeap *heap, void **out_item);
size_t binary_heap_size(const BinaryHeap *heap);
bool binary_heap_is_empty(const BinaryHeap *heap);
