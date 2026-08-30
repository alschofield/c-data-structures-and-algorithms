# Binary Search

Divide-and-conquer search over a sorted array that halves the candidate range
on every comparison.

## How It Works

Guided elimination over sorted input. Check the middle element: too small,
and the target can only be right of it; too large, only left. Either way half
the candidates disappear, so the search finishes in O(log n) comparisons. The
invariant that keeps the implementation honest: if the target exists, it is
always inside the current [low, high] window — every step must shrink the
window or exit. The famous defects are boundary bugs: midpoint overflow
(hence `low + (high - low) / 2`), off-by-one window updates, and loops that
stop shrinking.

## Required API

```c
typedef int (*BinarySearchCompareFn)(const void *left, const void *right);

bool binary_search(void *const *items, size_t count, const void *key,
                   BinarySearchCompareFn compare, size_t *out_index);
```

The checked-in implementation recursively narrows a half-open `[low, high)`
window using overflow-safe midpoint arithmetic.

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
- Space: O(log n) recursive call stack

## Verification

```text
make test NAME=algorithms/searching/binary-search
make benchmark NAME=algorithms/searching/binary-search BENCHMARK=binary_search
```

At 10,000 items per sample on this development machine, measured medians were
27.55 ns/op for a midpoint match, 27.81 ns/op for a first-item match,
18.25 ns/op for a last-item match, and 28.04 ns/op for a missing key.
