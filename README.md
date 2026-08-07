# Data Structures and Algorithms in C

A C17 portfolio for independently implementing and documenting data
structures and algorithms from first principles.

## Progress

Completed and tested:

- Stack: generic, dynamically growing LIFO container.
- Queue: generic, dynamically growing circular FIFO buffer.
- Singly linked list: generic, head-only linked list with indexed operations.

Planned:

- Dynamic array
- Hash table
- Binary search tree

## Layout

```text
src/        One folder per data structure, containing its header, implementation,
            tests, and README
build/      Generated binaries and debug symbols; ignored by Git
Makefile    Local build and test commands
```

## Adding a Module

For a stack, create these files:

```text
src/stack/stack.h
src/stack/stack.c
src/stack/test_stack.c
src/stack/README.md
```

Then compile and run its test executable:

```bash
make test NAME=stack
```

Completed module test commands:

```bash
make test NAME=stack
make test NAME=queue
make test NAME=singly-linked-list
```

The Makefile defaults to Clang. Override `CC` when needed, for example:

```bash
make CC=gcc test NAME=stack
```

## Test-Driven Workflow

Each module README defines its API and behavior contract. Its test file is the
acceptance suite. Implement the header and source only after reading both.

- Containers store generic `void *` values and never free caller-owned values.
- A failed operation returns `false` and must not change an output parameter.
- `destroy(NULL)` is a no-op; other operations on a `NULL` container fail safely.
- Allocation-failure behavior is part of each contract, but needs allocator
  injection before it can be tested deterministically.
