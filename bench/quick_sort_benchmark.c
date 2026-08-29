// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public quicksort API.
#include "../src/algorithms/sorting/comparison/quick-sort/quick_sort.h"

// Uses a substantial comparison-sort input unless Make supplies a custom size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 10000U
#endif

// Defines the number of pointers sorted by each timed operation.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Compares benchmark values by their integer contents.
static int compare_ints(const void *left, const void *right) {
    const int *left_value = left;
    const int *right_value = right;

    return (*left_value > *right_value) - (*left_value < *right_value);
}

// Stores state shared by quicksort benchmark samples.
struct SortContext {
    // Stores the caller-owned values referenced by the sortable pointer array.
    int values[ITEM_COUNT];
    // Holds the input arrangement rebuilt before every sample.
    void *items[ITEM_COUNT];
    // Stores the fixed shuffled arrangement used for shuffled samples.
    void *shuffled[ITEM_COUNT];
};

// Rebuilds the fixed shuffled input outside the timed operation.
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
        sort_context->items[index] = &sort_context->values[index];
    }

    return true;
}

// Rebuilds descending input outside the timed operation.
static bool reverse_setup(void *context) {
    struct SortContext *sort_context = context;

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        sort_context->items[index] = &sort_context->values[ITEM_COUNT - 1U - index];
    }

    return true;
}

// Sorts the whole rebuilt pointer array as one timed operation.
static bool sort_operation(void *context) {
    struct SortContext *sort_context = context;

    return quick_sort(sort_context->items, ITEM_COUNT, compare_ints);
}

// Confirms the timed sort left the pointer array in ascending order.
static bool sorted_verify(void *context) {
    struct SortContext *sort_context = context;

    for (size_t index = 1U; index < ITEM_COUNT; index++) {
        if (compare_ints(
                sort_context->items[index - 1U], sort_context->items[index]
            ) > 0) {
            return false;
        }
    }

    return true;
}

// Runs and prints one configured quicksort benchmark.
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
        .name = "Quick sort shuffled input (one whole sort)",
        .warmup_iterations = 1U,
        .sample_count = 21U,
        .iterations_per_sample = 1U,
        .setup = shuffled_setup,
        .operation = sort_operation,
        .verify = sorted_verify,
        .teardown = NULL,
        .context = &sort_context
    };

    // Initializes stable integer values and a pointer permutation to sort.
    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        sort_context.values[index] = (int)index;
        sort_context.shuffled[index] = &sort_context.values[index];
    }

    // Uses a deterministic Fisher-Yates shuffle for comparable repeated samples.
    unsigned long long state = 0x2545F4914F6CDD1DULL;
    for (size_t index = ITEM_COUNT - 1U; index > 0U; index--) {
        state = state * 6364136223846793005ULL + 1442695040888963407ULL;
        size_t swap_index = (size_t)(state % (unsigned long long)(index + 1U));
        void *held = sort_context.shuffled[index];
        sort_context.shuffled[index] = sort_context.shuffled[swap_index];
        sort_context.shuffled[swap_index] = held;
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Quick sort sorted input";
    config.setup = sorted_setup;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Quick sort reverse input";
    config.setup = reverse_setup;
    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
