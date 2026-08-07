// Selects the POSIX clock_gettime declaration before including system headers.
#if !defined(_WIN32)
#define _POSIX_C_SOURCE 200809L
#endif

// Provides the benchmark API declarations.
#include "benchmark.h"

// Declares qsort, malloc, and free.
#include <stdlib.h>
// Declares printf.
#include <stdio.h>

// Uses Windows' high-resolution monotonic performance counter.
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// Uses POSIX's monotonic clock on non-Windows platforms.
#else
#include <time.h>
#endif

// Converts one high-resolution timestamp to seconds.
static bool benchmark_now_seconds(double *out_seconds) {
    // Rejects a missing output location.
    if (out_seconds == NULL) {
        // Reports that no timestamp can be returned.
        return false;
    }

    // Uses QueryPerformanceCounter on Windows.
#if defined(_WIN32)
    // Caches the fixed counter frequency after the first successful lookup.
    static LARGE_INTEGER frequency;
    // Records whether the cached frequency is ready.
    static bool frequency_ready = false;
    // Holds the current counter tick value.
    LARGE_INTEGER counter;

    // Loads the counter frequency once before taking measurements.
    if (!frequency_ready) {
        // Reports failure if Windows cannot provide a counter frequency.
        if (!QueryPerformanceFrequency(&frequency)) {
            return false;
        }

        // Marks the cached frequency as valid.
        frequency_ready = true;
    }

    // Reads the current high-resolution counter value.
    if (!QueryPerformanceCounter(&counter)) {
        // Reports failure if Windows cannot read the counter.
        return false;
    }

    // Converts counter ticks to seconds using floating-point division.
    *out_seconds = (double)counter.QuadPart / (double)frequency.QuadPart;
    // Reports successful timestamp retrieval.
    return true;
// Uses clock_gettime with a clock unaffected by wall-clock adjustments.
#else
    // Holds the current monotonic timestamp.
    struct timespec timestamp;

    // Reads the current monotonic time.
    if (clock_gettime(CLOCK_MONOTONIC, &timestamp) != 0) {
        // Reports failure if the operating system cannot read the clock.
        return false;
    }

    // Converts seconds and nanoseconds to one floating-point seconds value.
    *out_seconds = (double)timestamp.tv_sec + (double)timestamp.tv_nsec / 1000000000.0;
    // Reports successful timestamp retrieval.
    return true;
#endif
}

// Orders two doubles in ascending order for qsort.
static int compare_doubles(const void *left, const void *right) {
    // Reads the left sample value.
    double left_value = *(const double *)left;
    // Reads the right sample value.
    double right_value = *(const double *)right;

    // Places smaller samples before larger samples.
    if (left_value < right_value) {
        return -1;
    }

    // Places larger samples after smaller samples.
    if (left_value > right_value) {
        return 1;
    }

    // Treats equal samples as equal ordering values.
    return 0;
}

// Runs a setup-operation-verify-teardown lifecycle, optionally timing operations.
static bool benchmark_run_batch(
    const BenchmarkConfig *config,
    size_t iterations,
    bool measure,
    double *out_seconds
) {
    // Tracks whether every operation callback succeeds.
    bool succeeded = true;
    // Holds the timestamp before the operation batch.
    double start_seconds = 0.0;
    // Holds the timestamp after the operation batch.
    double end_seconds = 0.0;

    // Prepares fresh benchmark state when the caller supplied setup work.
    if (config->setup != NULL && !config->setup(config->context)) {
        // Reports setup failure before any timed work begins.
        return false;
    }

    // Starts timing immediately before the repeated operation calls.
    if (measure && !benchmark_now_seconds(&start_seconds)) {
        // Releases setup state before reporting timer failure.
        if (config->teardown != NULL) {
            config->teardown(config->context);
        }

        // Reports timer failure.
        return false;
    }

    // Repeats the operation callback for the requested batch size.
    for (size_t iteration = 0U; iteration < iterations; iteration++) {
        // Stops the batch if the benchmarked operation reports failure.
        if (!config->operation(config->context)) {
            succeeded = false;
            break;
        }
    }

    // Stops timing immediately after the repeated operation calls.
    if (measure && !benchmark_now_seconds(&end_seconds)) {
        // Marks the batch as failed when the ending timestamp is unavailable.
        succeeded = false;
    }

    // Verifies state only after timing has stopped.
    if (succeeded && config->verify != NULL && !config->verify(config->context)) {
        // Marks the batch as failed when verification rejects the final state.
        succeeded = false;
    }

    // Releases benchmark state after every prepared batch.
    if (config->teardown != NULL) {
        config->teardown(config->context);
    }

    // Reports callback or timer failure without returning a duration.
    if (!succeeded) {
        return false;
    }

    // Stores the measured operation-batch duration when requested.
    if (measure) {
        *out_seconds = end_seconds - start_seconds;
    }

    // Reports a successful batch lifecycle.
    return true;
}

// Runs warmups and timed samples, returning false on callback or timer failure.
bool benchmark_run(const BenchmarkConfig *config, BenchmarkResult *out_result) {
    // Holds one duration for each timed sample.
    double *samples;

    // Rejects missing configuration or result storage.
    if (config == NULL || out_result == NULL) {
        return false;
    }

    // Requires a named operation and at least one non-empty timed sample.
    if (config->name == NULL || config->operation == NULL || config->sample_count == 0U ||
        config->iterations_per_sample == 0U) {
        return false;
    }

    // Rejects a sample allocation whose byte count would overflow size_t.
    if (config->sample_count > (size_t)-1 / sizeof(*samples)) {
        return false;
    }

    // Runs one untimed warmup lifecycle when warmup work was requested.
    if (config->warmup_iterations > 0U &&
        !benchmark_run_batch(config, config->warmup_iterations, false, NULL)) {
        return false;
    }

    // Allocates storage for the timed batch durations outside benchmark timing.
    samples = malloc(config->sample_count * sizeof(*samples));
    // Reports allocation failure without changing the caller's result.
    if (samples == NULL) {
        return false;
    }

    // Runs and records each independent timed sample.
    for (size_t sample = 0U; sample < config->sample_count; sample++) {
        // Releases sample storage and reports failure when a batch fails.
        if (!benchmark_run_batch(
                config,
                config->iterations_per_sample,
                true,
                &samples[sample]
            )) {
            free(samples);
            return false;
        }
    }

    // Sorts raw batch durations so min, median, and max are deterministic.
    qsort(samples, config->sample_count, sizeof(*samples), compare_doubles);

    // Records the benchmark identity and execution shape.
    out_result->name = config->name;
    out_result->sample_count = config->sample_count;
    out_result->iterations_per_sample = config->iterations_per_sample;
    // Normalizes the fastest batch duration to nanoseconds per operation.
    out_result->min_nanoseconds_per_operation =
        samples[0] * 1000000000.0 / (double)config->iterations_per_sample;
    // Uses the middle sample for odd counts and averages both middle samples for even counts.
    if (config->sample_count % 2U == 0U) {
        out_result->median_nanoseconds_per_operation =
            (samples[config->sample_count / 2U - 1U] + samples[config->sample_count / 2U]) /
            2.0 * 1000000000.0 / (double)config->iterations_per_sample;
    } else {
        out_result->median_nanoseconds_per_operation =
            samples[config->sample_count / 2U] * 1000000000.0 /
            (double)config->iterations_per_sample;
    }
    // Normalizes the slowest batch duration to nanoseconds per operation.
    out_result->max_nanoseconds_per_operation =
        samples[config->sample_count - 1U] * 1000000000.0 /
        (double)config->iterations_per_sample;

    // Releases the temporary sample storage after producing the result.
    free(samples);
    // Reports successful benchmark completion.
    return true;
}

// Prints the normalized timing result to standard output.
void benchmark_print_result(const BenchmarkResult *result) {
    // Makes a missing result a safe no-op.
    if (result == NULL) {
        return;
    }

    // Prints the benchmark identity and sample shape.
    printf(
        "%s: %zu samples x %zu operations\n",
        result->name,
        result->sample_count,
        result->iterations_per_sample
    );
    // Prints normalized timing values to make different batch sizes comparable.
    printf(
        "  ns/op: median %.2f, min %.2f, max %.2f\n",
        result->median_nanoseconds_per_operation,
        result->min_nanoseconds_per_operation,
        result->max_nanoseconds_per_operation
    );
}
