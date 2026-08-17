# Counting Sort

Non-comparison integer sort that counts key occurrences, prefix-sums the
counts into positions, and places elements directly.

## Required API

```c
bool counting_sort_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

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
- Scaffold gate: `counting_sort_is_implemented` returns `false`; the test
  asserts exactly that.

## Complexity Targets

- Best: O(n + k)
- Average: O(n + k)
- Worst: O(n + k)
- Space: O(n + k) auxiliary

## Learning Focus

Counting sort is the proof that the O(n log n) lower bound applies only to
comparison sorts — with structural knowledge of the keys you can sort in
linear time. Implementing the prefix-sum placement pass teaches how counts
become positions, and preserving stability in that pass is the exact skill
radix sort depends on.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
