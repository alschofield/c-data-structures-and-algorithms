// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public binary heap API.
#include "../src/data-structures/trees/heaps/binary-heap/binary_heap.h"

// Uses the standard benchmark size unless Make supplies an experiment size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 10000U
#endif

// Defines the number of item pointers used in each benchmark batch.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Compares benchmark integers in ascending order, creating a min-heap.
static int compare_ints(const void *left, const void *right) {
    const int *left_value = left;
    const int *right_value = right;

    return (*left_value > *right_value) - (*left_value < *right_value);
}

// Stores state shared by binary-heap operation benchmarks.
struct HeapContext {
    // Owns the heap created before each benchmark batch.
    BinaryHeap *heap;
    // Stores caller-owned values inserted into the heap.
    int values[ITEM_COUNT];
    // Stores a deterministic shuffled insertion order.
    size_t order[ITEM_COUNT];
    // Tracks the next item to push, pop, or check.
    size_t next_index;
    // Receives pointers returned by pop and peek.
    void *out_item;
};

// Fills order with a deterministic Fisher-Yates permutation.
static void shuffle_order(struct HeapContext *heap_context) {
    unsigned long long state = 0x2545F4914F6CDD1DULL;

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        heap_context->order[index] = index;
    }
    for (size_t index = ITEM_COUNT - 1U; index > 0U; index--) {
        state = state * 6364136223846793005ULL + 1442695040888963407ULL;
        size_t swap_index = (size_t)(state % (unsigned long long)(index + 1U));
        size_t held = heap_context->order[index];
        heap_context->order[index] = heap_context->order[swap_index];
        heap_context->order[swap_index] = held;
    }
}

// Creates an empty heap before a push sample.
static bool empty_setup(void *context) {
    struct HeapContext *heap_context = context;

    heap_context->heap = binary_heap_create(compare_ints);
    heap_context->next_index = 0U;
    return heap_context->heap != NULL;
}

// Creates and fills a heap outside the timed region for pop and peek samples.
static bool populated_setup(void *context) {
    struct HeapContext *heap_context = context;

    heap_context->heap = binary_heap_create(compare_ints);
    if (heap_context->heap == NULL) {
        return false;
    }
    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!binary_heap_push(
                heap_context->heap,
                &heap_context->values[heap_context->order[index]]
            )) {
            binary_heap_destroy(heap_context->heap);
            heap_context->heap = NULL;
            return false;
        }
    }

    heap_context->next_index = 0U;
    return true;
}

// Pushes one shuffled value into an empty heap.
static bool push_operation(void *context) {
    struct HeapContext *heap_context = context;

    if (heap_context->next_index == ITEM_COUNT || !binary_heap_push(
            heap_context->heap,
            &heap_context->values[heap_context->order[heap_context->next_index]]
        )) {
        return false;
    }

    heap_context->next_index++;
    return true;
}

// Removes one minimum item from a populated heap.
static bool pop_operation(void *context) {
    struct HeapContext *heap_context = context;

    if (heap_context->next_index == ITEM_COUNT || !binary_heap_pop(
            heap_context->heap,
            &heap_context->out_item
        )) {
        return false;
    }

    heap_context->next_index++;
    return true;
}

// Reads the root of a populated heap without changing its size.
static bool peek_operation(void *context) {
    struct HeapContext *heap_context = context;

    return binary_heap_peek(heap_context->heap, &heap_context->out_item);
}

// Confirms every push inserted exactly one item.
static bool push_verify(void *context) {
    struct HeapContext *heap_context = context;

    return heap_context->next_index == ITEM_COUNT &&
        binary_heap_size(heap_context->heap) == ITEM_COUNT;
}

// Confirms every pop completed and left the heap empty.
static bool pop_verify(void *context) {
    struct HeapContext *heap_context = context;

    return heap_context->next_index == ITEM_COUNT &&
        binary_heap_is_empty(heap_context->heap);
}

// Confirms repeated peeks preserve every populated item.
static bool peek_verify(void *context) {
    struct HeapContext *heap_context = context;

    return heap_context->out_item == &heap_context->values[0] &&
        binary_heap_size(heap_context->heap) == ITEM_COUNT;
}

// Releases the heap created for one benchmark batch.
static void heap_teardown(void *context) {
    struct HeapContext *heap_context = context;

    binary_heap_destroy(heap_context->heap);
    heap_context->heap = NULL;
}

// Runs and prints one configured binary-heap benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct HeapContext heap_context;
    BenchmarkConfig config = {
        .name = "Binary heap push",
        .warmup_iterations = ITEM_COUNT,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = empty_setup,
        .operation = push_operation,
        .verify = push_verify,
        .teardown = heap_teardown,
        .context = &heap_context
    };

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        heap_context.values[index] = (int)index;
    }
    shuffle_order(&heap_context);

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Binary heap pop";
    config.setup = populated_setup;
    config.operation = pop_operation;
    config.verify = pop_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Binary heap peek";
    config.operation = peek_operation;
    config.verify = peek_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
