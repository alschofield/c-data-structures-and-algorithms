# Prefix Trie

Character-tree over strings where each root-to-node path spells a prefix and
marked nodes denote complete keys.

## Required API

```c
bool prefix_trie_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that. The contract below specifies the trie type and
operations (create/destroy, insert, contains, starts_with prefix query,
remove, size).

## Contract

- Each node holds child links per character plus an is-end-of-key flag; the
  flag distinguishes a stored key from a mere prefix of one.
- `insert` walks the key creating missing nodes; inserting a duplicate key is
  idempotent and does not double-count size.
- `contains` matches whole keys only; `starts_with` matches any stored key
  extending the prefix. The empty string is a valid prefix.
- `remove` clears the end-of-key flag and prunes nodes that are no longer on
  any key's path; removing an absent key fails cleanly and changes nothing.
- Keys are `NUL`-terminated strings; the trie copies nothing it does not own
  and `destroy` frees trie nodes only.
- Operations never modify the trie on lookup paths.

## Complexity Targets

- `insert`, `contains`, `remove`, `starts_with`: O(m) for key/prefix length m,
  independent of the number of stored keys
- Space: O(total characters across stored keys) nodes in the worst case;
  shared prefixes share nodes

## Learning Focus

The trie replaces hashing and comparison with structural position: a key's
characters are its address. Implementing it makes the key-versus-prefix
distinction concrete via the end flag, and remove-with-pruning is a good
exercise in freeing shared structure safely. It explains why autocomplete and
prefix routing use tries rather than hash tables, which cannot answer prefix
queries.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
