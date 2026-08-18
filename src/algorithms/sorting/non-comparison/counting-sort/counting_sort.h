#ifndef COUNTING_SORT_H
#define COUNTING_SORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool counting_sort(uint32_t *items, size_t count, uint32_t key_limit);

#endif
