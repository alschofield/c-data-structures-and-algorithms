#ifndef PREFIX_TRIE_H
#define PREFIX_TRIE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct PrefixTrie PrefixTrie;

PrefixTrie *prefix_trie_create(void);
void prefix_trie_destroy(PrefixTrie *trie);
bool prefix_trie_insert(PrefixTrie *trie, const char *key);
bool prefix_trie_contains(const PrefixTrie *trie, const char *key);
bool prefix_trie_starts_with(const PrefixTrie *trie, const char *prefix);
bool prefix_trie_remove(PrefixTrie *trie, const char *key);
size_t prefix_trie_size(const PrefixTrie *trie);

#endif
