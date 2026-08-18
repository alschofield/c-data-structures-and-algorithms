// Scaffold stub: replace the bodies with the real implementation.
#include "binary_heap.h"

BinaryHeap *binary_heap_create(BinaryHeapCompareFn compare) {
    (void)compare;
    return NULL;
}

void binary_heap_destroy(BinaryHeap *heap) {
    (void)heap;
}

bool binary_heap_push(BinaryHeap *heap, void *item) {
    (void)heap;
    (void)item;
    return false;
}

bool binary_heap_pop(BinaryHeap *heap, void **out_item) {
    (void)heap;
    (void)out_item;
    return false;
}

bool binary_heap_peek(const BinaryHeap *heap, void **out_item) {
    (void)heap;
    (void)out_item;
    return false;
}

size_t binary_heap_size(const BinaryHeap *heap) {
    (void)heap;
    return 0U;
}

bool binary_heap_is_empty(const BinaryHeap *heap) {
    (void)heap;
    return true;
}
