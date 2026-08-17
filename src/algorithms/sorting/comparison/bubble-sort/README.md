# Bubble Sort

Comparison sort that repeatedly sweeps the array, swapping adjacent
out-of-order pairs until a full pass makes no swaps.

## Required API

```c
bool bubble_sort_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

## Contract

- Sorts in place into ascending order under the caller's comparison.
- Stable: equal elements keep their original relative order (adjacent swaps
  only, and never swap on equality).
- Must implement the early-exit optimization: a pass with zero swaps
  terminates the sort.
- Zero-length and single-element inputs are no-ops; a `NULL` array with
  nonzero length is invalid input handled without undefined behavior.
- After each pass `k`, the largest `k` elements occupy their final positions
  at the tail.
- Scaffold gate: `bubble_sort_is_implemented` returns `false`; the test
  asserts exactly that.

## Complexity Targets

- Best: O(n) (already sorted input, early exit after one pass)
- Average: O(n^2)
- Worst: O(n^2) (reverse-sorted input)
- Space: O(1), in place

## Learning Focus

Bubble sort teaches the anatomy of a comparison sort in its simplest form:
the invariant that grows a sorted suffix, why adjacent-only swaps guarantee
stability, and how a cheap flag turns a quadratic algorithm into a linear-time
verifier of already-sorted input. It is the reference point for appreciating
what merge and quick sort buy with their extra structure.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
