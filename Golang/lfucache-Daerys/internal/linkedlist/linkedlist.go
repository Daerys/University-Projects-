package linkedlist

// Node represents an element in the linked list.
// It contains the stored value and pointers to the next and previous nodes.
type Node[V any] struct {
	next, previous *Node[V] // Pointers to the adjacent nodes.
	Value          V        // The value stored in the node.
	list           *LinkedList[V]
}

// NewNode creates and returns a new node with the specified value.
//
// Params:
//   - value: The value to be stored in the node.
//
// Returns:
//   - A pointer to the newly created Node.
func NewNode[V any](value V) *Node[V] {
	return &Node[V]{Value: value}
}

// Previous returns the previous node in the list.
//
// Returns:
//   - A pointer to the previous Node, or nil if it is the head.
func (n *Node[V]) Previous() *Node[V] {
	if n.list != nil && n.previous != n.list.dummy {
		return n.previous
	}
	return nil
}

// Next returns the next node in the list.
//
// Returns:
//   - A pointer to the next Node, or nil if it is the tail.
func (n *Node[V]) Next() *Node[V] {
	if n.list != nil && n.next != n.list.dummy {
		return n.next
	}
	return nil
}

// LinkedList represents a doubly linked list.
// It keeps track of the head and tail nodes and maintains the length of the list.
type LinkedList[V any] struct {
	dummy  *Node[V] // Pointers to the first and last nodes.
	length int      // The number of nodes in the list.
}

// New creates and returns an empty linked list.
//
// Returns:
//   - A pointer to the newly initialized LinkedList.
func New[V any]() *LinkedList[V] {
	dummy := NewNode[V](*new(V))
	dummy.next, dummy.previous = dummy, dummy
	return &LinkedList[V]{dummy: dummy}
}

// Len returns the current number of elements in the list.
//
// Returns:
//   - The length of the list.
func (l *LinkedList[V]) Len() int {
	return l.length
}

// Front returns the first node of the list.
//
// Returns:
//   - A pointer to the head Node, or nil if the list is empty.
func (l *LinkedList[V]) Front() *Node[V] {
	if l.length == 0 {
		return nil
	}
	return l.dummy.next
}

// Back returns the last node of the list.
//
// Returns:
//   - A pointer to the tail Node, or nil if the list is empty.
func (l *LinkedList[V]) Back() *Node[V] {
	if l.length == 0 {
		return nil
	}
	return l.dummy.previous
}

// PushFront inserts a new node with the given value at the front of the list.
//
// Params:
//   - value: The value to insert.
//
// Returns:
//   - A pointer to the newly added Node.
func (l *LinkedList[V]) PushFront(value *Node[V]) *Node[V] {
	return l.InsertAfter(l.dummy, value)
}

// PushBack inserts a new node with the given value at the back of the list.
//
// Params:
//   - value: The value to insert.
//
// Returns:
//   - A pointer to the newly added Node.
func (l *LinkedList[V]) PushBack(value *Node[V]) *Node[V] {
	return l.InsertAfter(l.dummy, value.previous)
}

// InsertAfter inserts a new node with the given value immediately after the specified node.
//
// Params:
//   - afterNode: The node after which the new node will be inserted.
//     If nil, the new node is inserted at the front.
//   - value: The value to insert.
//
// Returns:
//   - A pointer to the newly inserted Node.
func (l *LinkedList[V]) InsertAfter(afterNode *Node[V], value *Node[V]) *Node[V] {
	value.previous, value.next = afterNode, afterNode.next
	afterNode.next.previous, afterNode.next = value, value
	value.list = l
	l.length++
	return value
}

// Remove deletes the specified node from the list.
//
// Params:
//   - node: The node to remove.
//
// Returns:
//   - None.
func (l *LinkedList[V]) Remove(node *Node[V]) {
	node.previous.next, node.next.previous = node.next, node.previous
	node.next, node.previous = nil, nil
	node.list = nil
	l.length--
}
