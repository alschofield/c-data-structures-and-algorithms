// Declares the public quicksort API.
#include "quick_sort.h"

// Partitions and recursively sorts one nonempty subrange in place.
bool recurse(void **items, size_t count, QuickSortCompareFn compare) {
    // Treats empty and singleton subranges as already sorted.
    if (count == 0U) {
        return true;
    }

    if (count == 1U) {
        return true;
    }

    // Scans the unclassified region from left to right.
    size_t n = 0U;
    // Saves the pivot value so swaps cannot change the comparison reference.
    void *pivot = items[count / 2];
    // Marks the first equal item after the completed less-than partition.
    size_t less_than_index = 0U;
    // Marks the final unclassified item before the completed greater-than partition.
    size_t greater_than_index = count - 1;
    // Classifies every item into less-than, equal-to, or greater-than partitions.
    while(n <= greater_than_index) {
        // Compares the current candidate to the saved pivot value.
        int comparison = compare(items[n], pivot);
        if (comparison < 0) {
            // Moves the candidate into the less-than partition.
            void *temp = items[n];
            items[n] = items[less_than_index];
            items[less_than_index] = temp;
            // Expands both the less-than and equal-to partitions.
            less_than_index++;
            n++;
        } else if (comparison > 0) {
            // Moves the candidate to the end of the unclassified region.
            void *temp = items[n];
            items[n] = items[greater_than_index];
            items[greater_than_index] = temp;
            // Expands the greater-than partition without advancing the scan.
            greater_than_index--;
        } else {
            // Leaves equal candidates in the middle partition.
            n++;
        }
    }

    // Recursively sorts the completed less-than partition.
    recurse(&items[0U], less_than_index, compare);
    // Recursively sorts the completed greater-than partition.
    recurse(&items[greater_than_index + 1], count - (greater_than_index + 1), compare);

    // Reports successful in-place sorting of this subrange.
    return true;
}

// Sorts caller-owned item pointers in ascending comparison order.
bool quick_sort(void **items, size_t count, QuickSortCompareFn compare) {
    // Treats an empty range as sorted, even without array storage.
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

    // Delegates the full range to the recursive in-place partitioner.
    return recurse(items, count, compare);
}
