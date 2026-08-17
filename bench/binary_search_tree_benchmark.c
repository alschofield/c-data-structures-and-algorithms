// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public binary-search-tree API.
#include "../src/data-structures/trees/binary-search-trees/binary-search-tree/binary_search_tree.h"

// Uses the default sample size unless Make supplies an experiment-specific size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 10000U
#endif

// Defines the number of operations in each timed sample.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Defines one caller-owned record stored in the benchmark tree.
struct Record {
    int id;
};

// Compares benchmark records by their integer identifier.
static int record_compare(const void *left, const void *right) {
    const struct Record *left_record = left;
    const struct Record *right_record = right;

    return (left_record->id > right_record->id) - (left_record->id < right_record->id);
}

// Stores state shared by binary-search-tree operation benchmarks.
struct TreeContext {
    // Owns the tree created before each sample.
    BinarySearchTree *tree;
    // Stores caller-owned records inserted into the tree.
    struct Record records[ITEM_COUNT];
    // Stores the shuffled insertion order that keeps the tree from degenerating.
    size_t order[ITEM_COUNT];
    // Tracks the next record to operate on.
    size_t next_index;
    // Receives values returned by find and remove.
    void *out_value;
};

// Fills the order array with a deterministic pseudo-random permutation.
static void shuffle_order(struct TreeContext *tree_context) {
    // Starts from the identity permutation.
    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        tree_context->order[index] = index;
    }

    // Applies a Fisher-Yates shuffle with a fixed linear-congruential sequence
    // so every run inserts in the same order and results stay comparable.
    unsigned long long state = 0x2545F4914F6CDD1DULL;
    for (size_t index = ITEM_COUNT - 1U; index > 0U; index--) {
        state = state * 6364136223846793005ULL + 1442695040888963407ULL;
        size_t swap_index = (size_t)(state % (unsigned long long)(index + 1U));
        size_t held = tree_context->order[index];
        tree_context->order[index] = tree_context->order[swap_index];
        tree_context->order[swap_index] = held;
    }
}

// Creates and populates a tree before a find, contains, or removal sample.
static bool populated_setup(void *context) {
    struct TreeContext *tree_context = context;

    tree_context->tree = binary_search_tree_create(record_compare);
    if (tree_context->tree == NULL) {
        return false;
    }

    // Inserts in shuffled order so the unbalanced tree stays near O(log n).
    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!binary_search_tree_insert(
                tree_context->tree,
                &tree_context->records[tree_context->order[index]]
            )) {
            binary_search_tree_destroy(tree_context->tree);
            tree_context->tree = NULL;
            return false;
        }
    }

    tree_context->next_index = 0U;
    return true;
}

// Creates an empty tree before an insertion sample.
static bool insert_setup(void *context) {
    struct TreeContext *tree_context = context;

    tree_context->tree = binary_search_tree_create(record_compare);
    tree_context->next_index = 0U;
    return tree_context->tree != NULL;
}

// Inserts one distinct record in shuffled order.
static bool insert_operation(void *context) {
    struct TreeContext *tree_context = context;

    if (tree_context->next_index == ITEM_COUNT || !binary_search_tree_insert(
            tree_context->tree,
            &tree_context->records[tree_context->order[tree_context->next_index]]
        )) {
        return false;
    }

    tree_context->next_index++;
    return true;
}

// Looks up one populated record.
static bool find_operation(void *context) {
    struct TreeContext *tree_context = context;

    if (tree_context->next_index == ITEM_COUNT || !binary_search_tree_find(
            tree_context->tree,
            &tree_context->records[tree_context->next_index],
            &tree_context->out_value
        )) {
        return false;
    }

    tree_context->next_index++;
    return true;
}

// Checks for one populated record.
static bool contains_operation(void *context) {
    struct TreeContext *tree_context = context;

    if (tree_context->next_index == ITEM_COUNT || !binary_search_tree_contains(
            tree_context->tree,
            &tree_context->records[tree_context->next_index]
        )) {
        return false;
    }

    tree_context->next_index++;
    return true;
}

// Removes one populated record in shuffled order.
static bool remove_operation(void *context) {
    struct TreeContext *tree_context = context;

    if (tree_context->next_index == ITEM_COUNT || !binary_search_tree_remove(
            tree_context->tree,
            &tree_context->records[tree_context->order[tree_context->next_index]],
            &tree_context->out_value
        )) {
        return false;
    }

    tree_context->next_index++;
    return true;
}

// Confirms insertion populated one record for every completed operation.
static bool insert_verify(void *context) {
    struct TreeContext *tree_context = context;

    return binary_search_tree_size(tree_context->tree) == tree_context->next_index;
}

// Confirms every requested lookup or membership check completed.
static bool lookup_verify(void *context) {
    struct TreeContext *tree_context = context;

    return tree_context->next_index > 0U &&
        tree_context->next_index <= ITEM_COUNT;
}

// Confirms removal changed the size by one record per completed operation.
static bool remove_verify(void *context) {
    struct TreeContext *tree_context = context;

    return tree_context->next_index > 0U &&
        tree_context->next_index <= ITEM_COUNT &&
        binary_search_tree_size(tree_context->tree) ==
            ITEM_COUNT - tree_context->next_index;
}

// Releases the tree created for one benchmark batch.
static void tree_teardown(void *context) {
    struct TreeContext *tree_context = context;

    binary_search_tree_destroy(tree_context->tree);
    tree_context->tree = NULL;
}

// Runs and prints one configured binary-search-tree benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct TreeContext tree_context;
    BenchmarkConfig config = {
        .name = "Binary search tree insert",
        .warmup_iterations = ITEM_COUNT,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = insert_setup,
        .operation = insert_operation,
        .verify = insert_verify,
        .teardown = tree_teardown,
        .context = &tree_context
    };

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        tree_context.records[index].id = (int)index;
    }
    shuffle_order(&tree_context);

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Binary search tree find";
    config.setup = populated_setup;
    config.operation = find_operation;
    config.verify = lookup_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Binary search tree contains";
    config.operation = contains_operation;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Binary search tree remove";
    config.operation = remove_operation;
    config.verify = remove_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
