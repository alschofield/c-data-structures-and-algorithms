#include "stack.h"

#include <assert.h>
#include <stddef.h>

struct Record {
    int id;
};

static void test_empty_stack(void) {
    Stack *stack = stack_create();
    int sentinel = 0;
    void *out = &sentinel;

    assert(stack != NULL);
    assert(stack_size(stack) == 0U);
    assert(stack_is_empty(stack));
    assert(!stack_pop(stack, &out));
    assert(out == &sentinel);
    assert(!stack_peek(stack, &out));
    assert(out == &sentinel);
    assert(!stack_pop(stack, NULL));
    assert(!stack_peek(stack, NULL));

    stack_destroy(stack);
}

static void test_lifo_and_generic_values(void) {
    Stack *stack = stack_create();
    int number = 42;
    struct Record record = { .id = 7 };
    const char label[] = "stack";
    void *out = NULL;

    assert(stack_push(stack, &number));
    assert(stack_push(stack, &record));
    assert(stack_push(stack, (void *)label));
    assert(stack_push(stack, NULL));
    assert(stack_size(stack) == 4U);

    assert(stack_peek(stack, &out));
    assert(out == NULL);
    assert(stack_size(stack) == 4U);
    assert(stack_pop(stack, &out));
    assert(out == NULL);
    assert(stack_pop(stack, &out));
    assert(out == label);
    assert(stack_pop(stack, &out));
    assert(out == &record);
    assert(stack_pop(stack, &out));
    assert(out == &number);
    assert(stack_is_empty(stack));

    stack_destroy(stack);
}

static void test_growth_and_reuse(void) {
    enum { ITEM_COUNT = 1024 };
    Stack *stack = stack_create();
    int values[ITEM_COUNT];
    void *out = NULL;

    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        values[index] = (int)index;
        assert(stack_push(stack, &values[index]));
    }
    assert(stack_size(stack) == ITEM_COUNT);

    for (size_t index = ITEM_COUNT; index > 0U; --index) {
        assert(stack_pop(stack, &out));
        assert(out == &values[index - 1U]);
    }
    assert(stack_is_empty(stack));
    assert(stack_push(stack, &values[0]));
    assert(stack_pop(stack, &out));
    assert(out == &values[0]);

    stack_destroy(stack);
}

static void test_null_container(void) {
    int value = 1;
    void *out = &value;

    assert(!stack_push(NULL, &value));
    assert(!stack_pop(NULL, &out));
    assert(out == &value);
    assert(!stack_peek(NULL, &out));
    assert(out == &value);
    assert(stack_size(NULL) == 0U);
    assert(stack_is_empty(NULL));
    stack_destroy(NULL);
}

int main(void) {
    test_empty_stack();
    test_lifo_and_generic_values();
    test_growth_and_reuse();
    test_null_container();
    return 0;
}
