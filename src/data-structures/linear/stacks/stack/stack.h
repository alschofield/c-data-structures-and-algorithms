#include <stdbool.h>
#include <stddef.h>

typedef struct Stack Stack;

Stack *stack_create(void);
void stack_destroy(Stack *stack);
bool stack_push(Stack *stack, void *item);
bool stack_pop(Stack *stack, void **out_item);
bool stack_peek(const Stack *stack, void **out_item);
size_t stack_size(const Stack *stack);
bool stack_is_empty(const Stack *stack);
