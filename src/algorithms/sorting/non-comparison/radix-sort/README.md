# Radix Sort

Non-comparison integer sort that sorts by one digit at a time using a stable
counting sort per digit, least significant digit first.

## Required API

```c
bool radix_sort_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

## Contract

- LSD order: process digits from least to most significant; correctness
  depends on it.
- Each per-digit pass must be a stable sort (counting sort by that digit);
  an unstable inner pass breaks the whole algorithm.
- Digit extraction uses a fixed radix (for example base 256 via byte masks);
  the digit count `d` is determined by the key width, not by comparisons.
- Overall the sort is stable: equal keys keep their original relative order.
- Requires an O(n + k) auxiliary buffer per pass (reusable across passes);
  allocation failure must be reported cleanly and leave the input unmodified.
- Zero-length input is a no-op; a `NULL` array with nonzero length is invalid
  input handled without undefined behavior.
- Scaffold gate: `radix_sort_is_implemented` returns `false`; the test asserts
  exactly that.

## Complexity Targets

- Best: O(d(n + k)) for d digits in radix k
- Average: O(d(n + k))
- Worst: O(d(n + k))
- Space: O(n + k) auxiliary

## Learning Focus

Radix sort shows how a stable subroutine composes into a bigger algorithm:
LSD ordering works only because earlier (less significant) passes are never
undone by later stable passes. Implementing it makes the radix/digit-count
trade-off concrete — larger radix means fewer passes but bigger count arrays —
and demonstrates linear-time sorting of fixed-width keys in practice.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
