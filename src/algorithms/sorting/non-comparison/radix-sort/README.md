# Radix Sort

Non-comparison integer sort that sorts by one digit at a time using a stable
counting sort per digit, least significant digit first.

## How It Works

Sort by one digit at a time, least significant digit first, using a stable
counting sort for each pass. Later passes look like they should scramble
earlier work, but they cannot: when two keys tie on the current digit, the
stable inner sort keeps them in the order the previous pass established, so
earlier-digit ordering survives as the tiebreak. After the most significant
digit's pass, the array is sorted. LSD order plus a stable inner sort is the
entire trick; break either and the algorithm fails. The digit count comes
from the key width (for example four base-256 passes for 32-bit keys), never
from comparisons.

## Required API

```c
bool radix_sort(uint32_t *items, size_t count);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.

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

## Complexity Targets

- Best: O(d(n + k)) for d digits in radix k
- Average: O(d(n + k))
- Worst: O(d(n + k))
- Space: O(n + k) auxiliary
