IN
POP AX
CALL :factorial
OUT
HLT

:factorial
    PUSH AX
    PUSH 1
    JNE :number_bigger_one
    PUSH 1
    RET

:number_bigger_one
    PUSH AX
    PUSH AX
    PUSH 1
    SUB
    POP AX
    CALL :factorial
    MUL
    RET
