// Declares the public heap-sort API.
#include "heap_sort.h"

// Sorts caller-owned item pointers in ascending comparison order.
bool heap_sort(void **items, size_t count, HeapSortCompareFn compare) {
    // Treats an empty range as already sorted, even without array storage.
    if (count == 0U) {
        return true;
    }

    // Rejects a missing nonempty item array.
    if (items == NULL) {
        return false;
    }

    // Rejects a missing ordering function.
    if (compare == NULL) {
        return false;
    }

    // Treats one item as already sorted.
    if (count == 1U) {
        return true;
    }

    // Heapifies bottom-up from the last parent through the root.
    int n = (count / 2 - 1);
    while (n >= 0) {
        // Holds the current parent while it moves down its subtree.
        size_t parent_index = n;
        // Continues only while the current parent has a valid left child.
        while (2U * parent_index + 1U < count) {
            // Calculates the current parent's left and right child indexes.
            size_t left_index = 2U * parent_index + 1U;
            size_t right_index = 2U * parent_index + 2U;

            // Defaults to the left child when no right child exists.
            size_t child_index = left_index;
            // Chooses the left child when it is the only active child.
            if (left_index <= count && right_index >= count) {
                child_index = left_index;
            } else {
                // Chooses the larger child to preserve the max-heap rule.
                child_index = compare(items[left_index], items[right_index]) > 0 ? left_index : right_index;
            }

            // Stops when the parent already dominates its larger child.
            if (compare(items[parent_index], items[child_index]) < 0) {
                // Promotes the larger child and moves the smaller parent down.
                void *temp = items[child_index];
                items[child_index] = items[parent_index];
                items[parent_index] = temp;
                // Continues inside the child subtree that received the parent.
                parent_index = child_index;
            } else {
                break;
            }
        }

        // Moves upward through the implicit tree toward the root index.
        n--;
    }

    // Repeatedly moves the current maximum from the root into the sorted suffix.
    size_t heap_size = count;
    while(heap_size >= 1U) {
        // Exchanges the root maximum with the final active heap slot.
        void *candidate = items[heap_size - 1];
        items[heap_size - 1] = items[0U];
        items[0U] = candidate;
        // Excludes the final maximum from further heap operations.
        heap_size--;

        // Sifts the new root down inside the remaining active heap prefix.
        size_t n = 0U;
        while(n < heap_size) {
            // Calculates the new root position's child indexes.
            size_t left_index = 2U * n + 1U;
            size_t right_index = 2U * n + 2U;

            // Stops when the active heap has no left child below this parent.
            if (left_index >= heap_size) {
                break;
            }

            // Defaults to the left child when no right child remains active.
            size_t child_index = left_index;
            // Selects the sole left child or the larger of two active children.
            if (left_index < heap_size && right_index >= heap_size) {
                child_index = left_index;
            } else {
                child_index = compare(items[left_index], items[right_index]) > 0 ? left_index : right_index;
            }

            // Stops when the moved root already dominates its larger child.
            if (compare(candidate, items[child_index]) >= 0) {
                break;
            } else {
                // Promotes the larger child and continues with the moved candidate.
                items[n] = items[child_index];
                items[child_index] = candidate;
                n = child_index;
            }
        }
    }

    // Reports successful in-place ascending sort.
    return true;
}
