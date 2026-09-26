# Prefix Trie

## Implementation Status

Implemented. This document describes the current learner-owned implementation contract.

## How It Works

An opaque sparse trie of NUL-terminated byte strings. Existing children are stored compactly and complete-key markers distinguish keys from prefixes.

Each edge represents part of a key, so shared prefixes share storage and prefix lookup follows a path.

## Required API

```c
PrefixTrie *prefix_trie_create(void); void prefix_trie_destroy(PrefixTrie *trie); bool prefix_trie_insert(PrefixTrie *trie, const char *key);
bool prefix_trie_contains(const PrefixTrie *trie, const char *key); bool prefix_trie_starts_with(const PrefixTrie *trie, const char *prefix);
bool prefix_trie_remove(PrefixTrie *trie, const char *key); size_t prefix_trie_size(const PrefixTrie *trie); bool prefix_trie_graph_view(const PrefixTrie *trie, GraphView *out_view);
```

## Contract

- Keys must be non-null, nonempty C strings. Duplicate insertion succeeds without changing size; `starts_with` accepts `""` and returns true for a valid trie.
- Removal succeeds only for a stored complete key and prunes unused suffix nodes. Size counts complete keys, not structural nodes.
- The trie owns structural allocations, never caller string storage. Allocation and capacity/byte-size overflow fail; new suffixes are fully built before linking. Its borrowed GraphView is directed, unit-weighted, and pre-order indexed including the root.

Operations are O(m * b) for string length `m` and maximum sparse child count `b`; storage is proportional to prefix nodes. Verify with `make test NAME=data-structures/trees/tries/prefix-trie`.

## Complexity Targets

Target: O(m) lookup/update by key length m and O(total stored key characters) storage.

## Verification

```sh
make test NAME=<topic-path>
```
