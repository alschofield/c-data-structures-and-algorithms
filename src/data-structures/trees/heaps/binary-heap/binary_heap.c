#include "binary_heap.h"
// Declares malloc and free.
#include <stdlib.h>
#include <stdint.h>

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

// Appends an item then sifts it upward into heap order.
bool binary_heap_push(BinaryHeap *heap, void *item) {
    // Rejects a missing heap.
    if (heap == NULL) {
        return false;
    }

    // Grows only when every currently allocated tree slot is occupied.
    if (heap->capacity == heap->size) {
        // Rejects a capacity that cannot add one complete tree level safely.
        if (heap->capacity > (SIZE_MAX - 1) / 2U) {
            return false;
        }

        // Expands from 3 to 7 to 15 and so on, reserving one full new level.
        size_t new_capacity = heap->capacity * 2U + 1U;
        // Rejects an item-slot count whose byte allocation would overflow.
        if (new_capacity > SIZE_MAX / sizeof(*heap->items)) {
            return false;
        }

        // Resizes through a temporary pointer so failure preserves the old array.
        void **items = realloc(heap->items, sizeof(*heap->items) * new_capacity);
        // Reports resize failure without changing heap fields.
        if (items == NULL) {
            return false;
        }
        // Publishes the successfully resized item array and capacity.
        heap->items = items;
        heap->capacity = new_capacity;
    }

    // Places the new item in the next open complete-tree slot.
    heap->items[heap->size] = item;
    // Includes that slot in the logical heap before sifting it upward.
    heap->size++;

    // Holds one pointer while it swaps with its parent.
    void *temp = NULL;
    // Starts at the new item's index after the size increment.
    size_t n = heap->size - 1;
    // Repeats until the item reaches the root or its parent already orders first.
    while(n > 0U) {
        // Computes the array index of this item's parent.
        size_t parent_index = (n - 1U) / 2U;
        // Swaps upward when the child should order before its parent.
        if (heap->compare(heap->items[parent_index], heap->items[n]) > 0) {
            temp = heap->items[n];
            heap->items[n] = heap->items[parent_index];
            heap->items[parent_index] = temp;
            // Continues checking from the item's new parent position.
            n = parent_index;
        } else {
            // Stops once the parent-child heap order holds.
            return true;
        }
    }

    // Reports success when the item reaches the root.
    return true;
}

// TODO: Return the root, move the final item to index zero, then sift down.
bool binary_heap_pop(BinaryHeap *heap, void **out_item);

// Returns the root item without changing size or heap order.
bool binary_heap_peek(const BinaryHeap *heap, void **out_item) {
    // Rejects a missing heap.
    if (heap == NULL) {
        return false;
    }

    // Rejects a missing output location.
    if (out_item == NULL) {
        return false;
    }

    // Rejects an empty heap before reading index zero.
    if (heap->size == 0U) {
        return false;
    }

    // Copies the root item pointer without removing it from the heap.
    *out_item = heap->items[0];

    // Reports successful root lookup.
    return true;
}

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
