# Counting Sort

Non-comparison integer sort that counts key occurrences, prefix-sums the
counts into positions, and places elements directly.

## Required API

```c
bool counting_sort(uint32_t *items, size_t count, uint32_t key_limit);
```

The checked-in source is still the scaffold gate `bool
counting_sort_is_implemented(void)`, which returns `false`; the test asserts exactly that.
Keys must lie in `[0, key_limit)`.

## Contract

- Applies to integer keys in a known range `[0, k)` (or an offset range); the
  key range is a precondition, not something discovered by comparison.
- Stable: the output-placement pass must iterate the input in reverse (or use
  an equivalent scheme) so equal keys keep their original relative order.
  Stability here is what makes radix sort possible.
- Uses a counts array of size `k` and prefix sums to compute each key's final
  output offset; no element comparisons anywhere.
- Requires an O(n + k) auxiliary allocation; allocation failure must be
  reported cleanly and leave the input unmodified.
- Zero-length input is a no-op; keys outside the declared range are invalid
  input handled without undefined behavior.

## Complexity Targets

- Best: O(n + k)
- Average: O(n + k)
- Worst: O(n + k)
- Space: O(n + k) auxiliary
