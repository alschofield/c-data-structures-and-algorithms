// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public counting-sort API.
#include "../src/algorithms/sorting/non-comparison/counting-sort/counting_sort.h"

// Uses a substantial integer-sort input unless Make supplies a custom size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 10000U
#endif

// Defines the number of integer keys sorted by each timed operation.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Stores state shared by counting-sort benchmark samples.
struct SortContext {
    // Holds the input rebuilt before every timed sort.
    uint32_t items[ITEM_COUNT];
    // Holds the fixed generated values copied into items before each sample.
    uint32_t source[ITEM_COUNT];
    // Declares the valid key range for the current benchmark series.
    uint32_t key_limit;
};

// Rebuilds generated input outside the timed operation.
static bool setup(void *context) {
    struct SortContext *sort_context = context;

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        sort_context->items[index] = sort_context->source[index];
    }

    return true;
}

// Sorts one whole integer array as the timed operation.
static bool sort_operation(void *context) {
    struct SortContext *sort_context = context;

    return counting_sort(sort_context->items, ITEM_COUNT, sort_context->key_limit);
}

// Confirms the timed sort left every key in ascending order.
static bool sorted_verify(void *context) {
    struct SortContext *sort_context = context;

    for (size_t index = 1U; index < ITEM_COUNT; index++) {
        if (sort_context->items[index - 1U] > sort_context->items[index]) {
            return false;
        }
    }

    return true;
}

// Fills source with deterministic keys inside the current key range.
static void fill_source(struct SortContext *sort_context) {
    uint32_t state = 0x9E3779B9U;

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        state = state * 1664525U + 1013904223U;
        sort_context->source[index] = state % sort_context->key_limit;
    }
}

// Runs and prints one configured counting-sort benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct SortContext sort_context;
    BenchmarkConfig config = {
        .name = "Counting sort 256-key range (one whole sort)",
        .warmup_iterations = 1U,
        .sample_count = 21U,
        .iterations_per_sample = 1U,
        .setup = setup,
        .operation = sort_operation,
        .verify = sorted_verify,
        .teardown = NULL,
        .context = &sort_context
    };

    sort_context.key_limit = 256U;
    fill_source(&sort_context);
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Counting sort 65536-key range (one whole sort)";
    sort_context.key_limit = 65536U;
    fill_source(&sort_context);
    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
