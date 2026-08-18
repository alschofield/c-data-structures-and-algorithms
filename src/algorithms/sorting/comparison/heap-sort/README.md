# Heap Sort

Comparison sort that builds a max-heap in the array, then repeatedly swaps the
root to the tail and re-heapifies the shrinking prefix.

## Required API

```c
typedef int (*HeapSortCompareFn)(const void *left, const void *right);

bool heap_sort(void **items, size_t count, HeapSortCompareFn compare);
```

The checked-in source is still the scaffold gate `bool
heap_sort_is_implemented(void)`, which returns `false`; the test asserts exactly that.

## Contract

- Sorts in place into ascending order under the caller's comparison.
- Not stable; sift operations reorder equal elements. Do not claim stability.
- Heap construction must use bottom-up heapify (sift-down from the last
  parent), which is O(n), not n repeated insertions.
- Uses implicit array indexing: children of `i` at `2i + 1` and `2i + 2`;
  no node allocation.
- Guaranteed O(n log n) regardless of input order.
- Zero-length and single-element inputs are no-ops; a `NULL` array with
  nonzero length is invalid input handled without undefined behavior.

## Complexity Targets

- Best: O(n log n)
- Average: O(n log n)
- Worst: O(n log n)
- Space: O(1), in place and iterative
