// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public prefix trie API.
#include "../src/data-structures/trees/tries/prefix-trie/prefix_trie.h"
// Declares snprintf.
#include <stdio.h>

// Uses a substantial key set while keeping all benchmark setup practical.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 10000U
#endif

// Defines the number of distinct keys used in every benchmark batch.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };
// Holds every generated key and its terminating NUL byte.
enum { KEY_CAPACITY = 32 };

// Stores state shared by prefix-trie operation benchmarks.
struct TrieContext {
    // Owns the trie created before each benchmark batch.
    PrefixTrie *trie;
    // Stores stable, caller-owned NUL-terminated keys.
    char keys[ITEM_COUNT][KEY_CAPACITY];
    // Tracks the next distinct key for an operation.
    size_t next_index;
};

// Creates an empty trie before an insertion sample.
static bool empty_setup(void *context) {
    struct TrieContext *trie_context = context;

    trie_context->trie = prefix_trie_create();
    trie_context->next_index = 0U;
    return trie_context->trie != NULL;
}

// Creates and fills a trie outside the timed region for lookup and removal.
static bool populated_setup(void *context) {
    struct TrieContext *trie_context = context;

    trie_context->trie = prefix_trie_create();
    if (trie_context->trie == NULL) {
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!prefix_trie_insert(trie_context->trie, trie_context->keys[index])) {
            prefix_trie_destroy(trie_context->trie);
            trie_context->trie = NULL;
            return false;
        }
    }

    trie_context->next_index = 0U;
    return true;
}

// Inserts one distinct key into an initially empty trie.
static bool insert_operation(void *context) {
    struct TrieContext *trie_context = context;

    if (trie_context->next_index == ITEM_COUNT || !prefix_trie_insert(
            trie_context->trie,
            trie_context->keys[trie_context->next_index]
        )) {
        return false;
    }

    trie_context->next_index++;
    return true;
}

// Looks up one exact populated key.
static bool contains_operation(void *context) {
    struct TrieContext *trie_context = context;

    if (trie_context->next_index == ITEM_COUNT || !prefix_trie_contains(
            trie_context->trie,
            trie_context->keys[trie_context->next_index]
        )) {
        return false;
    }

    trie_context->next_index++;
    return true;
}

// Looks up the common prefix shared by every generated key.
static bool starts_with_operation(void *context) {
    struct TrieContext *trie_context = context;

    return prefix_trie_starts_with(trie_context->trie, "word-");
}

// Removes one distinct populated key.
static bool remove_operation(void *context) {
    struct TrieContext *trie_context = context;

    if (trie_context->next_index == ITEM_COUNT || !prefix_trie_remove(
            trie_context->trie,
            trie_context->keys[trie_context->next_index]
        )) {
        return false;
    }

    trie_context->next_index++;
    return true;
}

// Confirms every insertion created exactly one complete stored key.
static bool insert_verify(void *context) {
    struct TrieContext *trie_context = context;

    return trie_context->next_index == ITEM_COUNT &&
        prefix_trie_size(trie_context->trie) == ITEM_COUNT;
}

// Confirms every distinct lookup completed.
static bool lookup_verify(void *context) {
    struct TrieContext *trie_context = context;

    return trie_context->next_index == ITEM_COUNT;
}

// Confirms the populated trie retains the common prefix after the timed batch.
static bool shared_prefix_verify(void *context) {
    struct TrieContext *trie_context = context;

    return prefix_trie_starts_with(trie_context->trie, "word-");
}

// Confirms every stored key was removed.
static bool remove_verify(void *context) {
    struct TrieContext *trie_context = context;

    return trie_context->next_index == ITEM_COUNT &&
        prefix_trie_size(trie_context->trie) == 0U;
}

// Releases the trie created for one benchmark batch.
static void trie_teardown(void *context) {
    struct TrieContext *trie_context = context;

    prefix_trie_destroy(trie_context->trie);
    trie_context->trie = NULL;
}

// Runs and prints one configured prefix-trie benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct TrieContext trie_context;
    BenchmarkConfig config = {
        .name = "Prefix trie insert",
        .warmup_iterations = ITEM_COUNT,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = empty_setup,
        .operation = insert_operation,
        .verify = insert_verify,
        .teardown = trie_teardown,
        .context = &trie_context
    };

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (snprintf(
                trie_context.keys[index],
                KEY_CAPACITY,
                "word-%05zu",
                index
            ) < 0) {
            return 1;
        }
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Prefix trie contains";
    config.setup = populated_setup;
    config.operation = contains_operation;
    config.verify = lookup_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Prefix trie starts_with shared prefix";
    config.operation = starts_with_operation;
    config.verify = shared_prefix_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Prefix trie remove";
    config.operation = remove_operation;
    config.verify = remove_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
