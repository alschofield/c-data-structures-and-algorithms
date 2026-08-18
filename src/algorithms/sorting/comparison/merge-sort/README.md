# Merge Sort

Divide-and-conquer comparison sort that recursively sorts halves and merges
them with an auxiliary buffer.

## Required API

```c
typedef int (*MergeSortCompareFn)(const void *left, const void *right);

bool merge_sort(void **items, size_t count, MergeSortCompareFn compare);
```

The checked-in source is still the scaffold gate `bool
merge_sort_is_implemented(void)`, which returns `false`; the test asserts exactly that.

## Contract

- Sorts into ascending order under the caller's comparison.
- Stable: on ties the merge step must take from the left run first.
- Guaranteed O(n log n) regardless of input order; no adversarial input
  degrades it.
- Requires an O(n) auxiliary buffer; allocation failure must be reported
  cleanly and leave the input unmodified.
- Zero-length and single-element inputs are no-ops; a `NULL` array with
  nonzero length is invalid input handled without undefined behavior.
- The merge must handle uneven halves and runs that exhaust at different
  times without reading out of bounds.

## Complexity Targets

- Best: O(n log n)
- Average: O(n log n)
- Worst: O(n log n)
- Space: O(n) auxiliary buffer (plus O(log n) recursion depth)
