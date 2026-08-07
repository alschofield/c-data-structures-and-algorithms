CC := clang
CFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic -Werror -g
BENCH_CFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic -Werror -O2
BUILD_DIR := build
NAME ?=
MODULE := $(subst -,_,$(NAME))
MODULE_DIR := src/$(NAME)
BENCH_DIR := bench
BENCHMARK_ITEM_COUNT ?=

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
	@echo "Create src/<name>/<name_with_underscores>.h, .c, test_*.c, and README.md."
	@echo "Run: make test NAME=<name>"
	@echo "Run: make benchmark NAME=<name>"
	@echo "Verify the benchmark harness: make benchmark-test"

ifeq ($(strip $(NAME)),)
test:
	@echo "Set NAME to the module name. Example: make test NAME=stack"
benchmark:
	@echo "Set NAME to the module name. Example: make benchmark NAME=dynamic-array"
else
test:
	$(CREATE_BUILD_DIR)
	$(CC) $(CFLAGS) $(MODULE_DIR)/$(MODULE).c $(MODULE_DIR)/test_$(MODULE).c -o $(BUILD_DIR)/test_$(MODULE)
	$(BUILD_DIR)/test_$(MODULE)
benchmark:
	$(CREATE_BUILD_DIR)
	$(CC) $(BENCH_CFLAGS) $(if $(BENCHMARK_ITEM_COUNT),-DBENCHMARK_ITEM_COUNT=$(BENCHMARK_ITEM_COUNT)) $(BENCH_DIR)/benchmark.c $(BENCH_DIR)/$(MODULE)_benchmark.c $(MODULE_DIR)/$(MODULE).c -o $(BUILD_DIR)/benchmark_$(MODULE)
	$(BUILD_DIR)/benchmark_$(MODULE)
endif

benchmark-test:
	$(CREATE_BUILD_DIR)
	$(CC) $(CFLAGS) $(BENCH_DIR)/benchmark.c $(BENCH_DIR)/test_benchmark.c -o $(BUILD_DIR)/test_benchmark
	$(BUILD_DIR)/test_benchmark

clean:
	$(REMOVE_BUILD_DIR)
