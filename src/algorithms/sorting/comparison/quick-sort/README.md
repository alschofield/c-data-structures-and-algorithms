# Quick Sort

In-place divide-and-conquer comparison sort that partitions around a pivot and
recursively sorts both sides.

## Required API

```c
bool quick_sort_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

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
- Scaffold gate: `quick_sort_is_implemented` returns `false`; the test asserts
  exactly that.

## Complexity Targets

- Best: O(n log n)
- Average: O(n log n)
- Worst: O(n^2) (adversarial pivots)
- Space: O(log n) expected recursion depth (recurse on the smaller side),
  in place otherwise

## Learning Focus

Quick sort teaches partitioning, the workhorse primitive behind quickselect
and many divide-and-conquer routines. It is the clearest case study in
average-case versus worst-case analysis: the same code is O(n log n) or
O(n^2) depending entirely on pivot quality. Implementing it also forces
recursion-depth discipline — recurse into the smaller partition to bound the
stack at O(log n).

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
