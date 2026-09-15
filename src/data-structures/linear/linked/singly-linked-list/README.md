# Singly Linked List

An opaque forward chain of borrowed item pointers. It tracks a head and size, so head work is direct while tail and indexed work walk links.

## C API
```c
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
bool singly_linked_list_graph_view(const SinglyLinkedList *list, GraphView *out_view);
```

## Behavior, Ownership, and Invariants
- Element indexes are `[0, size)`; `insert` accepts `[0, size]`. Removal/read outputs are mandatory and are untouched when validation fails.
- Nodes are list-owned; item pointers are borrowed and may be null. `destroy` is null-safe and frees nodes, not items.
- Each node points forward; an empty list has a null head. `graph_view` borrows `list`, indexes nodes by list order, and exposes directed unit edges to each successor. It expires when the list is destroyed or structurally changed.
- Allocation failure leaves the list unchanged. Null size is zero and null is empty.

## Complexity and Verification
Push/pop front are O(1); tail and indexed operations are O(n); size/empty are O(1). Verify with `make test NAME=data-structures/linear/linked/singly-linked-list`.
