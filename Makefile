CC := clang
CFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic -Werror -g
BENCH_CFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic -Werror -O2
BUILD_DIR := build
NAME ?=
MODULE := $(subst -,_,$(notdir $(NAME)))
MODULE_DIR := src/$(NAME)
BENCH_DIR := bench
BENCHMARK_ITEM_COUNT ?=

# The curriculum leaf describes the collision strategy; its established API keeps
# the generic hash_table filename.
ifeq ($(NAME),data-structures/associative/hash-tables/separate-chaining)
MODULE := hash_table
endif

# Graph traversals link against the adjacency-list module they traverse.
EXTRA_SOURCES :=
ifneq ($(filter $(NAME),algorithms/graph-traversal/breadth-first-search algorithms/graph-traversal/depth-first-search),)
EXTRA_SOURCES := src/data-structures/graphs/representations/adjacency-list/adjacency_list.c
endif

ifeq ($(OS),Windows_NT)
CREATE_BUILD_DIR = if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
REMOVE_BUILD_DIR = if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
else
CREATE_BUILD_DIR = mkdir -p "$(BUILD_DIR)"
REMOVE_BUILD_DIR = rm -rf "$(BUILD_DIR)"
endif

.DEFAULT_GOAL := help
.PHONY: help test benchmark benchmark-test clean

help:
	@echo "Run a taxonomy topic: make test NAME=data-structures/linear/stacks/stack"
	@echo "Run a scaffold topic: make test NAME=algorithms/searching/linear-search"
	@echo "Run a benchmark-backed completed topic: make benchmark NAME=data-structures/linear/stacks/stack BENCHMARK=stack"
	@echo "Verify the benchmark harness: make benchmark-test"

ifeq ($(strip $(NAME)),)
test:
	@echo "Set NAME to a taxonomy topic. Example: make test NAME=data-structures/linear/stacks/stack"
benchmark:
	@echo "Set NAME and BENCHMARK. Example: make benchmark NAME=data-structures/linear/arrays/dynamic-array BENCHMARK=dynamic_array"
else
test:
	$(CREATE_BUILD_DIR)
	$(CC) $(CFLAGS) $(MODULE_DIR)/$(MODULE).c $(EXTRA_SOURCES) $(MODULE_DIR)/test_$(MODULE).c -o $(BUILD_DIR)/test_$(MODULE)
	$(BUILD_DIR)/test_$(MODULE)
benchmark:
	$(CREATE_BUILD_DIR)
	$(CC) $(BENCH_CFLAGS) $(if $(BENCHMARK_ITEM_COUNT),-DBENCHMARK_ITEM_COUNT=$(BENCHMARK_ITEM_COUNT)) $(BENCH_DIR)/benchmark.c $(BENCH_DIR)/$(BENCHMARK)_benchmark.c $(MODULE_DIR)/$(MODULE).c -o $(BUILD_DIR)/benchmark_$(MODULE)
	$(BUILD_DIR)/benchmark_$(MODULE)
endif

benchmark-test:
	$(CREATE_BUILD_DIR)
	$(CC) $(CFLAGS) $(BENCH_DIR)/benchmark.c $(BENCH_DIR)/test_benchmark.c -o $(BUILD_DIR)/test_benchmark
	$(BUILD_DIR)/test_benchmark

clean:
	$(REMOVE_BUILD_DIR)
