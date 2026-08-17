# Doubly Linked List

Generic node-based collection with forward and backward links per node.
Stored `void *` values, including `NULL`, remain caller-owned.

## Required API

```c
bool doubly_linked_list_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that. The contract below specifies the list type and
operations (create/destroy, push/pop at both ends, indexed get/insert/remove,
size/is_empty).

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

## Learning Focus

The doubly linked list doubles the invariant surface of the singly linked
version: every splice must update four pointers correctly or the structure
silently corrupts. Implementing it builds the habit of stating and re-checking
structural invariants after each mutation, and shows what the second pointer
buys — O(1) tail removal and O(1) deletion given a node reference.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
