# Merge Sort

Divide-and-conquer comparison sort that recursively sorts halves and merges
them with an auxiliary buffer.

## Required API

```c
bool merge_sort_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

## Contract

- Sorts into ascending order under the caller's comparison.
- Stable: on ties the merge step must take from the left run first.
- Guaranteed O(n log n) regardless of input order; no adversarial input
  degrades it.
- Requires an O(n) auxiliary buffer; allocation failure must be reported
  cleanly and leave the input unmodified.
- Zero-length and single-element inputs are no-ops; a `NULL` array with
  nonzero length is invalid input handled without undefined behavior.
- The merge must handle uneven halves and runs that exhaust at different
  times without reading out of bounds.
- Scaffold gate: `merge_sort_is_implemented` returns `false`; the test asserts
  exactly that.

## Complexity Targets

- Best: O(n log n)
- Average: O(n log n)
- Worst: O(n log n)
- Space: O(n) auxiliary buffer (plus O(log n) recursion depth)

## Learning Focus

Merge sort is the canonical divide-and-conquer algorithm and the cleanest
proof that comparison sorting can be O(n log n) worst case. Implementing the
merge step precisely — left-first on ties, correct exhaustion handling —
demonstrates where stability actually comes from. It also introduces the
space/time trade-off: the guaranteed bound is bought with a linear buffer.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
