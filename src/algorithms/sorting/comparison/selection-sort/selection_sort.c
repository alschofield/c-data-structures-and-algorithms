// Provides the selection sort declaration and comparison callback type.
#include "selection_sort.h"

// Sorts the pointer array in place into ascending comparison order.
bool selection_sort(void **items, size_t count, SelectionSortCompareFn compare) {
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

    // Marks the next prefix slot to fill; everything before it is final.
    size_t index = 0U;
    while(index != count - 1) {
        // Starts the minimum champion as the prefix element itself.
        size_t next_index = index;
        // Scans the unsorted remainder backward from the last element.
        size_t n = (count - 1);
        while(n != index) {
            // Keeps the champion only while it is strictly smaller; ties and
            // losses pass the title to the scanned index, so the earliest
            // equal minimum ends up winning by scan order.
            next_index = compare(items[next_index], items[n]) < 0 ? next_index : n;
            // Steps toward the front of the unsorted region.
            n--;
        }

        // Swaps the pass minimum into the prefix slot; self-swap when already placed.
        void *temp = items[index];
        items[index] = items[next_index];
        items[next_index] = temp;

        // Locks the filled slot into the sorted prefix.
        index++;
    }

    // Reports a completed sort.
    return true;
}
