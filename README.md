# Data Structures and Algorithms in C

A C17 portfolio for independently implementing and documenting data
structures and algorithms from first principles.

> Brought to you by the offices of SchoTech: we bring you something you already
> have and will not need in the future, in a language you have already seen and
> also will not need in the future.

## Progress

Completed and tested:

- Stack: generic, dynamically growing LIFO container.
- Queue: generic, dynamically growing circular FIFO buffer.
- Singly linked list: generic, head-only linked list with indexed operations.
- Dynamic array: generic, resizable contiguous collection with indexed operations.
- Hash table: generic key-value collection using separate chaining.

Planned:

- Binary search tree

## Layout

```text
src/        One folder per data structure, containing its header, implementation,
            tests, and README
bench/      Reusable in-process benchmark harness and module benchmarks
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

| Module | Test command | Coverage | Result |
| --- | --- | --- | --- |
| Stack | `make test NAME=stack` | Empty and null handling; generic values including `NULL`; LIFO ordering; 1,024-item growth and reuse. | Pass |
| Queue | `make test NAME=queue` | Empty and null handling; generic values including `NULL`; FIFO ordering; 1,024-item growth and reuse; circular-buffer wraparound growth. | Pass |
| Singly linked list | `make test NAME=singly-linked-list` | Empty and null handling; generic values including `NULL`; indexed insert/get/remove; head, append, tail, and final-node boundaries; 1,024-item traversal and reuse. | Pass |
| Dynamic array | `make test NAME=dynamic-array` | Empty and null handling; generic values including `NULL`; indexed insert/get/set/remove; append insertion; 1,024-item growth. | Pass |
| Hash table | `make test NAME=hash-table` | Empty and null handling; caller-defined hash/equality; replacement; `NULL` values; removal; 256 forced collisions; and destruction. | Pass |

## Benchmarking

The dependency-free `bench/` harness measures repeated operations in one
process. It excludes process startup, setup, verification, and teardown from
the timed region, then reports median, minimum, and maximum nanoseconds per
operation across independent samples. Benchmark targets compile with `-O2`.

On Windows, it uses `QueryPerformanceCounter`; on POSIX systems, it uses
`clock_gettime(CLOCK_MONOTONIC)`.

```bash
make benchmark-test
make benchmark NAME=dynamic-array
make benchmark NAME=dynamic-array BENCHMARK_ITEM_COUNT=100000
```

Add a future benchmark as `bench/<module_name>_benchmark.c`. Each benchmark
provides setup, operation, verification, and teardown callbacks. To assess
growth trends, run the same operation at increasing input sizes and compare
normalized nanoseconds per operation; benchmarks provide evidence for expected
complexity but do not prove Big-O behavior.

Current benchmark coverage:

- Stack: push and pop.
- Queue: enqueue and dequeue.
- Singly linked list: push front/back, pop front/back, middle `get`, middle
  `insert`, and middle `remove`.
- Dynamic array: append insertion through the general `insert` API.
- Hash table: `set`, `get`, `contains`, and `remove` with distributed integer hashes.

Stack, queue, and dynamic-array benchmarks default to 10,000 operations per
sample. The singly linked-list benchmark defaults to 2,000 because its
back/indexed operations intentionally traverse nodes. The hash-table benchmark
also defaults to 10,000 operations per sample.

The current hash table has a fixed initial capacity of 10 buckets and does not
yet rehash. Its collision handling is correct, but average chain length grows
with the number of entries, so large-table operations do not yet meet the
expected O(1) complexity target. Run the benchmark at increasing item counts to
observe that behavior; resizing and rehashing are the next required improvement.

### Example Local Results

These measurements were collected on the development machine with Clang and
`-O2`; they are regression evidence for that environment, not portable speed
claims or proof of algorithmic complexity.

| Module / operation | Sample shape | Median ns/op |
| --- | --- | ---: |
| Stack push | 21 x 10,000 | 2.69 |
| Stack pop | 21 x 10,000 | 2.17 |
| Queue enqueue | 21 x 10,000 | 12.53 |
| Queue dequeue | 21 x 10,000 | 3.50 |
| Dynamic array append insertion | 21 x 10,000 | 3.84 |
| Singly linked list push front | 21 x 2,000 | 29.70 |
| Singly linked list push back | 21 x 2,000 | 1,628.05 |
| Singly linked list get middle | 21 x 2,000 | 1,117.55 |
| Singly linked list insert middle | 21 x 2,000 | 3,178.00 |
| Singly linked list remove middle | 21 x 2,000 | 577.50 |
| Hash table set | 21 x 1,000 | 98.20 |
| Hash table get | 21 x 1,000 | 64.50 |
| Hash table contains | 21 x 1,000 | 68.50 |
| Hash table remove | 21 x 1,000 | 84.80 |
| Hash table set | 21 x 10,000 | 2,634.72 |
| Hash table get | 21 x 10,000 | 2,507.58 |
| Hash table contains | 21 x 10,000 | 2,559.70 |
| Hash table remove | 21 x 10,000 | 2,540.57 |

The hash-table increase between 1,000 and 10,000 entries is expected from the
current fixed bucket count. The singly linked-list back and middle operations
likewise show their intentional traversal cost; the list deliberately remains
head-only rather than adding a saved tail pointer.

## Test-Driven Workflow

Each module README defines its API and behavior contract. Its test file is the
acceptance suite. Implement the header and source only after reading both.

- Containers store generic `void *` values and never free caller-owned values.
- A failed operation returns `false` and must not change an output parameter.
- `destroy(NULL)` is a no-op; other operations on a `NULL` container fail safely.
- Allocation-failure behavior is part of each contract, but needs allocator
  injection before it can be tested deterministically.

## Source Material

- [Capital STEEZ - Infinity And Beyond](https://www.youtube.com/watch?v=6z2W06cmpmE)
- The APIs, behavioral contracts, tests, benchmark harness, and README material
  are authored directly in this repository; no other external technical source
  was used or attributed during their creation.
