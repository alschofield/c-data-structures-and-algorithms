# Insertion Sort

Comparison sort that grows a sorted prefix by shifting each new element left
until it reaches its correct position.

## Required API

```c
bool insertion_sort_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

## Contract

- Sorts in place into ascending order under the caller's comparison.
- Stable: shift while strictly greater, insert after equal elements, so equal
  elements keep their original relative order.
- Adaptive: nearly sorted input costs close to O(n); each element moves only
  as far as its displacement.
- Zero-length and single-element inputs are no-ops; a `NULL` array with
  nonzero length is invalid input handled without undefined behavior.
- Invariant: before processing index `i`, the range `[0, i)` is sorted.
- Scaffold gate: `insertion_sort_is_implemented` returns `false`; the test
  asserts exactly that.

## Complexity Targets

- Best: O(n) (already sorted input, one comparison per element)
- Average: O(n^2)
- Worst: O(n^2) (reverse-sorted input)
- Space: O(1), in place

## Learning Focus

Insertion sort is the standard example of an adaptive algorithm: its cost is
proportional to the number of inversions, not just n. That makes it the
practical base case inside merge sort and quick sort for small subarrays.
Implementing the shift-then-place pattern (hold the element, shift, insert
once) instead of repeated swaps teaches a real constant-factor optimization.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
