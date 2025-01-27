package crawler

import (
	"context"
	"crawler/internal/fs"
	"crawler/internal/workerpool"
	"encoding/json"
	"errors"
	"fmt"
	"sync"
)

// Configuration holds the configuration for the crawler, specifying the number of workers for
// file searching, processing, and accumulating tasks. The values for SearchWorkers, FileWorkers,
// and AccumulatorWorkers are critical to efficient performance and must be defined in
// every configuration.
type Configuration struct {
	SearchWorkers      int // Number of workers responsible for searching files.
	FileWorkers        int // Number of workers for processing individual files.
	AccumulatorWorkers int // Number of workers for accumulating results.
}

// Combiner is a function type that defines how to combine two values of type R into a single
// result. Combiner is not required to be thread-safe
//
// Combiner can either:
//   - Modify one of its input arguments to include the result of the other and return it,
//     or
//   - Create a new combined result based on the inputs and return it.
//
// It is assumed that type R has a neutral element (forming a monoid)
type Combiner[R any] func(current R, accum R) R

// Crawler represents a concurrent crawler implementing a map-reduce model with multiple workers
// to manage file processing, transformation, and accumulation tasks. The crawler is designed to
// handle large sets of files efficiently, assuming that all files can fit into memory
// simultaneously.
type Crawler[T, R any] interface {
	// Collect performs the full crawling operation, coordinating with the file system
	// and worker pool to process files and accumulate results. The result type R is assumed
	// to be a monoid, meaning there exists a neutral element for combination, and that
	// R supports an associative combiner operation.
	// The result of this collection process, after all reductions, is returned as type R.
	//
	// Important requirements:
	// 1. Number of workers in the Configuration is mandatory for managing workload efficiently.
	// 2. FileSystem and Accumulator must be thread-safe.
	// 3. Combiner does not need to be thread-safe.
	// 4. If an accumulator or combiner function modifies one of its arguments,
	//    it should return that modified value rather than creating a new one,
	//    or alternatively, it can create and return a new combined result.
	// 5. Context cancellation is respected across workers.
	// 6. Type T is derived by json-deserializing the file contents, and any issues in deserialization
	//    must be handled within the worker.
	// 7. The combiner function will wait for all workers to complete, ensuring no goroutine leaks
	//    occur during the process.
	Collect(
		ctx context.Context,
		fileSystem fs.FileSystem,
		root string,
		conf Configuration,
		accumulator workerpool.Accumulator[T, R],
		combiner Combiner[R],
	) (R, error)
}

type crawlerImpl[T, R any] struct{}

func New[T, R any]() *crawlerImpl[T, R] {
	return &crawlerImpl[T, R]{}
}

// Collect orchestrates the process of crawling a file system, transforming and accumulating data.
// It traverses directories starting from the 'root', processes files concurrently using worker pools,
// and combines the results using the provided accumulator and combiner functions.
// It returns the final accumulated result or an error if one occurs during processing.
func (c *crawlerImpl[T, R]) Collect(
	ctx context.Context,
	fileSystem fs.FileSystem,
	root string,
	conf Configuration,
	accumulator workerpool.Accumulator[T, R],
	combiner Combiner[R],
) (R, error) {
	// Initialize the result variable of type R and an error variable.
	var result R
	var err error

	// Create a new context that can be canceled, derived from the provided context.
	contextWithCancel, cancel := context.WithCancel(ctx)
	// Ensure that the cancel function is called when Collect exits to release resources.
	defer cancel()

	// Create a channel to receive errors from goroutines.
	errChan := make(chan error)

	// Create a channel to send file paths to be processed.
	filePaths := make(chan string)

	// Initialize a WaitGroup to synchronize goroutines.
	var wg sync.WaitGroup

	// Add one to the WaitGroup for the error-handling goroutine.
	wg.Add(1)
	go func() {
		defer wg.Done()
		// Wait for an error to be received or for the context to be done.
		select {
		case receivedErr := <-errChan:
			// If an error is received, assign it to the err variable and cancel the context.
			err = receivedErr
			cancel()
		case <-contextWithCancel.Done():
			// If the context is done, simply exit the goroutine.
		}
	}()

	// Initialize the transformation worker pool.
	poolTransform := workerpool.New[string, T]()
	// Start the transformation process which reads file paths, transforms them into type T, and sends them to transformedChan.
	transformedChan := poolTransform.Transform(contextWithCancel, conf.FileWorkers, filePaths, func(filePath string) T {
		var t T

		// Recover from any panic that occurs during transformation to prevent the goroutine from crashing.
		defer func() {
			if r := recover(); r != nil {
				// Attempt to send the error to errChan without blocking.
				select {
				case errChan <- fmt.Errorf("panic in Transform: %w", r.(error)):
				default:
				}
				// Cancel the context to signal all goroutines to stop.
				cancel()
			}
		}()

		// Open the file at the given filePath using the provided file system.
		file, err := fileSystem.Open(filePath)
		if err != nil {
			// If an error occurs while opening the file, panic to trigger the recovery above.
			panic(err)
		}
		// Ensure the file is closed when the function exits.
		defer file.Close()

		// Create a new JSON decoder for the file.
		decoder := json.NewDecoder(file)
		// Decode the JSON content into the variable t of type T.
		if err := decoder.Decode(&t); err != nil {
			// If decoding fails, panic to trigger the recovery above.
			panic(err)
		}
		// Return the decoded value.
		return t
	})

	// Initialize the accumulation worker pool.
	poolAccumulate := workerpool.New[T, R]()
	// Start the accumulation process which reads transformed data, accumulates it using the accumulator function, and sends results to accChan.
	accChan := poolAccumulate.Accumulate(contextWithCancel, conf.AccumulatorWorkers, transformedChan, accumulator)

	// Add one to the WaitGroup for the result-combining goroutine.
	wg.Add(1)
	go func() {
		defer wg.Done()
		// Ensure the context is canceled when this goroutine exits to signal other goroutines to stop.
		defer cancel()
		// Iterate over all accumulated results from accChan.
		for r := range accChan {
			// Combine each result with the current accumulated result using the combiner function.
			result = combiner(r, result)
		}
	}()

	// Initialize the listing worker pool.
	pool := workerpool.New[string, any]()
	// Start the directory traversal process which lists directories and sends file paths to filePaths channel.
	pool.List(contextWithCancel, conf.SearchWorkers, root, func(parent string) []string {
		// Recover from any panic that occurs during listing to prevent the goroutine from crashing.
		defer func() {
			if r := recover(); r != nil {
				// Attempt to send the error to errChan without blocking.
				select {
				case errChan <- fmt.Errorf("panic in List: %w", r.(error)):
				default:
				}
				// Cancel the context to signal all goroutines to stop.
				cancel()
			}
		}()

		// Read the contents of the parent directory using the provided file system.
		entries, err := fileSystem.ReadDir(parent)
		if err != nil {
			// If an error occurs while reading the directory, panic to trigger the recovery above.
			panic(err)
		}

		// Initialize a slice to hold subdirectory paths.
		var dirs []string
		// Iterate over each entry in the directory.
		for _, entry := range entries {
			// Construct the full path of the entry.
			fullPath := fileSystem.Join(parent, entry.Name())
			if entry.IsDir() {
				// If the entry is a directory, add it to the dirs slice for further traversal.
				dirs = append(dirs, fullPath)
			} else {
				// If the entry is a file, attempt to send its path to the filePaths channel.
				select {
				case <-contextWithCancel.Done():
					// If the context is done, exit the function to stop processing.
					return nil
				case filePaths <- fullPath:
					// Successfully sent the file path to be processed.
				}
			}
		}
		// Return the list of subdirectories to be traversed in the next layer.
		return dirs
	})
	// Close the filePaths channel to signal that no more file paths will be sent.
	close(filePaths)

	// Wait for all goroutines to finish processing.
	wg.Wait()

	// Check if the context was canceled due to an error that is not simply context.Canceled.
	if contextWithCancel.Err() != nil && !errors.Is(contextWithCancel.Err(), context.Canceled) {
		// Return the accumulated result along with the context error.
		return result, contextWithCancel.Err()
	}
	// Return the accumulated result and any error that was received.
	return result, err
}
