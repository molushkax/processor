; ===========================================================
;  Draw_circle
;  Input: 0 < R < SIZE_OF_SIDE
;  Output: green slime
;
;  Registers:
;     AX = current value of Y
;     BX = current value of X
;     CX = current value of cell in circle
;     DX = r
;
;  RAM:
;     RAM[CX] = 1 - cells, that are in circle
; ==========================================================

; --------------- main ---------------
IN
POP DX                ; DX = r

PUSH 0
POP AX                ; AX = Y (0 - 38)
CALL :loop_y

DRAW
HLT

; ----------- loop for Y ------------
:loop_y
    PUSH AX
    PUSH 39
    JB :body_y
    RET

:body_y
    PUSH 0
    POP BX            ; BX = X (0 - 38)

    CALL :loop_x      ; start loop for X

    PUSH AX
    PUSH 1
    ADD
    POP AX            ; AX += 1
    JMP :loop_y

; ------------ loop for X ------------
:loop_x
    PUSH BX
    PUSH 39
    JB :body_x
    RET

:body_x
    CALL :check

    PUSH BX
    PUSH 1
    ADD
    POP BX            ; BX += 1
    JMP :loop_x

; --- check condition (X - 19)^2 (Y - 19)^2 <= R^2 ---
:check
    PUSH BX
    PUSH 19
    SUB
    PUSH BX
    PUSH 19
    SUB
    MUL               ; (X - 19)^2

    PUSH AX
    PUSH 19
    SUB
    PUSH AX
    PUSH 19
    SUB
    MUL               ; (Y - 19)^2

    ADD

    PUSH DX
    PUSH DX
    MUL

    JBE :draw         ; if in circle
    RET

; --------- fill RAM in "circle" cells --------
:draw
    ; address = Y * 39 + X
    PUSH AX
    PUSH 39
    MUL
    PUSH BX
    ADD
    POP CX            ; CX = address of current "circle" cell

    PUSH 1
    POP [CX]          ; fill RAM[CX] with 1
    RET
