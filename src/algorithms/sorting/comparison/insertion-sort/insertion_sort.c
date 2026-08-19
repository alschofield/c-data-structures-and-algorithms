// Provides the insertion sort declaration and comparison callback type.
#include "insertion_sort.h"

// Sorts the pointer array in place into ascending comparison order.
bool insertion_sort(void **items, size_t count, InsertionSortCompareFn compare) {
    // Rejects a missing comparison function.
    if (compare == NULL) {
        return false;
    }

    // Accepts an empty input as a successful no-op before validating the array.
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

    // Marks the first raw element; everything before it is the sorted prefix.
    size_t index = 1U;
    while(index != count) {
        // Walks the new element leftward from the end of the sorted prefix.
        for (size_t i = index; i > 0; i--) {
            // Swaps one step left while the sorted element is strictly greater.
            if (compare(items[i - 1], items[i]) > 0) {
                void *temp = items[i - 1];
                items[i - 1] = items[i];
                items[i] = temp;
            } else {
                // Stops at the first non-greater element: the slot is found,
                // and inserting after equals keeps the sort stable. Sorted
                // input breaks immediately, giving the O(n) best case.
                break;
            }
        }

        // Grows the sorted prefix over the newly placed element.
        index++;
    }

    // Reports a completed sort.
    return true;
}
