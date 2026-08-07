// Defines the reusable in-process benchmark API.
#ifndef BENCHMARK_H
#define BENCHMARK_H

// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Runs one setup, operation, or verification callback.
typedef bool (*BenchmarkCallback)(void *context);
// Cleans up state created by a benchmark callback.
typedef void (*BenchmarkTeardown)(void *context);

// Configures one repeated-operation benchmark.
typedef struct BenchmarkConfig {
    // Labels benchmark output.
    const char *name;
    // Runs before samples to warm instruction and data caches.
    size_t warmup_iterations;
    // Counts independently timed operation batches.
    size_t sample_count;
    // Counts operation calls in each timed batch.
    size_t iterations_per_sample;
    // Prepares fresh state before each warmup or timed batch.
    BenchmarkCallback setup;
    // Performs one operation inside the timed batch.
    BenchmarkCallback operation;
    // Checks the completed batch after timing stops.
    BenchmarkCallback verify;
    // Releases state after verification or a failed batch.
    BenchmarkTeardown teardown;
    // Passes caller-owned state to every callback.
    void *context;
} BenchmarkConfig;

// Reports normalized timing results for a completed benchmark.
typedef struct BenchmarkResult {
    // Copies the display name from BenchmarkConfig.
    const char *name;
    // Records how many timed batches completed.
    size_t sample_count;
    // Records operations performed within each batch.
    size_t iterations_per_sample;
    // Reports the fastest observed nanoseconds per operation.
    double min_nanoseconds_per_operation;
    // Reports the median observed nanoseconds per operation.
    double median_nanoseconds_per_operation;
    // Reports the slowest observed nanoseconds per operation.
    double max_nanoseconds_per_operation;
} BenchmarkResult;

// Runs warmups and timed samples, returning false on callback or timer failure.
bool benchmark_run(const BenchmarkConfig *config, BenchmarkResult *out_result);
// Prints the normalized timing result to standard output.
void benchmark_print_result(const BenchmarkResult *result);

#endif
