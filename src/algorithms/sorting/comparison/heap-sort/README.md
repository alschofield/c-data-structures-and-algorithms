# Heap Sort

Comparison sort that builds a max-heap in the array, then repeatedly swaps the
root to the tail and re-heapifies the shrinking prefix.

## How It Works

Two ideas glued together. First, the array is treated as an implicit tree:
the element at index i has children at 2i+1 and 2i+2, and the max-heap rule
(every parent >= its children) guarantees the largest element sits at index 0
without the array being sorted. The maintenance move is sift-down: a node
smaller than a child swaps with its larger child and repeats from its new
position until it settles.

Second, harvest: swap the root (the maximum) with the last heap element,
shrink the heap by one so that slot is final, and sift the new root down to
restore the rule. Each round locks one element at the tail — bubble sort's
shape, but finding the max costs O(log n) instead of an O(n) pass. The build
step walks backward from the last parent sifting each node down, which is
O(n) because most nodes are near the bottom and barely move. Long-distance
swaps leap over equal elements, which is why the sort cannot be stable.

## Required API

```c
typedef int (*HeapSortCompareFn)(const void *left, const void *right);

bool heap_sort(void **items, size_t count, HeapSortCompareFn compare);
```

The checked-in implementation uses in-place bottom-up max-heap construction
and repeated root extraction.

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

## Verification

```text
make test NAME=algorithms/sorting/comparison/heap-sort
make benchmark NAME=algorithms/sorting/comparison/heap-sort BENCHMARK=heap_sort
```

At 10,000 items per whole-sort sample on this development machine, measured
medians were 0.745 ms for shuffled input, 0.508 ms for sorted input, and
0.528 ms for reverse input.
