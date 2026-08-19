# Quick Sort

In-place divide-and-conquer comparison sort that partitions around a pivot and
recursively sorts both sides.

## How It Works

Partition, then recurse. Pick a pivot, then rearrange the array so
everything smaller sits left of it and everything larger right — after one
partition pass the pivot occupies its final sorted position. Recurse on the
two sides. Nothing merges; the partition itself is the sort.

All the danger is pivot choice: a fixed first/last pivot turns sorted input
into the O(n^2) worst case because one side of every partition is empty.
Median-of-three or randomized selection defends against that. Partitioning
moves equal elements across each other, so the sort is not stable.

## Required API

```c
typedef int (*QuickSortCompareFn)(const void *left, const void *right);

bool quick_sort(void **items, size_t count, QuickSortCompareFn compare);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.

## Contract

- Sorts in place into ascending order under the caller's comparison.
- Not stable; partitioning moves equal elements across each other. Do not
  claim stability.
- Pivot selection must defend against sorted and reverse-sorted input
  (median-of-three or randomized), not fixed first/last element.
- Partition invariant: after partitioning, the pivot is in its final position
  with all smaller elements left of it and all larger elements right of it.
- Must remain correct on all-equal input and on arrays full of duplicates
  without degrading to unbounded recursion.
- Zero-length and single-element inputs are no-ops; a `NULL` array with
  nonzero length is invalid input handled without undefined behavior.

## Complexity Targets

- Best: O(n log n)
- Average: O(n log n)
- Worst: O(n^2) (adversarial pivots)
- Space: O(log n) expected recursion depth (recurse on the smaller side),
  in place otherwise
