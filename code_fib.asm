; ============================================================
;  Fibonacci  F(n)
;  Input:     n >= 0
;  Output:    F(n)
;
;  Registers:
;     AX = a (F(k))
;     BX = b (F(k+1))
;     CX = k
;
;  RAM:
;     RAM[2] = n  (kept here so DX can be reused as temp)
; ============================================================

; ----------- main -------------
IN                  ; stack: [n]
POP DX              ; DX = n
PUSH DX
POP [2]             ; RAM[2] = n

; ---------- if (n == 0): print 0 ----------
PUSH 0
PUSH DX
JE :case_zero       ; JE pops both and compares 0 == n

; ---------- if (n == 1): print 1 ----------
PUSH 1
PUSH DX
JE :case_one        ; 1 == n ?

; -------- if (n >= 2): initialize --------
PUSH 0
POP AX              ; AX = 0   (a = F(0))
PUSH 1
POP BX              ; BX = 1   (b = F(1))
PUSH 1
POP CX              ; CX = 1   (k = 1)

; ---------- while k < n ----------
:loop
    PUSH CX            ; stack: [k]
    PUSH [2]           ; stack: [k, n]   (value2 = k, value1 = n)
    JB :body           ; if k < n -> body
    JMP :print_b       ; else (k >= n) -> print b

:body
    ; c = a + b  -> DX
    PUSH AX            ; stack: [a]
    PUSH BX            ; stack: [a, b]
    ADD                ; stack: [a + b]
    POP DX             ; DX = a + b

    ; a = b
    PUSH BX
    POP AX              ; AX = b

    ; b = c
    PUSH DX
    POP BX              ; BX = c

    ; k = k + 1
    PUSH 1
    PUSH CX
    ADD                 ; stack: [k+1]
    POP CX              ; CX = k + 1

    JMP :loop

; ---------- output F(n) ----------
:print_b
    PUSH BX
    OUT                 ; OUT prints top of stack but does NOT pop
    POP AX              ; clean the stack after OUT
    HLT

:case_zero
    PUSH 0
    OUT
    POP AX              ; clean the stack after OUT
    HLT

:case_one
    PUSH 1
    OUT
    POP AX              ; clean the stack after OUT
    HLT
