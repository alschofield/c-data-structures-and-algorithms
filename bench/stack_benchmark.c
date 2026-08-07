// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public stack API.
#include "../src/stack/stack.h"

// Uses the default sample size unless Make supplies an experiment-specific size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 10000U
#endif

// Defines the number of stack operations in each timed sample.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Stores state shared by stack push and pop benchmarks.
struct StackContext {
    // Owns the stack created before each sample.
    Stack *stack;
    // Stores caller-owned values used by stack operations.
    int values[ITEM_COUNT];
    // Tracks how many values are currently pushed.
    size_t count;
    // Receives pointers returned by pop operations.
    void *out_item;
};

// Creates an empty stack for push measurement.
static bool push_setup(void *context) {
    struct StackContext *stack_context = context;

    stack_context->stack = stack_create();
    stack_context->count = 0U;
    return stack_context->stack != NULL;
}

// Pushes one prebuilt value.
static bool push_operation(void *context) {
    struct StackContext *stack_context = context;

    if (stack_context->count == ITEM_COUNT) {
        return false;
    }

    if (!stack_push(stack_context->stack, &stack_context->values[stack_context->count])) {
        return false;
    }

    stack_context->count++;
    return true;
}

// Confirms every successful push is represented in the stack size.
static bool push_verify(void *context) {
    struct StackContext *stack_context = context;

    return stack_size(stack_context->stack) == stack_context->count;
}

// Creates a populated stack before pop measurement.
static bool pop_setup(void *context) {
    struct StackContext *stack_context = context;

    stack_context->stack = stack_create();
    if (stack_context->stack == NULL) {
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!stack_push(stack_context->stack, &stack_context->values[index])) {
            stack_destroy(stack_context->stack);
            stack_context->stack = NULL;
            return false;
        }
    }

    stack_context->count = ITEM_COUNT;
    return true;
}

// Pops one value from the prepared stack.
static bool pop_operation(void *context) {
    struct StackContext *stack_context = context;

    if (stack_context->count == 0U) {
        return false;
    }

    if (!stack_pop(stack_context->stack, &stack_context->out_item)) {
        return false;
    }

    stack_context->count--;
    return true;
}

// Confirms the remaining size tracks successful pops.
static bool pop_verify(void *context) {
    struct StackContext *stack_context = context;

    return stack_size(stack_context->stack) == stack_context->count;
}

// Releases the stack created for one benchmark batch.
static void stack_teardown(void *context) {
    struct StackContext *stack_context = context;

    stack_destroy(stack_context->stack);
    stack_context->stack = NULL;
}

// Runs and prints one configured stack benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    struct StackContext stack_context = { NULL, { 0 }, 0U, NULL };
    BenchmarkConfig config = {
        .name = "Stack push",
        .warmup_iterations = 1000U,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = push_setup,
        .operation = push_operation,
        .verify = push_verify,
        .teardown = stack_teardown,
        .context = &stack_context
    };

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        stack_context.values[index] = (int)index;
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Stack pop";
    config.setup = pop_setup;
    config.operation = pop_operation;
    config.verify = pop_verify;

    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
