package lfu

import (
	"errors"
	"iter"
	"lfucache/internal/linkedlist"
)

// ErrKeyNotFound is returned when a key is not found in the cache.
var ErrKeyNotFound = errors.New("key not found")

// DefaultCapacity defines the default capacity of the cache when none is provided.
const DefaultCapacity = 5

// Cache interface defines the methods for an LFU cache.
// O(capacity) memory usage.
type Cache[K comparable, V any] interface {
	// Get returns the value associated with the key if it exists in the cache,
	// otherwise, returns ErrKeyNotFound.
	//
	// O(1)
	Get(key K) (V, error)

	// Put updates the value of the key if present, or inserts the key if not already present.
	//
	// When the cache reaches its capacity, it should invalidate and remove the least frequently used key
	// before inserting a new item. When there is a tie (i.e., two or more keys with the same frequency),
	// the least recently used key will be invalidated.
	//
	// O(1)
	Put(key K, value V)

	// All returns the iterator in descending order of frequency.
	// If two or more keys have the same frequency, the most recently used key will be listed first.
	//
	// O(capacity)
	All() iter.Seq2[K, V]

	// Size returns the current number of elements in the cache.
	//
	// O(1)
	Size() int

	// Capacity returns the maximum number of elements the cache can hold.
	//
	// O(1)
	Capacity() int

	// GetKeyFrequency returns the access frequency of the key if it exists in the cache,
	// otherwise, returns ErrKeyNotFound.
	//
	// O(1)
	GetKeyFrequency(key K) (int, error)
}

type cacheEntity[K comparable, V any] struct {
	key             K
	value           V
	frequencyParent *linkedlist.Node[*frequencyItem[K, V]]
}

type frequencyItem[K comparable, V any] struct {
	values    *linkedlist.LinkedList[*cacheEntity[K, V]]
	frequency int
}

func newFrequency[K comparable, V any](frequency int) *frequencyItem[K, V] {
	return &frequencyItem[K, V]{
		values:    linkedlist.New[*cacheEntity[K, V]](),
		frequency: frequency,
	}
}

// cacheImpl represents the LFU cache implementation.
type cacheImpl[K comparable, V any] struct {
	capacity      int
	keyToNode     map[K]*linkedlist.Node[*cacheEntity[K, V]]
	frequencyList *linkedlist.LinkedList[*frequencyItem[K, V]]
}

// New initializes the cache with the given capacity.
func New[K comparable, V any](capacity ...int) *cacheImpl[K, V] {
	var capValue int
	if len(capacity) == 1 {
		capValue = capacity[0]
	} else {
		capValue = DefaultCapacity
	}
	if capValue < 0 {
		panic("New: non-negative capacity expected.")
	}
	return &cacheImpl[K, V]{
		capacity:      capValue,
		keyToNode:     make(map[K]*linkedlist.Node[*cacheEntity[K, V]], capValue),
		frequencyList: linkedlist.New[*frequencyItem[K, V]](),
	}
}

// Get retrieves the value associated with the specified key from the cache.
// It increments the frequency count of the key.
// If the key does not exist, it returns ErrKeyNotFound.
func (cache *cacheImpl[K, V]) Get(key K) (V, error) {
	var zero V
	// Check if the key exists in the cache
	valueNode, ok := cache.keyToNode[key]
	if !ok {
		return zero, ErrKeyNotFound
	}

	// Retrieve the value node from the frequency item
	value := valueNode.Value.value

	// Increment the frequency of the key
	cache.incrementFrequency(key, valueNode)

	return value, nil
}

// Put adds the key-value cacheEntity to the cache.
// If the key already exists, it updates its value and increments its frequency.
// If the key does not exist, it inserts it with frequency 1.
// If the cache is at capacity, it evicts the least frequently used item before inserting.
func (cache *cacheImpl[K, V]) Put(key K, value V) {
	if cacheEntityNode, ok := cache.keyToNode[key]; ok {
		cacheEntityNode.Value.value = value
		cache.incrementFrequency(key, cacheEntityNode)
		return
	}

	if cache.capacity == 1 {
		if cache.frequencyList.Len() == 0 {
			cache.insertNewKey(key, value)
			return
		}
		freqNode := cache.frequencyList.Front()
		cacheEntityNode := freqNode.Value.values.Front()

		// Removing existing current value inside cache
		clear(cache.keyToNode)

		// Changing value inside cache without creating new node
		cacheEntityNode.Value.key, cacheEntityNode.Value.value = key, value

		cache.keyToNode[key] = cacheEntityNode
		freqNode.Value.frequency = 1
		return
	}

	if len(cache.keyToNode) >= cache.capacity {
		cache.evictLFU()
	}
	// Insert the new key with frequency 1
	cache.insertNewKey(key, value)
}

// All returns an iterator over the cache items in descending order of frequency.
// For items with the same frequency, the most recently used items come first.
func (cache *cacheImpl[K, V]) All() iter.Seq2[K, V] {
	return func(yield func(K, V) bool) {
		// Iterate over frequencies in descending order
		for freqNode := cache.frequencyList.Back(); freqNode != nil; freqNode = freqNode.Previous() {
			freqItem := freqNode.Value
			// Iterate over values in frequency item
			for valueNode := freqItem.values.Front(); valueNode != nil; valueNode = valueNode.Next() {
				pair := valueNode.Value
				if !yield(pair.key, pair.value) {
					return
				}
			}
		}
	}
}

// Size returns the current number of items in the cache.
func (cache *cacheImpl[K, V]) Size() int {
	return len(cache.keyToNode)
}

// Capacity returns the maximum number of items the cache can hold.
func (cache *cacheImpl[K, V]) Capacity() int {
	return cache.capacity
}

// GetKeyFrequency returns the current frequency count of the specified key.
// If the key does not exist, it returns ErrKeyNotFound.
func (cache *cacheImpl[K, V]) GetKeyFrequency(key K) (int, error) {
	if cacheEntityNode, ok := cache.keyToNode[key]; ok {
		return cacheEntityNode.Value.frequencyParent.Value.frequency, nil
	}
	return 0, ErrKeyNotFound
}

// incrementFrequency increases the frequency of a key by moving it to the next frequency list.
// It updates the internal data structures accordingly.
func (cache *cacheImpl[K, V]) incrementFrequency(key K, valueNode *linkedlist.Node[*cacheEntity[K, V]]) {
	freqNode := valueNode.Value.frequencyParent
	currentFreqItem := freqNode.Value
	nextFreq := currentFreqItem.frequency + 1

	nextFreqNode := freqNode.Next()
	if nextFreqNode == nil || nextFreqNode.Value.frequency != nextFreq {
		nextFreqNode = cache.frequencyList.InsertAfter(freqNode, linkedlist.NewNode(newFrequency[K, V](nextFreq)))
	}

	// Remove the key from the current frequency item
	currentFreqItem.values.Remove(valueNode)

	// If current frequency item is empty, remove it
	if currentFreqItem.values.Len() == 0 {
		cache.frequencyList.Remove(freqNode)
	}

	// Move the existing value node to the next frequency item
	cache.pushIntoFrequency(key, valueNode, nextFreqNode)
}

// insertNewKey adds a new key-value cacheEntity to the cache with frequency 1.
func (cache *cacheImpl[K, V]) insertNewKey(key K, value V) {
	var freqNode *linkedlist.Node[*frequencyItem[K, V]]
	// Check if frequency 1 already exists
	if cache.frequencyList.Front() != nil && cache.frequencyList.Front().Value.frequency == 1 {
		freqNode = cache.frequencyList.Front()
	} else {
		// Create a new frequency item for frequency 1
		freqNode = cache.frequencyList.PushFront(linkedlist.NewNode(&frequencyItem[K, V]{
			values:    linkedlist.New[*cacheEntity[K, V]](),
			frequency: 1,
		}))
	}

	// Add the new key-value cacheEntity to the frequency item
	cache.pushIntoFrequency(key, linkedlist.NewNode(&cacheEntity[K, V]{key, value, freqNode}), freqNode)
}

// evictLFU removes the least frequently used item from the cache.
func (cache *cacheImpl[K, V]) evictLFU() {
	freqNode := cache.frequencyList.Front()
	if freqNode == nil {
		return
	}
	freqItem := freqNode.Value

	// Remove the least recently used key from the least frequently used frequency item
	valueNode := freqItem.values.Back()
	evictKey := valueNode.Value.key

	// Remove the key from cache
	delete(cache.keyToNode, evictKey)

	freqItem.values.Remove(valueNode)

	// If frequency item is empty, remove it and update minFreq
	if freqItem.values.Len() == 0 {
		cache.frequencyList.Remove(freqNode)
	}
}

// Pushing node containing key and value used in cache into other frequency
func (cache *cacheImpl[K, V]) pushIntoFrequency(key K, valueNode *linkedlist.Node[*cacheEntity[K, V]], nextFreqNode *linkedlist.Node[*frequencyItem[K, V]]) {
	nextFreqNode.Value.values.PushFront(valueNode)
	cache.keyToNode[key] = valueNode
	valueNode.Value.frequencyParent = nextFreqNode
}
