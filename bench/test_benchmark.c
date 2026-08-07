// Provides the reusable benchmark API.
#include "benchmark.h"

// Declares assert.
#include <assert.h>

// Stores state for a predictable counter benchmark.
struct CounterContext {
    // Counts successful operation callbacks in the current batch.
    size_t count;
    // Counts setup lifecycles.
    size_t setup_count;
    // Counts teardown lifecycles.
    size_t teardown_count;
};

// Resets the counter before a warmup or timed sample.
static bool counter_setup(void *context) {
    // Converts the generic callback context to its concrete type.
    struct CounterContext *counter = context;
    // Resets the current batch count.
    counter->count = 0U;
    // Records that setup ran.
    counter->setup_count++;
    // Reports successful setup.
    return true;
}

// Performs one cheap operation for lifecycle testing.
static bool counter_operation(void *context) {
    // Converts the generic callback context to its concrete type.
    struct CounterContext *counter = context;
    // Increments the current batch count.
    counter->count++;
    // Reports successful operation completion.
    return true;
}

// Confirms that at least one operation completed in the batch.
static bool counter_verify(void *context) {
    // Converts the generic callback context to its concrete type.
    struct CounterContext *counter = context;
    // Reports whether the batch ran one or more operations.
    return counter->count > 0U;
}

// Records cleanup after every prepared batch.
static void counter_teardown(void *context) {
    // Converts the generic callback context to its concrete type.
    struct CounterContext *counter = context;
    // Records that teardown ran.
    counter->teardown_count++;
}

int main(void) {
    // Initializes counter benchmark state.
    struct CounterContext counter = { 0U, 0U, 0U };
    // Configures one warmup plus five timed samples of 100 operations.
    BenchmarkConfig config = {
        .name = "Counter increment",
        .warmup_iterations = 10U,
        .sample_count = 5U,
        .iterations_per_sample = 100U,
        .setup = counter_setup,
        .operation = counter_operation,
        .verify = counter_verify,
        .teardown = counter_teardown,
        .context = &counter
    };
    // Holds the normalized timing result.
    BenchmarkResult result;

    // Verifies that the complete benchmark lifecycle succeeds.
    assert(benchmark_run(&config, &result));
    // Verifies the reported sample count.
    assert(result.sample_count == config.sample_count);
    // Verifies the reported operation count per sample.
    assert(result.iterations_per_sample == config.iterations_per_sample);
    // Verifies one warmup and five timed setup lifecycles.
    assert(counter.setup_count == config.sample_count + 1U);
    // Verifies every setup lifecycle received one teardown.
    assert(counter.teardown_count == counter.setup_count);
    // Verifies the final sample completed every requested operation.
    assert(counter.count == config.iterations_per_sample);
    // Verifies sorted result ordering.
    assert(result.min_nanoseconds_per_operation <= result.median_nanoseconds_per_operation);
    // Verifies sorted result ordering.
    assert(result.median_nanoseconds_per_operation <= result.max_nanoseconds_per_operation);

    // Rejects a configuration without an operation callback.
    config.operation = NULL;
    assert(!benchmark_run(&config, &result));
    // Reports successful benchmark-harness verification.
    return 0;
}
