# Doubly Linked List

## Implementation Status

Implemented. This document describes the current learner-owned implementation contract.

## How It Works

An opaque bidirectional chain of borrowed pointers with first/last links. Indexed operations choose the nearer end.

Nodes carry links between neighboring values so endpoint updates do not require shifting a contiguous array.

## Required API

```c
DoublyLinkedList *doubly_linked_list_create(void); void doubly_linked_list_destroy(DoublyLinkedList *list);
bool doubly_linked_list_push_front(DoublyLinkedList *list, void *item); bool doubly_linked_list_push_back(DoublyLinkedList *list, void *item);
bool doubly_linked_list_pop_front(DoublyLinkedList *list, void **out_item); bool doubly_linked_list_pop_back(DoublyLinkedList *list, void **out_item);
bool doubly_linked_list_get(const DoublyLinkedList *list, size_t index, void **out_item); bool doubly_linked_list_insert(DoublyLinkedList *list, size_t index, void *item);
bool doubly_linked_list_remove(DoublyLinkedList *list, size_t index, void **out_item); size_t doubly_linked_list_size(const DoublyLinkedList *list);
bool doubly_linked_list_is_empty(const DoublyLinkedList *list); bool doubly_linked_list_graph_view(const DoublyLinkedList *list, GraphView *out_view);
```

## Contract

- Element indexes are `[0, size)`; insertion also allows `size`. Missing list/output storage, empty pops, and invalid indexes fail without mutation.
- The list owns nodes only. Payload pointers, including null, remain caller-owned; `destroy(NULL)` is a no-op. Empty lists have null first and last pointers; neighboring nodes maintain reciprocal links.
- Its borrowed GraphView uses list-order indexes and directed unit arcs to both adjacent nodes. Do not retain it beyond the list lifetime or a structural mutation.

End push/pop are O(1); indexed work is O(min(index, size - index)); size/empty are O(1). Verify with `make test NAME=data-structures/linear/linked/doubly-linked-list`.

## Complexity Targets

Target: O(1) endpoint-link operations, O(n) indexed traversal, and O(n) storage.

## Verification

```sh
make test NAME=<topic-path>
```
