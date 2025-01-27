package workerpool

import (
	"context"
	"sync"
)

// Accumulator is a function type used to aggregate values of type T into a result of type R.
// It must be thread-safe, as multiple goroutines will access the accumulator function concurrently.
// Each worker will produce intermediate results, which are combined with an initial or
// accumulated value.
type Accumulator[T, R any] func(current T, accum R) R

// Transformer is a function type used to transform an element of type T to another type R.
// The function is invoked concurrently by multiple workers, and therefore must be thread-safe
// to ensure data integrity when accessed across multiple goroutines.
// Each worker independently applies the transformer to its own subset of data, and although
// no shared state is expected, the transformer must handle any internal state in a thread-safe
// manner if present.
type Transformer[T, R any] func(current T) R

// Searcher is a function type for exploring data in a hierarchical manner.
// Each call to Searcher takes a parent element of type T and returns a slice of T representing
// its child elements. Since multiple goroutines may call Searcher concurrently, it must be
// thread-safe to ensure consistent results during recursive  exploration.
//
// Important considerations:
//  1. Searcher should be designed to avoid race conditions, particularly if it captures external
//     variables in closures.
//  2. The calling function must handle any state or values in closures, ensuring that
//     captured variables remain consistent throughout recursive or hierarchical search paths.
type Searcher[T any] func(parent T) []T

// Pool is the primary interface for managing worker pools, with support for three main
// operations: Transform, Accumulate, and List. Each operation takes an input channel, applies
// a transformation, accumulation, or list expansion, and returns the respective output.
type Pool[T, R any] interface {
	// Transform applies a transformer function to each item received from the input channel,
	// with results sent to the output channel. Transform operates concurrently, utilizing the
	// specified number of workers. The number of workers must be explicitly defined in the
	// configuration for this function to handle expected workloads effectively.
	// Since multiple workers may call the transformer function concurrently, it must be
	// thread-safe to prevent race conditions or unexpected results when handling shared or
	// internal state. Each worker independently applies the transformer function to its own
	// data subset.
	Transform(ctx context.Context, workers int, input <-chan T, transformer Transformer[T, R]) <-chan R

	// Accumulate applies an accumulator function to the items received from the input channel,
	// with results accumulated and sent to the output channel. The accumulator function must
	// be thread-safe, as multiple workers concurrently update the accumulated result.
	// The output channel will contain intermediate accumulated results as R
	Accumulate(ctx context.Context, workers int, input <-chan T, accumulator Accumulator[T, R]) <-chan R

	// List expands elements based on a searcher function, starting
	// from the given element. The searcher function finds child elements for each parent,
	// allowing exploration in a tree-like structure.
	// The number of workers should be configured based on the workload, ensuring each worker
	// independently processes assigned elements.
	List(ctx context.Context, workers int, start T, searcher Searcher[T])
}

type poolImpl[T, R any] struct{}

func New[T, R any]() *poolImpl[T, R] {
	return &poolImpl[T, R]{}
}

// Accumulate processes input items using multiple workers and accumulates the results.
// It takes a context for cancellation, the number of worker goroutines, an input channel,
// and an accumulator function. It returns a channel that emits the accumulated results.
func (p *poolImpl[T, R]) Accumulate(
	ctx context.Context,
	workers int,
	input <-chan T,
	accumulator Accumulator[T, R],
) <-chan R {
	// Create a channel to send the accumulated results.
	results := make(chan R)

	// Initialize a WaitGroup to synchronize the completion of all workers.
	var wg sync.WaitGroup

	// Define the worker function that each goroutine will execute.
	worker := func() {
		// Ensure the WaitGroup counter is decremented when the worker exits.
		defer wg.Done()

		// Initialize a local variable to hold the accumulated result.
		var localR R

		// Continuously process items until the context is canceled or the input channel is closed.
		for {
			select {
			// If the context is done, exit the worker.
			case <-ctx.Done():
				return

			// Receive an item from the input channel.
			case item, ok := <-input:
				if ok {
					// Apply the accumulator function to the received item and the current accumulated result.
					localR = accumulator(item, localR)
					continue
				}

				// Attempt to send the accumulated result to the results channel.
				select {
				case results <- localR:
					// Successfully sent the result.
				case <-ctx.Done():
					// If the context is done while sending, exit the worker.
					return
				}

				// Exit the worker after processing one item.
				return
			}
		}
	}

	// Launch the specified number of worker goroutines.
	for range workers {
		// Increment the WaitGroup counter for each worker.
		wg.Add(1)
		// Start the worker goroutine.
		go worker()
	}

	// Launch a goroutine to close the results channel once all workers have finished.
	go func() {
		// Wait for all workers to complete.
		wg.Wait()
		// Close the results channel to signal that no more results will be sent.
		close(results)
	}()

	// Return the channel from which accumulated results can be received.
	return results
}

// List performs a breadth-first search starting from the 'start' item using multiple workers.
// It processes each layer of items concurrently and collects the next layer of items using the provided searcher function.
func (p *poolImpl[T, R]) List(ctx context.Context, workers int, start T, searcher Searcher[T]) {
	// Initialize the current layer with the starting item.
	currentLayer := []T{start}

	// Create a mutex to protect access to the nextLayer slice.
	var mu sync.Mutex

	// Initialize a WaitGroup to synchronize the completion of all workers.
	var wg sync.WaitGroup

	// Continue processing layers until there are no more items to process.
	for len(currentLayer) > 0 {
		// Initialize a slice to hold the next layer of items.
		var nextLayer []T

		// Add the number of workers to the WaitGroup counter.
		wg.Add(workers)

		// Launch worker goroutines to process the current layer.
		for w := 0; w < workers; w++ {
			// Start a worker goroutine with a unique worker ID.
			go func(workerID int) {
				// Ensure the WaitGroup counter is decremented when the worker exits.
				defer wg.Done()

				// Each worker processes items assigned based on their worker ID.
				for i := workerID; i < len(currentLayer); i += workers {
					select {
					// If the context is done, exit the worker.
					case <-ctx.Done():
						return
					default:
						// Retrieve the parent item from the current layer.
						parent := currentLayer[i]
						// Use the searcher function to find child items of the parent.
						children := searcher(parent)
						if len(children) > 0 {
							// Lock the mutex before modifying the nextLayer slice.
							mu.Lock()
							// Append the child items to the nextLayer slice.
							nextLayer = append(nextLayer, children...)
							// Unlock the mutex after modification.
							mu.Unlock()
						}
					}
				}
			}(w) // Pass the worker ID to the goroutine.
		}

		// Wait for all workers to finish processing the current layer.
		wg.Wait()

		// Update the currentLayer with the items collected for the next layer.
		currentLayer = nextLayer
	}
}

// Transform applies a transformation function to each item from the input channel using multiple workers.
// It takes a context for cancellation, the number of worker goroutines, an input channel,
// and a transformer function. It returns a channel that emits the transformed results.
func (p *poolImpl[T, R]) Transform(
	ctx context.Context,
	workers int,
	input <-chan T,
	transformer Transformer[T, R],
) <-chan R {
	// Create a channel to send the transformed results.
	result := make(chan R)

	// Initialize a WaitGroup to synchronize the completion of all workers.
	var wg sync.WaitGroup

	// Define the worker function that each goroutine will execute.
	worker := func() {
		// Ensure the WaitGroup counter is decremented when the worker exits.
		defer wg.Done()

		// Continuously process elements from the input channel until it's closed or the context is canceled.
		for {
			select {
			// If the context is done, exit the worker.
			case <-ctx.Done():
				return

			// Receive an element from the input channel.
			case element, ok := <-input:
				if !ok {
					// If the input channel is closed, exit the worker.
					return
				}

				// Attempt to send the transformed element to the result channel.
				select {
				case <-ctx.Done():
					// If the context is done while sending, exit the worker.
					return
				case result <- transformer(element):
					// Successfully sent the transformed result.
				}
			}
		}
	}

	// Launch the specified number of worker goroutines.
	for range workers {
		// Increment the WaitGroup counter for each worker.
		wg.Add(1)
		// Start the worker goroutine.
		go worker()
	}

	// Launch a goroutine to close the result channel once all workers have finished.
	go func() {
		// Ensure the result channel is closed after all workers are done.
		defer close(result)
		// Wait for all workers to complete.
		wg.Wait()
	}()

	// Return the channel from which transformed results can be received.
	return result
}
