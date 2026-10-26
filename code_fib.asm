IN
CALL :fibonachi
OUT
HLT

:fibonachi
    POP AX

    PUSH AX
    PUSH 0
    JE :fibonachi_0

    PUSH AX
    PUSH 1
    JE :fibonachi_1

    PUSH AX
    PUSH AX
    PUSH 1
    SUB
    CALL :fibonachi

    POP BX
    POP AX
    PUSH BX
    PUSH AX
    PUSH 2
    SUB
    CALL :fibonachi

    ADD
    RET

:fibonachi_0
    PUSH 0
    RET

:fibonachi_1
    PUSH 1
    RET
