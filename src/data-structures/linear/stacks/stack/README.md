# Stack

Generic LIFO collection that stores caller-owned `void *` values, including
`NULL`.

## Required API

```c
typedef struct Stack Stack;

Stack *stack_create(void);
void stack_destroy(Stack *stack);
bool stack_push(Stack *stack, void *item);
bool stack_pop(Stack *stack, void **out_item);
bool stack_peek(const Stack *stack, void **out_item);
size_t stack_size(const Stack *stack);
bool stack_is_empty(const Stack *stack);
```

## Contract

- `push` appends to the top; `pop` and `peek` return the most recently pushed
  item.
- `pop` removes its item; `peek` does not.
- `pop` and `peek` fail on an empty stack or `NULL` output pointer without
  changing `*out_item`.
- `push` returns `false` if allocation fails and leaves the stack unchanged.
- `destroy` frees only stack-owned memory, never stored values.
- `size` is always non-negative and `is_empty` is equivalent to `size == 0`.

## Complexity Targets

- `push`: amortized O(1)
- `pop`, `peek`, `size`, `is_empty`: O(1)
