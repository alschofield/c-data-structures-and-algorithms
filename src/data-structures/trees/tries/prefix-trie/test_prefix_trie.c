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

static void test_remove_preserves_siblings(void) {
    PrefixTrie *trie = prefix_trie_create();

    assert(trie != NULL);
    assert(prefix_trie_insert(trie, "car"));
    assert(prefix_trie_insert(trie, "cat"));

    assert(prefix_trie_remove(trie, "car"));
    assert(prefix_trie_size(trie) == 1U);
    assert(!prefix_trie_contains(trie, "car"));
    assert(prefix_trie_contains(trie, "cat"));
    assert(prefix_trie_starts_with(trie, "ca"));
    prefix_trie_destroy(trie);
}

static void test_remove_prefix_only_fails(void) {
    PrefixTrie *trie = prefix_trie_create();

    assert(trie != NULL);
    assert(prefix_trie_insert(trie, "carton"));

    assert(!prefix_trie_remove(trie, "cart"));
    assert(prefix_trie_size(trie) == 1U);
    assert(prefix_trie_contains(trie, "carton"));
    prefix_trie_destroy(trie);
}

static void test_remove_word_prefix_preserves_descendant(void) {
    PrefixTrie *trie = prefix_trie_create();

    assert(trie != NULL);
    assert(prefix_trie_insert(trie, "car"));
    assert(prefix_trie_insert(trie, "carton"));

    assert(prefix_trie_remove(trie, "car"));
    assert(prefix_trie_size(trie) == 1U);
    assert(!prefix_trie_contains(trie, "car"));
    assert(prefix_trie_contains(trie, "carton"));
    prefix_trie_destroy(trie);
}

static void test_reuse_after_emptying_trie(void) {
    PrefixTrie *trie = prefix_trie_create();

    assert(trie != NULL);
    assert(prefix_trie_insert(trie, "cat"));
    assert(prefix_trie_remove(trie, "cat"));
    assert(prefix_trie_size(trie) == 0U);

    assert(prefix_trie_insert(trie, "dog"));
    assert(prefix_trie_size(trie) == 1U);
    assert(prefix_trie_contains(trie, "dog"));
    prefix_trie_destroy(trie);
}

static void test_null_container(void) {
    PrefixTrie *trie = prefix_trie_create();

    assert(!prefix_trie_insert(NULL, "cat"));
    assert(!prefix_trie_contains(NULL, "cat"));
    assert(!prefix_trie_starts_with(NULL, "c"));
    assert(!prefix_trie_remove(NULL, "cat"));
    assert(prefix_trie_size(NULL) == 0U);
    prefix_trie_destroy(NULL);

    assert(trie != NULL);
    assert(!prefix_trie_insert(trie, ""));
    assert(!prefix_trie_contains(trie, ""));
    assert(prefix_trie_starts_with(trie, ""));
    assert(!prefix_trie_remove(trie, ""));
    assert(!prefix_trie_insert(trie, NULL));
    assert(!prefix_trie_contains(trie, NULL));
    assert(!prefix_trie_starts_with(trie, NULL));
    assert(!prefix_trie_remove(trie, NULL));
    assert(prefix_trie_size(trie) == 0U);
    prefix_trie_destroy(trie);
}

struct GraphLog { size_t indexes[2]; size_t count; };

static bool record_graph_index(size_t index, uint64_t weight, void *context) {
    (void)weight;
    struct GraphLog *log = context;
    log->indexes[log->count++] = index;
    return true;
}

static void test_graph_view_adapter(void) {
    PrefixTrie *trie = prefix_trie_create();
    GraphView view = { 0 };
    struct GraphLog log = { 0 };

    assert(trie != NULL);
    assert(prefix_trie_insert(trie, "a"));
    assert(prefix_trie_insert(trie, "b"));
    assert(prefix_trie_graph_view(trie, &view));
    assert(graph_view_is_valid(&view) && graph_view_is_directed(&view));
    assert(graph_view_vertex_count(&view) == 3U && graph_view_node_at(&view, 2U));
    assert(graph_view_neighbors(&view, 0U, record_graph_index, &log));
    assert(log.count == 2U && log.indexes[0] == 1U && log.indexes[1] == 2U);
    prefix_trie_destroy(trie);
}

int main(void) {
    test_empty_trie();
    test_keys_versus_prefixes();
    test_duplicate_insert_is_idempotent();
    test_remove_prunes_and_preserves();
    test_remove_preserves_siblings();
    test_remove_prefix_only_fails();
    test_remove_word_prefix_preserves_descendant();
    test_reuse_after_emptying_trie();
    test_null_container();
    test_graph_view_adapter();
    return 0;
}
