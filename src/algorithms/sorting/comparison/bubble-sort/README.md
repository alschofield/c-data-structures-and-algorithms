# Bubble Sort

Comparison sort that repeatedly sweeps the array, swapping adjacent
out-of-order pairs until a full pass makes no swaps.

## Required API

```c
typedef int (*BubbleSortCompareFn)(const void *left, const void *right);

bool bubble_sort(void **items, size_t count, BubbleSortCompareFn compare);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.

## Contract

- Sorts in place into ascending order under the caller's comparison.
- Stable: equal elements keep their original relative order (adjacent swaps
  only, and never swap on equality).
- Must implement the early-exit optimization: a pass with zero swaps
  terminates the sort.
- Zero-length and single-element inputs are no-ops; a `NULL` array with
  nonzero length is invalid input handled without undefined behavior.
- After each pass `k`, the largest `k` elements occupy their final positions
  at the tail.

## Complexity Targets

- Best: O(n) (already sorted input, early exit after one pass)
- Average: O(n^2)
- Worst: O(n^2) (reverse-sorted input)
- Space: O(1), in place
