// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public linear-search API.
#include "../src/algorithms/searching/linear-search/linear_search.h"
// Declares SIZE_MAX.
#include <stdint.h>

// Limits full-scan samples to a practical repeated benchmark runtime.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 2000U
#endif

// Defines the number of searchable item pointers per sample.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Stores state shared by linear-search benchmark samples.
struct SearchContext {
    // Stores caller-owned searchable integer values.
    int values[ITEM_COUNT];
    // Stores pointers to values in sequential search order.
    void *items[ITEM_COUNT];
    // Holds the key selected by the current benchmark setup.
    int key;
    // Receives the first matching index when a search succeeds.
    size_t out_index;
    // Records the expected result for post-sample verification.
    size_t expected_index;
    // Records whether the current setup expects a match.
    bool expect_found;
};

// Compares benchmark values by their integer contents.
static int compare_ints(const void *left, const void *right) {
    const int *left_value = left;
    const int *right_value = right;

    return (*left_value > *right_value) - (*left_value < *right_value);
}

// Sets the key and expected result for one search-position benchmark.
static bool set_search(struct SearchContext *search_context, size_t index,
                       bool expect_found) {
    search_context->out_index = SIZE_MAX;
    search_context->expect_found = expect_found;
    search_context->expected_index = index;
    search_context->key = expect_found ? search_context->values[index] : (int)ITEM_COUNT;
    return true;
}

// Prepares a key found at the first input position.
static bool first_setup(void *context) {
    return set_search(context, 0U, true);
}

// Prepares a key found near the middle input position.
static bool middle_setup(void *context) {
    return set_search(context, ITEM_COUNT / 2U, true);
}

// Prepares a key found at the final input position.
static bool last_setup(void *context) {
    return set_search(context, ITEM_COUNT - 1U, true);
}

// Prepares a key guaranteed absent from the generated input.
static bool missing_setup(void *context) {
    return set_search(context, 0U, false);
}

// Runs one search and confirms its found/not-found outcome.
static bool search_operation(void *context) {
    struct SearchContext *search_context = context;
    bool found = linear_search(
        search_context->items,
        ITEM_COUNT,
        &search_context->key,
        compare_ints,
        &search_context->out_index
    );

    return found == search_context->expect_found;
}

// Confirms the search returned the expected found index or preserved sentinel.
static bool search_verify(void *context) {
    struct SearchContext *search_context = context;

    if (search_context->expect_found) {
        return search_context->out_index == search_context->expected_index;
    }

    return search_context->out_index == SIZE_MAX;
}

// Runs and prints one configured linear-search benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct SearchContext search_context;
    BenchmarkConfig config = {
        .name = "Linear search first item",
        .warmup_iterations = ITEM_COUNT,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = first_setup,
        .operation = search_operation,
        .verify = search_verify,
        .teardown = NULL,
        .context = &search_context
    };

    // Initializes distinct ascending values with a guaranteed missing key above range.
    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        search_context.values[index] = (int)index;
        search_context.items[index] = &search_context.values[index];
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Linear search middle item";
    config.setup = middle_setup;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Linear search last item";
    config.setup = last_setup;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Linear search missing item";
    config.setup = missing_setup;
    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
