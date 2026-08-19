// Provides the merge sort declaration and comparison callback type.
#include "merge_sort.h"

// Sorts one window of the array by recursive halving and merging.
static void traverse(void **items, void **temp, size_t front, size_t back, MergeSortCompareFn compare) {
    // Accepts an empty window as already sorted.
    if (back - front == 0U) {
        return;
    }

    // Accepts a single-element window as already sorted.
    if (back - front == 1U) {
        return;
    }

    // Splits the window at its midpoint without overflowing.
    size_t mid = front + (back - front) / 2;
    // Sorts the left half of the window in place.
    traverse(items, temp, front, mid, compare);
    // Sorts the right half of the window in place.
    traverse(items, temp, mid, back, compare);

    // Tracks the front of the unmerged left half.
    size_t left_index = front;
    // Tracks the front of the unmerged right half.
    size_t right_index = mid;
    // Tracks the next open slot in the merge output.
    size_t temp_index = 0U;
    // Zips both halves into temp until every element is taken.
    while(left_index < mid || right_index < back) {
        // Fakes a decisive result when either half is exhausted: an empty
        // left forces a right take (1), an empty right forces a left take
        // (0); only when both halves are live does the comparison run.
        int comparison = left_index == mid ? 1 : (right_index == back ? 0 : compare(items[left_index], items[right_index]));
        // Takes from the left on smaller-or-equal, keeping equal elements in
        // their original order (stable).
        if (comparison <= 0) {
            temp[temp_index] = items[left_index];
            left_index++;
            temp_index++;
        } else if (comparison > 0) {
            temp[temp_index] = items[right_index];
            right_index++;
            temp_index++;
        }
    }

    // temp should be filled with the sorted items
    // Copies the merged result back over this window; temp is zero-based, so
    // the window offset shifts each slot into place.
    for (size_t i = 0U; i < (back - front); i++) {
        items[front + i] = temp[i];
    }

    return;
}

// Sorts the pointer array into ascending comparison order.
bool merge_sort(void **items, size_t count, MergeSortCompareFn compare) {
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

    // Allocates the shared merge buffer once; every recursive merge reuses it.
    void **temp = malloc(count * sizeof(void *));
    // Reports allocation failure before any element has moved.
    if (temp == NULL) {
        return false;
    }

    // Sorts the full range in place.
    traverse(items, temp, 0U, count, compare);
    // Releases the merge buffer after the sort completes.
    free(temp);

    // Reports a completed sort.
    return true;
}
