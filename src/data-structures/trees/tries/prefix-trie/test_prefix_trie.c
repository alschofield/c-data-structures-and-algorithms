#include "prefix_trie.h"

#include <assert.h>
#include <stddef.h>

static void test_empty_trie(void) {
    PrefixTrie *trie = prefix_trie_create();

    assert(trie != NULL);
    assert(prefix_trie_size(trie) == 0U);
    assert(!prefix_trie_contains(trie, "cat"));
    assert(!prefix_trie_starts_with(trie, "c"));
    assert(!prefix_trie_remove(trie, "cat"));
    prefix_trie_destroy(trie);
}

static void test_keys_versus_prefixes(void) {
    PrefixTrie *trie = prefix_trie_create();

    assert(trie != NULL);
    assert(prefix_trie_insert(trie, "carton"));
    assert(prefix_trie_insert(trie, "car"));
    assert(prefix_trie_size(trie) == 2U);

    assert(prefix_trie_contains(trie, "car"));
    assert(prefix_trie_contains(trie, "carton"));
    assert(!prefix_trie_contains(trie, "cart"));
    assert(!prefix_trie_contains(trie, "ca"));

    assert(prefix_trie_starts_with(trie, "car"));
    assert(prefix_trie_starts_with(trie, "cart"));
    assert(prefix_trie_starts_with(trie, ""));
    assert(!prefix_trie_starts_with(trie, "dog"));
    prefix_trie_destroy(trie);
}

static void test_duplicate_insert_is_idempotent(void) {
    PrefixTrie *trie = prefix_trie_create();

    assert(trie != NULL);
    assert(prefix_trie_insert(trie, "cat"));
    assert(prefix_trie_insert(trie, "cat"));
    assert(prefix_trie_size(trie) == 1U);
    prefix_trie_destroy(trie);
}

static void test_remove_prunes_and_preserves(void) {
    PrefixTrie *trie = prefix_trie_create();

    assert(trie != NULL);
    assert(prefix_trie_insert(trie, "car"));
    assert(prefix_trie_insert(trie, "carton"));

    assert(prefix_trie_remove(trie, "carton"));
    assert(prefix_trie_size(trie) == 1U);
    assert(prefix_trie_contains(trie, "car"));
    assert(!prefix_trie_contains(trie, "carton"));
    assert(!prefix_trie_starts_with(trie, "cart"));

    assert(!prefix_trie_remove(trie, "carton"));
    assert(!prefix_trie_remove(trie, "ca"));
    assert(prefix_trie_size(trie) == 1U);

    assert(prefix_trie_remove(trie, "car"));
    assert(prefix_trie_size(trie) == 0U);
    assert(!prefix_trie_starts_with(trie, "c"));
    prefix_trie_destroy(trie);
}

static void test_null_container(void) {
    assert(!prefix_trie_insert(NULL, "cat"));
    assert(!prefix_trie_contains(NULL, "cat"));
    assert(!prefix_trie_starts_with(NULL, "c"));
    assert(!prefix_trie_remove(NULL, "cat"));
    assert(prefix_trie_size(NULL) == 0U);
    prefix_trie_destroy(NULL);
}

int main(void) {
    test_empty_trie();
    test_keys_versus_prefixes();
    test_duplicate_insert_is_idempotent();
    test_remove_prunes_and_preserves();
    test_null_container();
    return 0;
}
