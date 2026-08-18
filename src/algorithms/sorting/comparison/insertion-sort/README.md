# Insertion Sort

Comparison sort that grows a sorted prefix by shifting each new element left
until it reaches its correct position.

## Required API

```c
typedef int (*InsertionSortCompareFn)(const void *left, const void *right);

bool insertion_sort(void **items, size_t count, InsertionSortCompareFn compare);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.

## Contract

- Sorts in place into ascending order under the caller's comparison.
- Stable: shift while strictly greater, insert after equal elements, so equal
  elements keep their original relative order.
- Adaptive: nearly sorted input costs close to O(n); each element moves only
  as far as its displacement.
- Zero-length and single-element inputs are no-ops; a `NULL` array with
  nonzero length is invalid input handled without undefined behavior.
- Invariant: before processing index `i`, the range `[0, i)` is sorted.

## Complexity Targets

- Best: O(n) (already sorted input, one comparison per element)
- Average: O(n^2)
- Worst: O(n^2) (reverse-sorted input)
- Space: O(1), in place
