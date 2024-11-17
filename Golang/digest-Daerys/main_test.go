package go_digest

import (
	"math"
	"math/rand/v2"
	"slices"
	"strings"
	"testing"
	"unsafe"

	"github.com/stretchr/testify/require"
)

func TestGetStringBySliceOfIndexesAlloc(t *testing.T) {
	result := testing.Benchmark(func(b *testing.B) {
		s := "1"
		ind := slices.Repeat([]int{0}, 1_000_000)
		b.ResetTimer()

		for i := 0; i < b.N; i++ {
			GetStringBySliceOfIndexes(s, ind)
		}
	})

	require.LessOrEqual(t, result.AllocsPerOp(), int64(2))
}

func TestGetStringBySliceOfIndexesBenchmark(t *testing.T) {
	result := testing.Benchmark(func(b *testing.B) {
		b.StopTimer()
		inputString := strings.Repeat("椅子摆得不整重新摆一下儿", 42*42*42*42)
		targetSlice := make([]int, 42*42)

		for i := 0; i < len(targetSlice); i++ {
			targetSlice[i] = 42
		}
		b.StartTimer()

		for i := 0; i < b.N; i++ {
			GetStringBySliceOfIndexes(inputString, targetSlice)
		}
	})

	require.Less(t, result.AllocsPerOp(), int64(127))
}

func TestGetStringBySliceOfIndexes(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name           string
		inputString    string
		indexes        []int
		expectedString string
	}{
		{
			name:           "simple get",
			inputString:    "abcdef",
			indexes:        []int{1, 3, 5},
			expectedString: "bdf",
		},
		{
			name:           "simple get random index order",
			inputString:    "abcdef",
			indexes:        []int{1, 3, 3, 3, 4, 2},
			expectedString: "bdddec",
		},
		{
			name:           "chinese symbols",
			inputString:    "椅子摆得不整重新摆一下儿",
			indexes:        []int{0, 1, 3, 5},
			expectedString: "椅子得整",
		},
		{
			name:           "arabic symbols",
			inputString:    "كورنييف جورج الكسندروفيتش",
			indexes:        []int{0, 8, 15},
			expectedString: "كجك",
		},
		{
			name:           "emoji",
			inputString:    "a🙂🙃🌚😑😐z",
			indexes:        []int{6, 3, 2, 5, 4, 4},
			expectedString: "z🌚🙃😐😑😑",
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			actual := GetStringBySliceOfIndexes(tc.inputString, tc.indexes)

			require.Equal(t, tc.expectedString, actual)
		})
	}
}

func TestCharByIndexCopy(t *testing.T) {
	result := testing.Benchmark(func(b *testing.B) {
		s := strings.Repeat("🙃", 10_000)
		b.ResetTimer()

		for i := 0; i < b.N; i++ {
			GetCharByIndex(s, i%10_000)
		}
	})

	require.EqualValues(t, 0, result.AllocsPerOp())
}

func TestGetCharByIndex(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name        string
		inputString string
		index       int
		expected    rune
	}{
		{
			name:        "simple get",
			inputString: "abcdef",
			index:       4,
			expected:    'e',
		},
		{
			name:        "chinese symbols",
			inputString: "椅子摆得不整重新摆一下儿",
			index:       4,
			expected:    '不',
		},
		{
			name:        "arabic symbols",
			inputString: "كورنييف جورج الكسندروفيتش",
			index:       8,
			expected:    'ج',
		},
		{
			name:        "emoji",
			inputString: "a🙂🙃🌚😑😐z",
			index:       3,
			expected:    '🌚',
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			actual := GetCharByIndex(tc.inputString, tc.index)

			require.Equal(t, tc.expected, actual)
		})
	}
}

func TestShiftPointer(t *testing.T) {
	t.Parallel()

	a := []int{42: 42}

	pointer := unsafe.SliceData(a)
	ShiftPointer(&pointer, int(unsafe.Sizeof(42))*42)

	require.Equal(t, 42, *pointer)
}

func TestSwapPointers(t *testing.T) {
	t.Parallel()

	a := 2
	b := 3

	SwapPointers(&a, &b)

	require.Equal(t, a, 3)
	require.Equal(t, b, 2)
}

func TestIsComplexEqual(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name     string
		a        complex128
		b        complex128
		expected bool
	}{
		{
			name:     "exactly equal",
			a:        complex(1, 2),
			b:        complex(1, 2),
			expected: true,
		},
		{
			name:     "pi equal with precision",
			a:        complex(2, math.Pi),
			b:        complex(2, 3.141592),
			expected: true,
		},
		{
			name:     "equal with precision",
			a:        complex(1, 2),
			b:        complex(1.000000001, 2.000000001),
			expected: true,
		},
		{
			name:     "not equal",
			a:        complex(1, 2),
			b:        complex(2, 1),
			expected: false,
		},
		{
			name:     "not equal low precision",
			a:        complex(1, 2),
			b:        complex(1.0001, 2.0001),
			expected: false,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			actual := IsComplexEqual(tc.a, tc.b)

			require.Equal(t, tc.expected, actual)
		})
	}
}

func TestGetRootsOfQuadraticEquation(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name  string
		a     float64
		b     float64
		c     float64
		rootA complex128
		rootB complex128
	}{
		{
			name:  "simple",
			a:     1.0,
			b:     -3.0,
			c:     2.0,
			rootA: complex(1, 0),
			rootB: complex(2, 0),
		},
		{
			name:  "single root",
			a:     1.0,
			b:     -2.0,
			c:     1.0,
			rootA: complex(1, 0),
			rootB: complex(1, 0),
		},
		{
			name:  "complex roots integer",
			a:     1.0,
			b:     0.0,
			c:     1.0,
			rootA: complex(0, -1),
			rootB: complex(0, 1),
		},
		{
			name:  "complex roots floats",
			a:     1.0,
			b:     1.0,
			c:     1.0,
			rootA: complex(-0.5, 0.8660254037844386),
			rootB: complex(-0.5, -0.8660254037844386),
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			actRootA, actRootB := GetRootsOfQuadraticEquation(tc.a, tc.b, tc.c)

			if !(IsComplexEqual(actRootA, tc.rootA) && IsComplexEqual(actRootB, tc.rootB) ||
				IsComplexEqual(actRootA, tc.rootB) && IsComplexEqual(actRootB, tc.rootA)) {
				t.Errorf("GetRootsOfQuadraticEquation is bad, want (%v, %v) got (%v, %v)", tc.rootA, tc.rootB, actRootA, actRootB)
			}
		})
	}
}

func TestSort(t *testing.T) {
	t.Parallel()

	t.Run("simple sort", func(t *testing.T) {
		t.Parallel()

		s := []int{2, 1, 3, 0}
		want := []int{0, 1, 2, 3}
		Sort(s)

		require.True(t, slices.Equal(want, s))
	})

	t.Run("random elements sort", func(t *testing.T) {
		t.Parallel()

		s := make([]int, 1_000_000)
		want := make([]int, 1_000_000)

		for i := 0; i < len(s); i++ {
			s[i] = math.MaxInt - rand.N[int](len(s))
			want[i] = s[i]
		}

		Sort(s)
		slices.Sort(want)

		require.Equal(t, want, s)
	})
}

func TestSortPerformance(t *testing.T) {
	goSort := testing.Benchmark(func(b *testing.B) {
		b.StopTimer()

		s := make([]int, 100_000)
		for i := len(s) - 1; i >= 0; i-- {
			s[i] = i
		}

		b.StartTimer()

		for i := 0; i < b.N; i++ {
			slices.Sort(s)
		}
	})

	studentSort := testing.Benchmark(func(b *testing.B) {
		b.StopTimer()

		s := make([]int, 100_000)
		for i := len(s) - 1; i >= 0; i-- {
			s[i] = i
		}

		b.StartTimer()

		for i := 0; i < b.N; i++ {
			Sort(s)
		}

		b.ReportAllocs()
	})

	require.LessOrEqual(t, studentSort.AllocsPerOp(), int64(1))
	require.LessOrEqual(t, float64(studentSort.NsPerOp())/float64(goSort.NsPerOp()), 50.0)
}

func TestReverseSliceOne(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name     string
		slice    []int
		expected []int
	}{
		{
			name:     "nil",
			slice:    nil,
			expected: nil,
		},
		{
			name:     "empty",
			slice:    []int{},
			expected: []int{},
		},
		{
			name:     "simple reverse",
			slice:    []int{1, 2, 3, 4, 5},
			expected: []int{5, 4, 3, 2, 1},
		},
		{
			name:     "odd size",
			slice:    []int{1, 2, 3, 4, 5},
			expected: []int{5, 4, 3, 2, 1},
		},
		{
			name:     "even size",
			slice:    []int{1, 2, 3, 4},
			expected: []int{4, 3, 2, 1},
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			ReverseSliceOne(tc.slice)

			require.Equal(t, tc.expected, tc.slice)
		})
	}
}

func TestReverseSliceTwo(t *testing.T) {
	t.Parallel()

	t.Run("simple tests", func(t *testing.T) {
		t.Parallel()

		testCases := []struct {
			name     string
			slice    []int
			expected []int
		}{
			{
				name:     "nil",
				slice:    nil,
				expected: []int{},
			},
			{
				name:     "empty",
				slice:    []int{},
				expected: []int{},
			},
			{
				name:     "odd size",
				slice:    []int{1, 2, 3, 4, 5},
				expected: []int{5, 4, 3, 2, 1},
			},
			{
				name:     "even size",
				slice:    []int{1, 2, 3, 4},
				expected: []int{4, 3, 2, 1},
			},
		}

		for _, tc := range testCases {
			t.Run(tc.name, func(t *testing.T) {
				t.Parallel()

				actual := ReverseSliceTwo(tc.slice)

				require.Equal(t, tc.expected, actual)
			})
		}
	})

	t.Run("immutability check", func(t *testing.T) {
		t.Parallel()

		s := []int{1, 2, 3, 4}
		sCopy := make([]int, len(s))
		copy(sCopy, s)
		actual := ReverseSliceTwo(s)
		want := []int{4, 3, 2, 1}

		require.Equal(t, want, actual)

		for i := 0; i < len(actual); i++ {
			actual[i] = i + 42
		}

		require.Equal(t, sCopy, s, "ReverseSliceTwo is bad, original slice is modified")
	})
}

func TestSwapPointersSimple(t *testing.T) {
	t.Parallel()

	a, b := 1, 2
	SwapPointers(&a, &b)
	wantA, wantB := 2, 1

	require.Equal(t, wantA, a)
	require.Equal(t, wantB, b)
}

func TestIsSliceEqual(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name     string
		a        []int
		b        []int
		expected bool
	}{
		{
			name:     "empty slice",
			a:        []int{},
			b:        []int{},
			expected: true,
		},
		{
			name:     "equal slices",
			a:        []int{1, 2, 3},
			b:        []int{1, 2, 3},
			expected: true,
		},
		{
			name:     "not equal slices",
			a:        []int{1, 2, 3},
			b:        []int{1, 4, 3},
			expected: false,
		},
		{
			name:     "different length",
			a:        []int{1, 2, 3},
			b:        []int{1, 2},
			expected: false,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			actual := IsSliceEqual(tc.a, tc.b)

			require.Equal(t, tc.expected, actual)
		})
	}
}

func TestDeleteByIndex(t *testing.T) {
	t.Parallel()

	t.Run("simple tests", func(t *testing.T) {
		t.Parallel()

		testCases := []struct {
			name     string
			slice    []int
			index    int
			expected []int
		}{
			{
				name:     "middle element",
				slice:    []int{0, 1, 2, 3, 4, 5, 6, 7, 8},
				index:    3,
				expected: []int{0, 1, 2, 4, 5, 6, 7, 8},
			},
			{
				name:     "first element",
				slice:    []int{0, 1, 2, 3, 4, 5, 6, 7, 8},
				index:    0,
				expected: []int{1, 2, 3, 4, 5, 6, 7, 8},
			},
			{
				name:     "last element",
				slice:    []int{0, 1, 2, 3, 4, 5, 6, 7, 8},
				index:    8,
				expected: []int{0, 1, 2, 3, 4, 5, 6, 7},
			},
		}

		for _, tc := range testCases {
			t.Run(tc.name, func(t *testing.T) {
				t.Parallel()

				actual := DeleteByIndex(tc.slice, tc.index)

				require.Equal(t, tc.expected, actual)
			})
		}
	})

	t.Run("immutability check", func(t *testing.T) {
		t.Parallel()

		s, idx := []int{0, 1, 2, 3, 4, 5, 6, 7, 8}, 8

		temp := make([]int, len(s))
		copy(temp, s)

		got := DeleteByIndex(s, idx)
		want := []int{0, 1, 2, 3, 4, 5, 6, 7}

		require.Equal(t, want, got)

		for i := 0; i < len(got); i++ {
			got[i] = 99
		}

		require.Equal(t, temp, s)
	})
}
