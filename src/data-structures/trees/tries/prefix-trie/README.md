# Prefix Trie

Character-tree over strings where each root-to-node path spells a prefix and
marked nodes denote complete keys.

## How It Works

Strings stored as a tree of characters: the root is the empty string, each
edge adds one letter, and a root-to-node path spells a prefix. "car" and
"carton" share the c-a-r path and diverge after it, so shared prefixes are
stored exactly once. An end-of-key flag on each node distinguishes a stored
word from a mere waypoint — "car" can be a real entry while "cart" is only a
path on the way to "carton".

Every operation costs O(m) in the key's length, independent of how many keys
the trie holds — the property neither a hash table nor a BST can offer, and
what makes prefix queries (starts_with) natural. The delicate operation is
remove: clear the flag, then prune nodes that no longer lead to any stored
key without cutting a branch another key still needs.

## Required API

```c
typedef struct PrefixTrie PrefixTrie;

PrefixTrie *prefix_trie_create(void);
void prefix_trie_destroy(PrefixTrie *trie);
bool prefix_trie_insert(PrefixTrie *trie, const char *key);
bool prefix_trie_contains(const PrefixTrie *trie, const char *key);
bool prefix_trie_starts_with(const PrefixTrie *trie, const char *prefix);
bool prefix_trie_remove(PrefixTrie *trie, const char *key);
size_t prefix_trie_size(const PrefixTrie *trie);
```

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

## Source Material

- https://www.youtube.com/watch?v=OLPebSpTISE
