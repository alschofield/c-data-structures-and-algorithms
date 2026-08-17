# Binary Search

Divide-and-conquer search over a sorted array that halves the candidate range
on every comparison.

## Required API

```c
bool binary_search_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

## Contract

- Input must already be sorted ascending under the caller's comparison; the
  function may assume but never verify or re-sort it.
- Midpoint arithmetic must not overflow (`low + (high - low) / 2`, not
  `(low + high) / 2`).
- A missing target is reported distinctly; when duplicates exist, any matching
  index is acceptable unless a lower-bound variant is specified.
- Empty input or a `NULL` array pointer is a clean not-found result.
- The input array is never modified.
- Scaffold gate: `binary_search_is_implemented` returns `false`; the test
  asserts exactly that.

## Complexity Targets

- Best: O(1) (target at first midpoint)
- Average: O(log n)
- Worst: O(log n)
- Space: O(1) iterative

## Learning Focus

Binary search is short but notoriously easy to get subtly wrong: off-by-one
bounds, non-terminating loops, and midpoint overflow are classic defects.
Implementing it from first principles builds the loop-invariant discipline
("the target, if present, is always inside [low, high]") that generalizes to
partition-based algorithms and to reasoning about any halving process.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
