package fact

import (
	"errors"
	"fmt"
	"io"
	"strconv"
	"strings"
	"sync"
)

var (
	// ErrFactorizationCancelled is returned when the factorization process is cancelled via the done channel.
	ErrFactorizationCancelled = errors.New("cancelled")

	// ErrWriterInteraction is returned if an error occurs while interacting with the writer
	// triggering early termination.
	ErrWriterInteraction = errors.New("writer interaction")
)

// Config defines the configuration for factorization and write workers.
type Config struct {
	FactorizationWorkers int
	WriteWorkers         int
}

// Factorization interface represents a concurrent prime factorization task with configurable workers.
// Thread safety and error handling are implemented as follows:
// - The provided writer must be thread-safe to handle concurrent writes from multiple workers.
// - Output uses '\n' for newlines.
// - Factorization has a time complexity of O(sqrt(n)) per number.
// - If an error occurs while writing to the writer, early termination is triggered across all workers.
type Factorization interface {
	// Do performs factorization on a list of integers, writing the results to an io.Writer.
	// - done: a channel to signal early termination.
	// - numbers: the list of integers to factorize.
	// - writer: the io.Writer where factorization results are output.
	// - config: optional worker configuration.
	// Returns an error if the process is cancelled or if a writer error occurs.
	Do(done <-chan struct{}, numbers []int, writer io.Writer, config ...Config) error
}

type factorizationImpl struct{}

// New creates a new instance of Factorization.
func New() Factorization {
	return &factorizationImpl{}
}

type factorizationResult struct {
	number  int
	factors []int
}

// Do performs concurrent factorization of the given numbers and writes the results to the writer.
// It uses a pool of factorization workers and write workers as per the provided configuration.
func (f *factorizationImpl) Do(done <-chan struct{}, numbers []int, writer io.Writer, config ...Config) error {
	cfg, err := parseConfig(config)
	if err != nil {
		return err
	}

	numbersChan := make(chan int)
	resultsChan := make(chan factorizationResult)
	errChan := make(chan error, 1) // Buffered channel to store a single error
	stop := make(chan struct{})
	var stopOnce sync.Once
	cancel := func() {
		stopOnce.Do(func() {
			close(stop)
		})
	}

	var writeWG sync.WaitGroup
	startWriteWorkers(&writeWG, cfg.WriteWorkers, done, stop, resultsChan, writer, errChan, cancel)

	var factorizationWG sync.WaitGroup
	startFactorizationWorkers(&factorizationWG, cfg.FactorizationWorkers, done, stop, numbersChan, resultsChan)

	feedNumbers(done, stop, numbers, numbersChan, cancel)

	// Wait for all factorization workers to finish
	factorizationWG.Wait()
	close(resultsChan) // Close resultsChan after all factorization results have been sent

	// Wait for all write workers to finish
	writeWG.Wait()

	// Check for any errors that occurred during processing
	if err := checkErrors(errChan, done); err != nil {
		return err
	}

	return nil
}

// parseConfig parses the optional configuration provided to the Do function.
// It sets default values if no configuration is provided and validates the worker counts.
func parseConfig(config []Config) (Config, error) {
	// Initialize default configuration with 1 worker for factorization and writing
	cfg := Config{
		FactorizationWorkers: 1,
		WriteWorkers:         1,
	}
	// If a custom configuration is provided, use it
	if len(config) > 0 {
		cfg = config[0]
	}

	// Validate that the number of workers is positive
	if cfg.FactorizationWorkers <= 0 || cfg.WriteWorkers <= 0 {
		return Config{}, errors.New("invalid worker count in config")
	}
	return cfg, nil
}

// startWriteWorkers starts a specified number of write worker goroutines.
// Each worker reads factorization results from resultsChan and writes them to the writer.
// If an error occurs during writing, it sends an error to errChan and calls cancel().
func startWriteWorkers(
	wg *sync.WaitGroup,
	numWorkers int,
	done,
	stop <-chan struct{},
	resultsChan <-chan factorizationResult,
	writer io.Writer,
	errChan chan<- error,
	cancel func()) {

	for i := 0; i < numWorkers; i++ {
		wg.Add(1)
		go writeWorker(wg, done, stop, resultsChan, writer, errChan, cancel)
	}
}

// writeWorker is a goroutine that writes factorization results to the writer.
// It listens to the resultsChan and writes each result.
// If writing fails, it reports the error and initiates cancellation.
func writeWorker(
	wg *sync.WaitGroup,
	done,
	stop <-chan struct{},
	resultsChan <-chan factorizationResult,
	writer io.Writer,
	errChan chan<- error,
	cancel func()) {

	defer wg.Done()
	for {
		select {
		case <-done:
			// Received cancellation signal
			return
		case <-stop:
			// Stop signal received due to an error elsewhere
			return
		case res, ok := <-resultsChan:
			if !ok {
				// Channel closed, no more results
				return
			}
			line := formatResult(res)
			_, err := writer.Write([]byte(line + "\n"))
			if err != nil {
				// Error occurred during writing; report it and cancel processing
				select {
				case errChan <- ErrWriterInteraction:
				default:
				}
				cancel()
				return
			}
		}
	}
}

// startFactorizationWorkers starts a specified number of factorization worker goroutines.
// Each worker reads numbers from numbersChan, factorizes them, and sends the results to resultsChan.
func startFactorizationWorkers(
	wg *sync.WaitGroup,
	numWorkers int,
	done,
	stop <-chan struct{},
	numbersChan <-chan int,
	resultsChan chan<- factorizationResult) {

	for i := 0; i < numWorkers; i++ {
		wg.Add(1)
		go factorizationWorker(wg, done, stop, numbersChan, resultsChan)
	}
}

// factorizationWorker is a goroutine that performs prime factorization on numbers.
// It reads numbers from numbersChan, computes their factors, and sends the results to resultsChan.
func factorizationWorker(
	wg *sync.WaitGroup,
	done,
	stop <-chan struct{},
	numbersChan <-chan int,
	resultsChan chan<- factorizationResult) {

	defer wg.Done()
	for {
		select {
		case <-done:
			// Received cancellation signal
			return
		case <-stop:
			// Stop signal received due to an error elsewhere
			return
		case n, ok := <-numbersChan:
			if !ok {
				// Channel closed, no more numbers to process
				return
			}
			// Perform factorization
			factors := factorizeNumber(n)
			res := factorizationResult{number: n, factors: factors}
			select {
			case <-done:
				// Received cancellation signal
				return
			case <-stop:
				// Stop signal received due to an error elsewhere
				return
			case resultsChan <- res:
				// Successfully sent result to resultsChan
			}
		}
	}
}

// feedNumbers sends the list of numbers to the numbersChan for processing.
// It monitors for cancellation or stop signals and closes the channel when done.
func feedNumbers(done, stop <-chan struct{}, numbers []int, numbersChan chan<- int, cancel func()) {
	defer close(numbersChan)
	for _, n := range numbers {
		select {
		case <-done:
			// Received cancellation signal; initiate cancellation
			cancel()
			return
		case <-stop:
			// Stop signal received due to an error elsewhere
			return
		case numbersChan <- n:
			// Successfully sent number to numbersChan
		}
	}
}

// checkErrors checks for any errors that occurred during processing or if a cancellation signal was received.
// It returns the appropriate error if any.
func checkErrors(errChan <-chan error, done <-chan struct{}) error {
	select {
	case err := <-errChan:
		// An error occurred during processing
		return err
	default:
	}

	select {
	case <-done:
		// Received cancellation signal
		return ErrFactorizationCancelled
	default:
	}

	// No errors or cancellation; processing completed successfully
	return nil
}

// factorizeNumber performs prime factorization on the given integer n.
// It returns a slice of factors in ascending order, handling special cases for 0, 1, and negative numbers.
func factorizeNumber(n int) []int {
	if n == 0 {
		// Special case: 0
		return []int{0}
	}
	var factors []int
	if n < 0 {
		// Include -1 as a factor for negative numbers
		factors = append(factors, -1)
		n = -n
	}
	if n == 1 {
		// Special case: 1
		factors = append(factors, 1)
		return factors
	}
	// Factorization using trial division up to sqrt(n)
	for i := 2; i*i <= n; i++ {
		for n%i == 0 {
			factors = append(factors, i)
			n /= i
		}
	}
	if n > 1 {
		// Remaining prime factor
		factors = append(factors, n)
	}
	return factors
}

// formatResult formats the factorization result into a string in the required format.
// Example: "100 = 2 * 2 * 5 * 5"
func formatResult(res factorizationResult) string {
	// Convert factors to strings
	factorsStr := make([]string, len(res.factors))
	for i, factor := range res.factors {
		factorsStr[i] = strconv.Itoa(factor)
	}
	// Join factors with ' * ' and format the result
	return fmt.Sprintf("%d = %s", res.number, strings.Join(factorsStr, " * "))
}
