#include "textflag.h"

// func SumSlice(s []int32) int64
TEXT ·SumSlice(SB), NOSPLIT, $0-32
    MOVQ s_base+0(FP), AX   // AX = base address of slice
    MOVQ s_len+8(FP), CX    // CX = length of slice
    XORQ DX, DX             // DX = 0, sum accumulator

loop:
    MOVLQSX (AX), BX        // BX = s[i] (curr slice's element, 32 бита)
    ADDQ BX, DX             // DX += BX

    ADDQ $4, AX             // Increment pointer to 4 bytes (int32)

    DECQ CX                 // CX -= 1
    JNZ loop                // if CX != 0, continue цикл

end:
    MOVQ DX, ret+24(FP)     // ret = sum (DX)
    RET

#define s_base 0(FP)
#define s_len 8(FP)
#define s_cap 16(FP)
#define ret 24(FP)
