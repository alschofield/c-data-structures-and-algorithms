// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public radix-sort API.
#include "../src/algorithms/sorting/non-comparison/radix-sort/radix_sort.h"

// Uses a substantial fixed-width integer input unless Make supplies a custom size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 10000U
#endif

// Defines the number of integer keys sorted by each timed operation.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Stores state shared by radix-sort benchmark samples.
struct SortContext {
    // Holds the input rebuilt before every timed sort.
    uint32_t items[ITEM_COUNT];
    // Holds the deterministic full-width generated input.
    uint32_t shuffled[ITEM_COUNT];
};

// Rebuilds the generated full-width input outside the timed operation.
static bool shuffled_setup(void *context) {
    struct SortContext *sort_context = context;

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        sort_context->items[index] = sort_context->shuffled[index];
    }

    return true;
}

// Rebuilds ascending input outside the timed operation.
static bool sorted_setup(void *context) {
    struct SortContext *sort_context = context;

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        sort_context->items[index] = (uint32_t)index;
    }

    return true;
}

// Rebuilds descending input outside the timed operation.
static bool reverse_setup(void *context) {
    struct SortContext *sort_context = context;

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        sort_context->items[index] = (uint32_t)(ITEM_COUNT - 1U - index);
    }

    return true;
}

// Sorts one whole fixed-width integer array as the timed operation.
static bool sort_operation(void *context) {
    struct SortContext *sort_context = context;

    return radix_sort(sort_context->items, ITEM_COUNT);
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

// Runs and prints one configured radix-sort benchmark.
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
        .name = "Radix sort shuffled input (one whole sort)",
        .warmup_iterations = 1U,
        .sample_count = 21U,
        .iterations_per_sample = 1U,
        .setup = shuffled_setup,
        .operation = sort_operation,
        .verify = sorted_verify,
        .teardown = NULL,
        .context = &sort_context
    };

    // Generates deterministic values spanning the full 32-bit key width.
    uint32_t state = 0x9E3779B9U;
    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        state = state * 1664525U + 1013904223U;
        sort_context.shuffled[index] = state;
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Radix sort sorted input";
    config.setup = sorted_setup;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Radix sort reverse input";
    config.setup = reverse_setup;
    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
