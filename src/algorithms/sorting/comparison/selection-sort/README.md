# Selection Sort

Comparison sort that grows a sorted prefix by repeatedly selecting the minimum
of the unsorted remainder and swapping it into place.

## How It Works

Grow a sorted prefix by selection: scan the unsorted remainder for its
minimum, swap it into the next prefix slot, repeat. Comparisons never shrink
— sorted input still costs a full scan per position, so every case is O(n^2)
— but the sort performs at most n-1 swaps total, its one real advantage when
writes are expensive. The long-distance swap can carry an element past an
equal one, so the classic form is not stable.

## Required API

```c
typedef int (*SelectionSortCompareFn)(const void *left, const void *right);

bool selection_sort(void **items, size_t count, SelectionSortCompareFn compare);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.

## Contract

- Sorts in place into ascending order under the caller's comparison.
- Performs at most n - 1 swaps total; this is the algorithm's defining
  advantage when writes are expensive.
- Not stable in its classic swap form; do not claim stability.
- Zero-length and single-element inputs are no-ops; a `NULL` array with
  nonzero length is invalid input handled without undefined behavior.
- Invariant: after iteration `k`, the first `k` elements are the `k` smallest
  in final sorted order.

## Complexity Targets

- Best: O(n^2) (comparisons do not shrink on sorted input)
- Average: O(n^2)
- Worst: O(n^2)
- Space: O(1), in place
