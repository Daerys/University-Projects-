#include "textflag.h"


// func Fibonacci(n uint64) uint64
TEXT ·Fibonacci(SB), NOSPLIT, $0-16
    MOVQ n+0(FP), AX

    CMPQ AX, $0
    JEQ zero_case
    CMPQ AX, $1
    JEQ one_case

    XORQ BX, BX
    MOVQ $1, CX

loop:
    SUBQ $1, AX
    JZ end_loop


    MOVQ CX, DX
    ADDQ BX, CX
    MOVQ DX, BX

    JMP loop

end_loop:
    MOVQ CX, ret+8(FP)
    RET

zero_case:
    MOVQ $0, ret+8(FP)
    RET

one_case:
    MOVQ $1, ret+8(FP)
    RET

#define n 0(FP)
#define ret 8(FP)
