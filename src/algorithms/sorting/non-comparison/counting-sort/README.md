# Counting Sort

Non-comparison integer sort that counts key occurrences, prefix-sums the
counts into positions, and places elements directly.

## How It Works

No comparisons at all. Keys are small integers in a known range [0, k), so
counting replaces comparing: tally how many of each key exist, prefix-sum the
tallies so each key knows where its block of the output starts, then place
every element directly into its computed slot. Because nothing is compared,
the O(n log n) lower bound on comparison sorts does not apply — the cost is
O(n + k). The placement pass iterates the input in reverse so equal keys keep
their original order; that stability is not a nicety, it is the property
radix sort is built on.

## Required API

```c
bool counting_sort(uint32_t *items, size_t count, uint32_t key_limit);
```

The checked-in implementation uses raw counts, prefix sums, reverse stable
placement, and copy-back. Keys must lie in `[0, key_limit)`.

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

## Verification

```text
make test NAME=algorithms/sorting/non-comparison/counting-sort
make benchmark NAME=algorithms/sorting/non-comparison/counting-sort BENCHMARK=counting_sort
```

At 10,000 items per whole-sort sample on this development machine, a 256-key
range measured 0.011 ms and a 65,536-key range measured 0.044 ms. The wider
range spends more time initializing and prefix-summing its larger counts array,
demonstrating the `O(n + k)` tradeoff.
