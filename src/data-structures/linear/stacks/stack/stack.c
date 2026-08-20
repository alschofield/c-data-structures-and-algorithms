// Provides the public Stack type and function declarations.
#include "stack.h"
// Declares malloc, realloc, and free.
#include <stdlib.h>
// Declares SIZE_MAX.
#include <stdint.h>

// Defines the fields hidden from callers of the public API.
struct Stack {
    // Counts the item pointers currently stored.
    size_t size;
    // Counts the item-pointer slots currently allocated.
    size_t capacity;
    // Points to the dynamically allocated item-pointer array.
    void **items;
};

// Allocates and initializes an empty stack.
Stack* stack_create(void) {
    // Allocates memory for the Stack structure.
    Stack *stack = malloc(sizeof(Stack));
    // Checks whether allocation failed.
    if (stack == NULL) {
        // Reports allocation failure to the caller.
        return NULL;
    }

    // Initializes the item-buffer pointer as a null pointer.
    stack->items = NULL;
    // Initializes the logical item count as zero.
    stack->size = 0U;
    // Initializes capacity because no item slots are allocated yet.
    stack->capacity = 0U;
    // Returns the initialized stack.
    return stack;
}

// Removes the top item and returns it through out_item.
bool stack_pop(Stack *stack, void **out_item) {
    // Rejects a missing stack pointer.
    if (stack == NULL) {
        // Reports failure without changing an output pointer.
        return false;
    }

    // Rejects removal from an empty stack.
    if (stack->size == 0) {
        // Reports that no item is available.
        return false;
    }

    // Rejects a missing output location.
    if (out_item == NULL) {
        // Reports failure without removing an item.
        return false;
    }

    // Copies the top pointer into the caller's output variable.
    *out_item = stack->items[stack->size - 1U];
    // Clears the now-unused pointer slot.
    stack->items[stack->size - 1U] = NULL;
    // Decreases the logical item count.
    stack->size--;
    // Reports successful removal.
    return true;
}

// Adds an item pointer to the top of the stack.
bool stack_push(Stack *stack, void *item) {
    // Rejects a missing stack pointer.
    if (stack == NULL) {
        // Reports failure because there is no stack to modify.
        return false;
    }

    // Grows the item buffer only when every allocated slot is occupied.
    if (stack->size == stack->capacity) {
        // Rejects a capacity that cannot double without overflowing size_t.
        if (stack->capacity > SIZE_MAX / 2U) {
            return false;
        }
        // Uses one slot for the first allocation and doubles later allocations.
        size_t new_capacity = stack->capacity == 0U ? 1U : stack->capacity * 2U;
        // Rejects a pointer-slot count whose byte allocation would overflow.
        if (new_capacity > SIZE_MAX / sizeof(*stack->items)) {
            return false;
        }

        // Requests storage for the new total number of pointer slots.
        void **temp = realloc(stack->items, new_capacity * sizeof(*stack->items));
        // Checks whether the resize failed.
        if (temp == NULL) {
            // Preserves the old allocation and reports failure.
            return false;
        }

        // Stores the possibly relocated item-buffer address.
        stack->items = temp;
        // Records how many item-pointer slots are now available.
        stack->capacity = new_capacity;
    }

    // Writes the new pointer into the first unused slot.
    stack->items[stack->size] = item;
    // Increases the logical item count.
    stack->size++;
    // Reports successful insertion.
    return true;
}

// Frees the allocations owned by the stack.
void stack_destroy(Stack *stack) {
    // Makes destruction of a null stack a safe no-op.
    if (stack == NULL) {
        // Returns because there is no allocation to release.
        return;
    }

    // Frees the item-pointer array.
    free(stack->items);
    // Frees the Stack structure.
    free(stack);
    // Ends the void function.
    return;
}

// Copies the top item pointer without removing it.
bool stack_peek(const Stack *stack, void **out_item) {
    // Rejects a missing stack pointer.
    if (stack == NULL) {
        // Reports failure because there is no stack to inspect.
        return false;
    }

    // Rejects inspection of an empty stack.
    if (stack->size == 0) {
        // Reports that no top item exists.
        return false;
    }

    // Rejects a missing output location.
    if (out_item == NULL) {
        // Reports failure without changing stack state.
        return false;
    }

    // Copies the top pointer into the caller's output variable.
    *out_item = stack->items[stack->size - 1U];
    // Reports successful inspection without changing size.
    return true;
}

// Returns the number of item pointers stored.
size_t stack_size(const Stack *stack) {
    // Treats a null stack as containing zero items.
    if (stack == NULL) {
        // Returns the empty-stack count.
        return 0U;
    }

    // Returns the current logical item count.
    return stack->size;
}

// Reports whether a stack contains no item pointers.
bool stack_is_empty(const Stack *stack) {
    // Treats a null stack as empty under this API contract.
    if (stack == NULL) {
        // Reports an empty stack.
        return true;
    }

    // Compares the logical item count with zero.
    return stack->size == 0;
}
