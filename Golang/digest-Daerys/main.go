package go_digest

import (
	"math/cmplx"
	"strings"
	"unsafe"
)

// GetCharByIndex returns the i-th character from the given string.
func GetCharByIndex(str string, idx int) rune {
	for _, r := range str {
		if idx == 0 {
			return r
		}
		idx--
	}
	return 0
}

// GetStringBySliceOfIndexes returns a string formed by concatenating specific characters from the input string based
// on the provided indexes.
func GetStringBySliceOfIndexes(str string, indexes []int) string {
	var result strings.Builder
	runes := []rune(str)
	result.Grow(len(indexes))

	for _, idx := range indexes {
		result.WriteRune(runes[idx])
	}
	return result.String()
}

// ShiftPointer shifts the given pointer by the specified number of bytes using unsafe.Add.
func ShiftPointer(pointer **int, shift int) {
	*pointer = (*int)(unsafe.Add(unsafe.Pointer(*pointer), shift))
}

// IsComplexEqual compares two complex numbers and determines if they are equal.
func IsComplexEqual(a, b complex128) bool {
	const epsilon = 1e-6
	return cmplx.Abs(a-b) < epsilon
}

// GetRootsOfQuadraticEquation returns two roots of a quadratic equation ax^2 + bx + c = 0.
func GetRootsOfQuadraticEquation(a, b, c float64) (complex128, complex128) {
	complexA := complex(a, 0)
	complexB := complex(b, 0)
	discriminant := cmplx.Sqrt(complex(b*b-4*a*c, 0))
	return (-complexB + discriminant) / (2 * complexA), (-complexB - discriminant) / (2 * complexA)
}

// Sort sorts in-place the given slice of integers in ascending order using an optimized Merge Sort.
func Sort(source []int) {
	if len(source) <= 1 {
		return
	}
	aux := make([]int, len(source))
	mergeSort(source, aux)
}

func mergeSort(arr, aux []int) {
	n := len(arr)
	for width := 1; width < n; width *= 2 {
		for i := 0; i < n; i += 2 * width {
			mid := i + width
			if mid > n {
				mid = n
			}
			end := i + 2*width
			if end > n {
				end = n
			}
			merge(arr, aux, i, mid, end)
		}
	}
}

func merge(arr, aux []int, low, mid, high int) {
	copy(aux[low:high], arr[low:high])

	i, j := low, mid

	for k := low; k < high; k++ {
		if i < mid && (j >= high || aux[i] <= aux[j]) {
			arr[k] = aux[i]
			i++
		} else {
			arr[k] = aux[j]
			j++
		}
	}
}

// ReverseSliceOne in-place reverses the order of elements in the given slice.
func ReverseSliceOne(s []int) {
	for i, j := 0, len(s)-1; i < j; i, j = i+1, j-1 {
		s[i], s[j] = s[j], s[i]
	}
}

// ReverseSliceTwo returns a new slice of integers with elements in reverse order compared to the input slice.
// The original slice remains unmodified.
func ReverseSliceTwo(s []int) []int {
	result := make([]int, len(s))
	for i := 0; i < len(s); i++ {
		result[i] = s[len(s)-1-i]
	}
	return result
}

// SwapPointers swaps the values of two pointers.
func SwapPointers(a, b *int) {
	*a, *b = *b, *a
}

// IsSliceEqual compares two slices of integers and returns true if they contain the same elements in the same order.
func IsSliceEqual(a, b []int) bool {
	if len(a) != len(b) {
		return false
	}
	for i := range a {
		if a[i] != b[i] {
			return false
		}
	}
	return true
}

// DeleteByIndex deletes the element at the specified index from the slice and returns a new slice.
// The original slice remains unmodified.
func DeleteByIndex(s []int, idx int) []int {
	if idx < 0 || idx >= len(s) {
		result := make([]int, len(s))
		copy(result, s)
		return result
	}
	result := make([]int, len(s)-1)
	copy(result, s[:idx])
	copy(result[idx:], s[idx+1:])
	return result
}
