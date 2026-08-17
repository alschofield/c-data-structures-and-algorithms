// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public queue API.
#include "../src/data-structures/linear/queues/queue/queue.h"

// Uses the default sample size unless Make supplies an experiment-specific size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 10000U
#endif

// Defines the number of queue operations in each timed sample.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Stores state shared by queue enqueue and dequeue benchmarks.
struct QueueContext {
    // Owns the queue created before each sample.
    Queue *queue;
    // Stores caller-owned values used by queue operations.
    int values[ITEM_COUNT];
    // Tracks how many values are currently enqueued.
    size_t count;
    // Receives pointers returned by dequeue operations.
    void *out_item;
};

// Creates an empty queue for enqueue measurement.
static bool enqueue_setup(void *context) {
    struct QueueContext *queue_context = context;

    queue_context->queue = queue_create();
    queue_context->count = 0U;
    return queue_context->queue != NULL;
}

// Enqueues one prebuilt value.
static bool enqueue_operation(void *context) {
    struct QueueContext *queue_context = context;

    if (queue_context->count == ITEM_COUNT) {
        return false;
    }

    if (!queue_enqueue(queue_context->queue, &queue_context->values[queue_context->count])) {
        return false;
    }

    queue_context->count++;
    return true;
}

// Confirms every successful enqueue is represented in the queue size.
static bool enqueue_verify(void *context) {
    struct QueueContext *queue_context = context;

    return queue_size(queue_context->queue) == queue_context->count;
}

// Creates a populated queue before dequeue measurement.
static bool dequeue_setup(void *context) {
    struct QueueContext *queue_context = context;

    queue_context->queue = queue_create();
    if (queue_context->queue == NULL) {
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!queue_enqueue(queue_context->queue, &queue_context->values[index])) {
            queue_destroy(queue_context->queue);
            queue_context->queue = NULL;
            return false;
        }
    }

    queue_context->count = ITEM_COUNT;
    return true;
}

// Dequeues one value from the prepared queue.
static bool dequeue_operation(void *context) {
    struct QueueContext *queue_context = context;

    if (queue_context->count == 0U) {
        return false;
    }

    if (!queue_dequeue(queue_context->queue, &queue_context->out_item)) {
        return false;
    }

    queue_context->count--;
    return true;
}

// Confirms the remaining size tracks successful dequeues.
static bool dequeue_verify(void *context) {
    struct QueueContext *queue_context = context;

    return queue_size(queue_context->queue) == queue_context->count;
}

// Releases the queue created for one benchmark batch.
static void queue_teardown(void *context) {
    struct QueueContext *queue_context = context;

    queue_destroy(queue_context->queue);
    queue_context->queue = NULL;
}

// Runs and prints one configured queue benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    struct QueueContext queue_context = { NULL, { 0 }, 0U, NULL };
    BenchmarkConfig config = {
        .name = "Queue enqueue",
        .warmup_iterations = 1000U,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = enqueue_setup,
        .operation = enqueue_operation,
        .verify = enqueue_verify,
        .teardown = queue_teardown,
        .context = &queue_context
    };

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        queue_context.values[index] = (int)index;
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Queue dequeue";
    config.setup = dequeue_setup;
    config.operation = dequeue_operation;
    config.verify = dequeue_verify;

    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
