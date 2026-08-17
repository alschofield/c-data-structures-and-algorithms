# Selection Sort

Comparison sort that grows a sorted prefix by repeatedly selecting the minimum
of the unsorted remainder and swapping it into place.

## Required API

```c
bool selection_sort_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

## Contract

- Sorts in place into ascending order under the caller's comparison.
- Performs at most n - 1 swaps total; this is the algorithm's defining
  advantage when writes are expensive.
- Not stable in its classic swap form; do not claim stability.
- Zero-length and single-element inputs are no-ops; a `NULL` array with
  nonzero length is invalid input handled without undefined behavior.
- Invariant: after iteration `k`, the first `k` elements are the `k` smallest
  in final sorted order.
- Scaffold gate: `selection_sort_is_implemented` returns `false`; the test
  asserts exactly that.

## Complexity Targets

- Best: O(n^2) (comparisons do not shrink on sorted input)
- Average: O(n^2)
- Worst: O(n^2)
- Space: O(1), in place

## Learning Focus

Selection sort separates two costs that other sorts blend: comparisons versus
data movement. Its comparison count is fixed regardless of input order, which
demonstrates that "best case" depends on what an algorithm can actually skip.
Implementing it also shows concretely why long-distance swaps destroy
stability, a property adjacent-swap sorts get for free.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
