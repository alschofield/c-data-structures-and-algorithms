CC := clang
CFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic -Werror -g
BUILD_DIR := build
NAME ?=
MODULE := $(subst -,_,$(NAME))
MODULE_DIR := src/$(NAME)

ifeq ($(OS),Windows_NT)
CREATE_BUILD_DIR = if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
REMOVE_BUILD_DIR = if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
else
CREATE_BUILD_DIR = mkdir -p "$(BUILD_DIR)"
REMOVE_BUILD_DIR = rm -rf "$(BUILD_DIR)"
endif

.DEFAULT_GOAL := help
.PHONY: help test clean

help:
	@echo "Create src/<name>/<name_with_underscores>.h, .c, test_*.c, and README.md."
	@echo "Run: make test NAME=<name>"

ifeq ($(strip $(NAME)),)
test:
	@echo "Set NAME to the module name. Example: make test NAME=stack"
else
test:
	$(CREATE_BUILD_DIR)
	$(CC) $(CFLAGS) $(MODULE_DIR)/$(MODULE).c $(MODULE_DIR)/test_$(MODULE).c -o $(BUILD_DIR)/test_$(MODULE)
	$(BUILD_DIR)/test_$(MODULE)
endif

clean:
	$(REMOVE_BUILD_DIR)
