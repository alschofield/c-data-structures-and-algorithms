# Data Structures and Algorithms in C

A C17 portfolio for independently implementing and documenting data
structures and algorithms from first principles.

## Progress

Completed and tested:

- Stack: generic, dynamically growing LIFO container.
- Queue: generic, dynamically growing circular FIFO buffer.
- Singly linked list: generic, head-only linked list with indexed operations.
- Dynamic array: generic, resizable contiguous collection with indexed operations.

Planned:

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
make test NAME=dynamic-array
```

The Makefile defaults to Clang. Override `CC` when needed, for example:

```bash
make CC=gcc test NAME=stack
```

## Verification Results

Each command below builds with strict C17 flags (`-Wall -Wextra -Wpedantic
-Werror -g`) before running its assertion-based test executable.

| Module | Test command | Coverage | Result | Median wall-clock time |
| --- | --- | --- | --- | --- |
| Stack | `make test NAME=stack` | Empty and null handling; generic values including `NULL`; LIFO ordering; 1,024-item growth and reuse. | Pass, 100/100 timed runs | 11.227 ms (6.025-47.323 ms) |
| Queue | `make test NAME=queue` | Empty and null handling; generic values including `NULL`; FIFO ordering; 1,024-item growth and reuse; circular-buffer wraparound growth. | Pass, 100/100 timed runs | 7.453 ms (5.915-32.369 ms) |
| Singly linked list | `make test NAME=singly-linked-list` | Empty and null handling; generic values including `NULL`; indexed insert/get/remove; head, append, tail, and final-node boundaries; 1,024-item traversal and reuse. | Pass, 100/100 timed runs | 8.740 ms (6.868-46.067 ms) |
| Dynamic array | `make test NAME=dynamic-array` | Empty and null handling; generic values including `NULL`; indexed insert/get/set/remove; append insertion; 1,024-item growth. | Pass, 100/100 timed runs | 75.942 ms (47.772-998.044 ms) |

Timings were captured on Windows by running each already-built test executable
100 times. They include process startup and operating-system scheduling, so
they verify test completion rather than benchmark individual data-structure
operations.

## Test-Driven Workflow

Each module README defines its API and behavior contract. Its test file is the
acceptance suite. Implement the header and source only after reading both.

- Containers store generic `void *` values and never free caller-owned values.
- A failed operation returns `false` and must not change an output parameter.
- `destroy(NULL)` is a no-op; other operations on a `NULL` container fail safely.
- Allocation-failure behavior is part of each contract, but needs allocator
  injection before it can be tested deterministically.
