# Singly Linked List

Generic node-based collection with one forward link. Stored `void *` values,
including `NULL`, remain caller-owned.

## How It Works

A chain of nodes, each holding a value and a pointer to the next. The list
holds only the head, so the front is O(1) and everything else is a walk —
push back must traverse all n nodes because nothing remembers the tail.
Insertion and removal never shift elements; they re-point two pointers.
The trade against the dynamic array: cheap splicing, but every step is a
dependent pointer load with no cache locality.

## Required API

```c
typedef struct SinglyLinkedList SinglyLinkedList;

SinglyLinkedList *singly_linked_list_create(void);
void singly_linked_list_destroy(SinglyLinkedList *list);
bool singly_linked_list_push_front(SinglyLinkedList *list, void *item);
bool singly_linked_list_push_back(SinglyLinkedList *list, void *item);
bool singly_linked_list_pop_front(SinglyLinkedList *list, void **out_item);
bool singly_linked_list_pop_back(SinglyLinkedList *list, void **out_item);
bool singly_linked_list_get(const SinglyLinkedList *list, size_t index, void **out_item);
bool singly_linked_list_insert(SinglyLinkedList *list, size_t index, void *item);
bool singly_linked_list_remove(SinglyLinkedList *list, size_t index, void **out_item);
size_t singly_linked_list_size(const SinglyLinkedList *list);
bool singly_linked_list_is_empty(const SinglyLinkedList *list);
```

## Contract

- Valid element indexes are `[0, size)`; `insert` also accepts `size`.
- Failed operations leave output parameters and list contents unchanged.
- Removing the final node returns the list to a valid empty state.
- `destroy` frees nodes only, never stored values.

## Complexity Targets

- `push_front`, `pop_front`, `size`, `is_empty`: O(1)
- `push_back`, `pop_back`, `get`, `insert`, `remove`: O(n)
- Space: O(n) nodes, one pointer of overhead per node

## Optional GraphView Wrapper

A singly linked list can expose an optional read-only GraphView wrapper where
each list Node maps to one adapter-owned GraphView Node and each `next` link is
a directed unweighted edge. BFS/DFS over this wrapper is equivalent to walking
the list, so the wrapper is primarily useful for uniform traversal experiments
and GraphView benchmark coverage.

- The wrapper owns dense Node-index mapping and borrows list structure only.
- List mutation invalidates wrapper relationships and is disallowed while the
  wrapper is in use.
