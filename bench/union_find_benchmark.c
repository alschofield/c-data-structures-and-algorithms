// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public union-find API.
#include "../src/data-structures/graphs/disjoint-sets/union-find/union_find.h"

// Uses a substantial element count unless Make supplies an experiment size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 10000U
#endif

// Defines the number of effective merges and queries per benchmark sample.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Stores state shared by union-find operation benchmarks.
struct UnionFindContext {
    // Owns the union-find created before each benchmark sample.
    UnionFind *set;
    // Tracks the next non-root element used by a timed operation.
    size_t next_index;
    // Receives representative lookup results.
    size_t representative;
    // Receives connectivity query results.
    bool connected;
    // Receives whether a union changed the forest.
    bool merged;
};

// Creates one singleton set for the root plus every timed operation.
static bool empty_setup(void *context) {
    struct UnionFindContext *union_context = context;

    union_context->set = union_find_create(ITEM_COUNT + 1U);
    union_context->next_index = 0U;
    return union_context->set != NULL;
}

// Creates a populated star-shaped forest outside timed lookup samples.
static bool populated_setup(void *context) {
    struct UnionFindContext *union_context = context;

    if (!empty_setup(context)) {
        return false;
    }

    for (size_t index = 1U; index <= ITEM_COUNT; index++) {
        if (!union_find_union(union_context->set, 0U, index, &union_context->merged)) {
            union_find_destroy(union_context->set);
            union_context->set = NULL;
            return false;
        }
    }

    union_context->next_index = 0U;
    return true;
}

// Merges one singleton element into the root set.
static bool union_operation(void *context) {
    struct UnionFindContext *union_context = context;

    if (union_context->next_index == ITEM_COUNT || !union_find_union(
            union_context->set,
            0U,
            union_context->next_index + 1U,
            &union_context->merged
        ) || !union_context->merged) {
        return false;
    }

    union_context->next_index++;
    return true;
}

// Finds the representative for one element in the populated forest.
static bool find_operation(void *context) {
    struct UnionFindContext *union_context = context;

    if (union_context->next_index == ITEM_COUNT || !union_find_find(
            union_context->set,
            union_context->next_index + 1U,
            &union_context->representative
        )) {
        return false;
    }

    union_context->next_index++;
    return true;
}

// Queries whether one element belongs to the populated root set.
static bool connected_operation(void *context) {
    struct UnionFindContext *union_context = context;

    if (union_context->next_index == ITEM_COUNT || !union_find_connected(
            union_context->set,
            0U,
            union_context->next_index + 1U,
            &union_context->connected
        ) || !union_context->connected) {
        return false;
    }

    union_context->next_index++;
    return true;
}

// Confirms every timed union reduced the forest to one set.
static bool union_verify(void *context) {
    struct UnionFindContext *union_context = context;

    return union_context->next_index == ITEM_COUNT &&
        union_find_set_count(union_context->set) == 1U;
}

// Confirms every timed find reached the populated root representative.
static bool find_verify(void *context) {
    struct UnionFindContext *union_context = context;

    return union_context->next_index == ITEM_COUNT &&
        union_context->representative == 0U;
}

// Confirms every timed connectivity query found one shared set.
static bool connected_verify(void *context) {
    struct UnionFindContext *union_context = context;

    return union_context->next_index == ITEM_COUNT && union_context->connected;
}

// Releases the union-find created for one benchmark sample.
static void union_find_teardown(void *context) {
    struct UnionFindContext *union_context = context;

    union_find_destroy(union_context->set);
    union_context->set = NULL;
}

// Runs and prints one configured union-find benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct UnionFindContext union_context;
    BenchmarkConfig config = {
        .name = "Union-find union",
        .warmup_iterations = ITEM_COUNT,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = empty_setup,
        .operation = union_operation,
        .verify = union_verify,
        .teardown = union_find_teardown,
        .context = &union_context
    };

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Union-find find";
    config.setup = populated_setup;
    config.operation = find_operation;
    config.verify = find_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Union-find connected";
    config.operation = connected_operation;
    config.verify = connected_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
