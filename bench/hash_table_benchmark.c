// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public hash-table API.
#include "../src/data-structures/associative/hash-tables/separate-chaining/hash_table.h"

// Uses the default sample size unless Make supplies an experiment-specific size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 10000U
#endif

// Defines the number of operations in each timed sample.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Defines one caller-owned key used by the benchmark table.
struct Key {
    int id;
};

// Converts a benchmark key into its integer hash value.
static size_t key_hash(const void *key) {
    const struct Key *typed_key = key;

    return (size_t)typed_key->id;
}

// Compares benchmark keys by their integer identifier.
static bool key_equals(const void *left, const void *right) {
    const struct Key *left_key = left;
    const struct Key *right_key = right;

    return left_key->id == right_key->id;
}

// Stores state shared by hash-table operation benchmarks.
struct HashTableContext {
    // Owns the table created before each sample.
    HashTable *table;
    // Stores caller-owned keys used by every operation.
    struct Key keys[ITEM_COUNT];
    // Stores caller-owned values inserted into the table.
    int values[ITEM_COUNT];
    // Tracks the next key to operate on.
    size_t next_index;
    // Receives values returned by set, get, and remove.
    void *out_value;
};

// Creates and populates a table before a lookup, contains, or removal sample.
static bool populated_setup(void *context) {
    struct HashTableContext *hash_table_context = context;

    hash_table_context->table = hash_table_create(key_hash, key_equals);
    if (hash_table_context->table == NULL) {
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!hash_table_set(
                hash_table_context->table,
                &hash_table_context->keys[index],
                &hash_table_context->values[index],
                &hash_table_context->out_value
            )) {
            hash_table_destroy(hash_table_context->table);
            hash_table_context->table = NULL;
            return false;
        }
    }

    hash_table_context->next_index = 0U;
    return true;
}

// Creates an empty table before an insertion sample.
static bool set_setup(void *context) {
    struct HashTableContext *hash_table_context = context;

    hash_table_context->table = hash_table_create(key_hash, key_equals);
    hash_table_context->next_index = 0U;
    return hash_table_context->table != NULL;
}

// Inserts one distinct key-value pair.
static bool set_operation(void *context) {
    struct HashTableContext *hash_table_context = context;

    if (hash_table_context->next_index == ITEM_COUNT || !hash_table_set(
            hash_table_context->table,
            &hash_table_context->keys[hash_table_context->next_index],
            &hash_table_context->values[hash_table_context->next_index],
            &hash_table_context->out_value
        )) {
        return false;
    }

    hash_table_context->next_index++;
    return true;
}

// Looks up one populated key.
static bool get_operation(void *context) {
    struct HashTableContext *hash_table_context = context;

    if (hash_table_context->next_index == ITEM_COUNT || !hash_table_get(
            hash_table_context->table,
            &hash_table_context->keys[hash_table_context->next_index],
            &hash_table_context->out_value
        )) {
        return false;
    }

    hash_table_context->next_index++;
    return true;
}

// Checks for one populated key.
static bool contains_operation(void *context) {
    struct HashTableContext *hash_table_context = context;

    if (hash_table_context->next_index == ITEM_COUNT || !hash_table_contains(
            hash_table_context->table,
            &hash_table_context->keys[hash_table_context->next_index]
        )) {
        return false;
    }

    hash_table_context->next_index++;
    return true;
}

// Removes one populated key.
static bool remove_operation(void *context) {
    struct HashTableContext *hash_table_context = context;

    if (hash_table_context->next_index == ITEM_COUNT || !hash_table_remove(
            hash_table_context->table,
            &hash_table_context->keys[hash_table_context->next_index],
            &hash_table_context->out_value
        )) {
        return false;
    }

    hash_table_context->next_index++;
    return true;
}

// Confirms insertion populated one entry for every completed operation.
static bool set_verify(void *context) {
    struct HashTableContext *hash_table_context = context;

    return hash_table_size(hash_table_context->table) == hash_table_context->next_index;
}

// Confirms every requested lookup or membership check completed.
static bool lookup_verify(void *context) {
    struct HashTableContext *hash_table_context = context;

    return hash_table_context->next_index > 0U &&
        hash_table_context->next_index <= ITEM_COUNT;
}

// Confirms removal changed the size by one entry per completed operation.
static bool remove_verify(void *context) {
    struct HashTableContext *hash_table_context = context;

    return hash_table_context->next_index > 0U &&
        hash_table_context->next_index <= ITEM_COUNT &&
        hash_table_size(hash_table_context->table) ==
            ITEM_COUNT - hash_table_context->next_index;
}

// Releases the table created for one benchmark batch.
static void hash_table_teardown(void *context) {
    struct HashTableContext *hash_table_context = context;

    hash_table_destroy(hash_table_context->table);
    hash_table_context->table = NULL;
}

// Runs and prints one configured hash-table benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    struct HashTableContext hash_table_context = { NULL, { { 0 } }, { 0 }, 0U, NULL };
    BenchmarkConfig config = {
        .name = "Hash table set",
        .warmup_iterations = ITEM_COUNT,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = set_setup,
        .operation = set_operation,
        .verify = set_verify,
        .teardown = hash_table_teardown,
        .context = &hash_table_context
    };

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        hash_table_context.keys[index].id = (int)index;
        hash_table_context.values[index] = (int)(index * 2U);
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Hash table get";
    config.setup = populated_setup;
    config.operation = get_operation;
    config.verify = lookup_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Hash table contains";
    config.operation = contains_operation;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Hash table remove";
    config.operation = remove_operation;
    config.verify = remove_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
