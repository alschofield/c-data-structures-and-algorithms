// Provides the bubble sort declaration and comparison callback type.
#include "bubble_sort.h"

// Sorts the pointer array in place into ascending comparison order.
bool bubble_sort(void **items, size_t count, BubbleSortCompareFn compare) {
    // Rejects a missing comparison function.
    if (compare == NULL) {
        return false;
    }

    // Accepts an empty input as a successful no-op before validating pointers.
    if (count == 0U) {
        return true;
    }

    // Rejects a missing array when there are elements to sort.
    if (items == NULL) {
        return false;
    }

    // Accepts a single element as already sorted.
    if (count == 1U) {
        return true;
    }

    // it needs to do this over and over until there are no swaps
    // Counts the swaps made in the current pass.
    size_t num_of_swaps = 0U;
    // Marks where the unsorted region ends; each pass locks one element past it.
    size_t boundary = (count - 1);
    do {
        // Resets the counter so this pass is judged on its own swaps.
        num_of_swaps = 0U;
        // Walks the unsorted region from the front.
        size_t n = 0U;
        while(n != boundary) {
            // Compares the adjacent pair once per step.
            int swap = compare(items[n], items[n + 1]);
            // Swaps only when strictly out of order, keeping equal elements stable.
            if (swap > 0) {
                // Holds both pointers while they trade places.
                void *left = items[n];
                void *right = items[n + 1];
                items[n] = right;
                items[n + 1] = left;
                // Records that this pass changed the array.
                num_of_swaps++;
            }

            // Advances to the next adjacent pair.
            n++;
        }

        // Shrinks the region because the pass carried its maximum to the end.
        boundary--;
    // Runs another pass only if this one swapped; a clean pass means sorted.
    } while(num_of_swaps > 0U);

    // Reports a completed sort.
    return true;
}
