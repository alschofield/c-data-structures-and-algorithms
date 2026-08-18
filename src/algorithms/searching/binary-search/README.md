# Binary Search

Divide-and-conquer search over a sorted array that halves the candidate range
on every comparison.

## Required API

```c
typedef int (*BinarySearchCompareFn)(const void *left, const void *right);

bool binary_search(void *const *items, size_t count, const void *key,
                   BinarySearchCompareFn compare, size_t *out_index);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.

## Contract

- Input must already be sorted ascending under the caller's comparison; the
  function may assume but never verify or re-sort it.
- Midpoint arithmetic must not overflow (`low + (high - low) / 2`, not
  `(low + high) / 2`).
- A missing target is reported distinctly; when duplicates exist, any matching
  index is acceptable unless a lower-bound variant is specified.
- Empty input or a `NULL` array pointer is a clean not-found result.
- The input array is never modified.

## Complexity Targets

- Best: O(1) (target at first midpoint)
- Average: O(log n)
- Worst: O(log n)
- Space: O(1) iterative
