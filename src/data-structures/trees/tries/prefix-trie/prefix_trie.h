// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Names the private trie-node type.
typedef struct Node Node;
// Names the opaque prefix trie.
typedef struct PrefixTrie PrefixTrie;

// Allocates an empty prefix trie.
PrefixTrie *prefix_trie_create(void);
// Frees trie nodes.
void prefix_trie_destroy(PrefixTrie *trie);
// Inserts one NUL-terminated key.
bool prefix_trie_insert(PrefixTrie *trie, const char *key);
// Reports whether one complete key exists.
bool prefix_trie_contains(const PrefixTrie *trie, const char *key);
// Reports whether any stored key begins with prefix.
bool prefix_trie_starts_with(const PrefixTrie *trie, const char *prefix);
// Removes one complete key and prunes unused nodes.
bool prefix_trie_remove(PrefixTrie *trie, const char *key);
// Returns the number of stored complete keys.
size_t prefix_trie_size(const PrefixTrie *trie);
