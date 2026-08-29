# Quick Sort

In-place divide-and-conquer comparison sort that partitions around a pivot and
recursively sorts both sides.

## How It Works

Partition, then recurse. Pick a pivot, then rearrange the array into smaller,
equal, and larger regions. The equal region needs no recursion; recurse only
on the two outer regions. Nothing merges; the partition itself is the sort.

All the danger is pivot choice: a fixed first/last pivot turns sorted input
into the O(n^2) worst case because one side of every partition is empty. The
implementation uses the midpoint item, avoiding that immediate sorted/reverse
pathology while still admitting adversarial O(n^2) input. Partitioning moves
equal elements across each other, so the sort is not stable.

## Required API

```c
typedef int (*QuickSortCompareFn)(const void *left, const void *right);

bool quick_sort(void **items, size_t count, QuickSortCompareFn compare);
```

The checked-in implementation uses an in-place three-way partition around a
saved midpoint pivot value.

## Contract

- Sorts in place into ascending order under the caller's comparison.
- Not stable; partitioning moves equal elements across each other. Do not
  claim stability.
- Pivot selection must defend against sorted and reverse-sorted input, not use
  a fixed first/last element.
- Partition invariant: every smaller item is left of the equal region and
  every larger item is right of it.
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

## Verification

```text
make test NAME=algorithms/sorting/comparison/quick-sort
make benchmark NAME=algorithms/sorting/comparison/quick-sort BENCHMARK=quick_sort
```

At 10,000 items per whole-sort sample on this development machine, measured
medians were 0.628 ms for shuffled input, 0.171 ms for sorted input, and
0.176 ms for reverse input.
