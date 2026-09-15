# Stack

An opaque LIFO stack backed by a doubling pointer array. The last pushed item is the first item popped.

## C API
```c
Stack *stack_create(void); void stack_destroy(Stack *stack);
bool stack_push(Stack *stack, void *item);
bool stack_pop(Stack *stack, void **out_item);
bool stack_peek(const Stack *stack, void **out_item);
size_t stack_size(const Stack *stack); bool stack_is_empty(const Stack *stack);
```

## Contract
- `pop` and `peek` fail for null/empty stacks or null output storage; failed calls do not remove an item or write the output.
- Items are borrowed and may be null. `destroy` accepts null and frees only stack allocations, never stored items.
- Push grows from one slot by doubling and rejects capacity/byte-size overflow or allocation failure without changing the stack. Null size is zero and null is empty.

## Complexity and Verification
Push is amortized O(1); pop, peek, size, and empty are O(1). Verify with `make test NAME=data-structures/linear/stacks/stack`.

Generic LIFO collection that stores caller-owned `void *` values, including
`NULL`.

## How It Works

Last in, first out — a stack of plates. Push places on top, pop takes from
the top, and nothing below the top is reachable without removing what sits
on it. Array-backed with a count, the top is just index count-1, so both
operations are O(1) pointer-and-counter work plus occasional geometric
growth. The LIFO discipline is the call stack, undo history, and DFS's
frontier.

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
- Space: O(n) contiguous
