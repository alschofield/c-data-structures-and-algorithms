#include "binary_heap.h"
// Declares malloc and free.
#include <stdlib.h>

// Defines the fields hidden from callers of the public API.
struct BinaryHeap {
    // Counts occupied array slots in the complete heap tree.
    size_t size;
    // Counts allocated array slots, always a full-tree capacity.
    size_t capacity;
    // Orders two caller-owned item pointers.
    BinaryHeapCompareFn compare;
    // Points to the array whose indexes encode the heap tree.
    void **items;
};

// Allocates an empty heap with one root and two child slots reserved.
BinaryHeap *binary_heap_create(BinaryHeapCompareFn compare) {
    // Rejects a missing ordering rule.
    if (compare == NULL) {
        return NULL;
    }
    
    // Allocates the heap wrapper.
    BinaryHeap *heap = malloc(sizeof(BinaryHeap));
    // Reports wrapper allocation failure.
    if (heap == NULL) {
        return NULL;
    }

    // Initializes the complete-tree capacity and caller comparison function.
    heap->size = 0U;
    heap->capacity = 3U;
    heap->compare = compare;
    // Allocates room for the root and its two children.
    heap->items = malloc(sizeof(*heap->items) * 3U);

    // Releases the wrapper when item-array allocation fails.
    if (heap->items == NULL) {
        free(heap);
        return NULL;
    }

    return heap;
}

// Frees the heap-owned item array and wrapper, never caller-owned items.
void binary_heap_destroy(BinaryHeap *heap) {
    // Treats a missing heap as a no-op.
    if (heap == NULL) {
        return;
    }

    // Releases the array that encoded the complete heap tree.
    free(heap->items);
    free(heap);

    return;
}

// TODO: Grow capacity, append item, then sift it upward into heap order.
bool binary_heap_push(BinaryHeap *heap, void *item);

// TODO: Return the root, move the final item to index zero, then sift down.
bool binary_heap_pop(BinaryHeap *heap, void **out_item);

// TODO: Return items[0] without changing size or heap order.
bool binary_heap_peek(const BinaryHeap *heap, void **out_item);

// Reports how many item pointers currently occupy the heap tree.
size_t binary_heap_size(const BinaryHeap *heap) {
    // Treats a missing heap as holding zero items.
    if (heap == NULL) {
        return 0U;
    }

    // Returns the tracked number of occupied array slots.
    return heap->size;
}

// Reports whether the heap has no occupied array slots.
bool binary_heap_is_empty(const BinaryHeap *heap) {
    // Treats a missing heap as empty, matching the other containers.
    if (heap == NULL) {
        return true;
    }

    // Compares the tracked item count against zero.
    return heap->size == 0U;
}
