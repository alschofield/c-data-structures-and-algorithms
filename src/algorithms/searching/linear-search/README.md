# Linear Search

Sequential scan that finds a target by comparing every element in order until a
match is found or the input is exhausted.

## Required API

```c
bool linear_search_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

## Contract

- Works on unsorted input; no ordering precondition may be assumed.
- Returns the index of the first matching element when duplicates exist.
- A missing target is reported distinctly (no valid index), never by returning
  an arbitrary position.
- An empty input or `NULL` array pointer is a clean not-found result, not
  undefined behavior.
- The input array is never modified.
- Scaffold gate: `linear_search_is_implemented` returns `false`; the test
  asserts exactly that.

## Complexity Targets

- Best: O(1) (target at index 0)
- Average: O(n)
- Worst: O(n) (target absent or last)
- Space: O(1)

## Learning Focus

Linear search is the baseline every other search is measured against. Writing
it precisely forces attention on the details that matter everywhere else:
first-match semantics under duplicates, an unambiguous not-found signal, and
edge handling for empty input. It also anchors the intuition for why sorted
input plus binary search is worth the preprocessing cost.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
