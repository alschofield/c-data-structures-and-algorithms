#ifndef BREADTH_FIRST_SEARCH_H
#define BREADTH_FIRST_SEARCH_H

#include <stdbool.h>
#include <stddef.h>

#include "../../../data-structures/graphs/representations/adjacency-list/adjacency_list.h"

bool breadth_first_search(const AdjacencyList *graph, size_t source,
                          size_t *out_order, size_t *out_count);

#endif
