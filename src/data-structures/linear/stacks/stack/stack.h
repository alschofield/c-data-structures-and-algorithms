// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Names the opaque LIFO stack.
typedef struct Stack Stack;

// Allocates an empty stack.
Stack *stack_create(void);
// Frees stack storage without freeing caller-owned items.
void stack_destroy(Stack *stack);
// Pushes one item onto the stack top.
bool stack_push(Stack *stack, void *item);
// Removes and returns the top item.
bool stack_pop(Stack *stack, void **out_item);
// Returns the top item without removing it.
bool stack_peek(const Stack *stack, void **out_item);
// Returns the number of stored items.
size_t stack_size(const Stack *stack);
// Reports whether the stack contains no items.
bool stack_is_empty(const Stack *stack);
