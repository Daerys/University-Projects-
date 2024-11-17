#include "textflag.h"

// func LowerBound(slice []int64, value int64) int64
TEXT ·LowerBound(SB), NOSPLIT, $0-40
    MOVQ slice_base+0(FP), SI  // SI = base address of slice (int64)
    MOVQ slice_len+8(FP), CX   // CX = length of slice
    MOVQ value+24(FP), DX      // DX = value to find

    XORQ AX, AX
    LEAQ -1(CX), BX
    MOVQ $-1, R10

binary_search:
    CMPQ AX, BX
    JG end

    // mid = (high + low) / 2
    MOVQ BX, DI                // DI = high
    ADDQ AX, DI                // DI = high + low
    SHRQ $1, DI                // DI = (high + low) / 2

    MOVQ (SI)(DI*8), R8        // R8 = slice[mid]

    CMPQ R8, DX
    JLE update_low
    LEAQ -1(DI), BX
    JMP binary_search

update_low:
    MOVQ DI, R10
    LEAQ 1(DI), AX
    JMP binary_search

end:
    MOVQ R10, ret+32(FP)
    RET

#define slice_base 0(FP)
#define slice_len  8(FP)
#define slice_cap  16(FP)
#define value      24(FP)
#define ret        32(FP)
