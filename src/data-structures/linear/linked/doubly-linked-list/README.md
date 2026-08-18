# Doubly Linked List

Generic node-based collection with forward and backward links per node.
Stored `void *` values, including `NULL`, remain caller-owned.

## Required API

```c
typedef struct DoublyLinkedList DoublyLinkedList;

DoublyLinkedList *doubly_linked_list_create(void);
void doubly_linked_list_destroy(DoublyLinkedList *list);
bool doubly_linked_list_push_front(DoublyLinkedList *list, void *item);
bool doubly_linked_list_push_back(DoublyLinkedList *list, void *item);
bool doubly_linked_list_pop_front(DoublyLinkedList *list, void **out_item);
bool doubly_linked_list_pop_back(DoublyLinkedList *list, void **out_item);
bool doubly_linked_list_get(const DoublyLinkedList *list, size_t index, void **out_item);
bool doubly_linked_list_insert(DoublyLinkedList *list, size_t index, void *item);
bool doubly_linked_list_remove(DoublyLinkedList *list, size_t index, void **out_item);
size_t doubly_linked_list_size(const DoublyLinkedList *list);
bool doubly_linked_list_is_empty(const DoublyLinkedList *list);
```

## Contract

- Every node maintains both `next` and `prev`; after any mutation,
  `node->next->prev == node` and `node->prev->next == node` hold throughout
  the list.
- Both ends support O(1) push and pop via head and tail pointers.
- Valid element indexes are `[0, size)`; `insert` also accepts `size` to
  append. Indexed traversal should start from the nearer end.
- Failed operations leave output parameters and list contents unchanged.
- Removing the final node returns the list to a valid empty state with both
  head and tail cleared.
- `destroy` frees nodes only, never stored values.

## Complexity Targets

- `push_front`, `pop_front`, `push_back`, `pop_back`, `size`, `is_empty`: O(1)
- `get`, `insert`, `remove` by index: O(n), at most n/2 traversal steps from
  the nearer end
- Space: O(n) nodes, two pointers of overhead per node
