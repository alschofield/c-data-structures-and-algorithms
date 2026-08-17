// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public dynamic-array API.
#include "../src/data-structures/linear/arrays/dynamic-array/dynamic_array.h"

// Uses the default sample size unless Make supplies an experiment-specific size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 10000U
#endif

// Defines the number of append insertions in each timed sample.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Stores state for amortized dynamic-array append insertion.
struct DynamicArrayAppendContext {
    // Owns the dynamic array created before each sample.
    DynamicArray *array;
    // Stores caller-owned values inserted into the array.
    int values[ITEM_COUNT];
    // Tracks which value will be inserted next.
    size_t next_index;
};

// Creates an empty dynamic array before each batch.
static bool append_setup(void *context) {
    // Converts the generic callback context to its concrete type.
    struct DynamicArrayAppendContext *append_context = context;

    // Creates the array outside the timed operation batch.
    append_context->array = dynamic_array_create();
    // Resets insertion to the first prebuilt value.
    append_context->next_index = 0U;
    // Reports whether creation succeeded.
    return append_context->array != NULL;
}

// Appends one prebuilt value through the general indexed-insert API.
static bool append_operation(void *context) {
    // Converts the generic callback context to its concrete type.
    struct DynamicArrayAppendContext *append_context = context;

    // Rejects operation calls beyond the configured sample size.
    if (append_context->next_index == ITEM_COUNT) {
        return false;
    }

    // Appends the next value at the valid insertion position equal to size.
    if (!dynamic_array_insert(
            append_context->array,
            dynamic_array_size(append_context->array),
            &append_context->values[append_context->next_index]
        )) {
        return false;
    }

    // Advances to the next caller-owned value.
    append_context->next_index++;
    // Reports successful insertion.
    return true;
}

// Confirms every requested insertion completed correctly.
static bool append_verify(void *context) {
    // Converts the generic callback context to its concrete type.
    struct DynamicArrayAppendContext *append_context = context;

    // Confirms the logical size and consumed-value count agree.
    return dynamic_array_size(append_context->array) == append_context->next_index;
}

// Releases the array created for a batch.
static void append_teardown(void *context) {
    // Converts the generic callback context to its concrete type.
    struct DynamicArrayAppendContext *append_context = context;

    // Destroys the array without freeing caller-owned values.
    dynamic_array_destroy(append_context->array);
    // Clears the destroyed pointer before the next lifecycle.
    append_context->array = NULL;
}

int main(void) {
    // Initializes benchmark state.
    struct DynamicArrayAppendContext append_context = { NULL, { 0 }, 0U };
    // Configures 21 timed append samples after one 1,000-operation warmup.
    BenchmarkConfig config = {
        .name = "Dynamic array append insertion",
        .warmup_iterations = 1000U,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = append_setup,
        .operation = append_operation,
        .verify = append_verify,
        .teardown = append_teardown,
        .context = &append_context
    };
    // Holds normalized append timing results.
    BenchmarkResult result;

    // Initializes every caller-owned integer once before timing begins.
    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        append_context.values[index] = (int)index;
    }

    // Runs the benchmark and reports failure through the process exit code.
    if (!benchmark_run(&config, &result)) {
        return 1;
    }

    // Prints the normalized timing summary.
    benchmark_print_result(&result);
    // Reports successful benchmark completion.
    return 0;
}
