; ============================================================
;  Quadratic / linear equation solver
;  Input:  a, b, c
; ============================================================

IN
POP AX              ; AX = a
IN
POP BX              ; BX = b
IN
POP CX              ; CX = c

; ============================================================
;  if a == 0 -> linear
; ============================================================
PUSH AX
PUSH 0
JE :linear

; ============================================================
;  Quadratic:  D = b*b - 4*a*c
; ============================================================
PUSH 4
PUSH AX
MUL                 ; [4a]
PUSH CX
MUL                 ; [4ac]
POP [4]             ; RAM[4] = 4ac

PUSH BX
PUSH BX
MUL                 ; [b*b]

PUSH [4]
SUB                 ; [b*b - 4ac]
POP DX              ; DX = D

; ============================================================
;  if D < 0 -> no real roots
; ============================================================
PUSH DX
PUSH 0
JB :no_roots

; ============================================================
;  if D == 0 -> one root
; ============================================================
PUSH DX
PUSH 0
JE :one_root_quad

; ============================================================
;  D > 0 -> two roots
; ============================================================
PUSH DX
SQRT
POP [0]             ; RAM[0] = sqrt(D)

PUSH 2
PUSH AX
MUL
POP [1]             ; RAM[1] = 2a

; x1 = (-b + sqrtD) / 2a
PUSH 0
PUSH BX
SUB
PUSH [0]
ADD
PUSH [1]
DIV
POP [2]             ; RAM[2] = x1

; x2 = (-b - sqrtD) / 2a
PUSH 0
PUSH BX
SUB
PUSH [0]
SUB
PUSH [1]
DIV
POP [3]             ; RAM[3] = x2

; --- output "x1 = " ---
PUSH 120            ; 'x'
POP [10]
PUSH 49             ; '1'
POP [11]
PUSH 32             ; ' '
POP [12]
PUSH 61             ; '='
POP [13]
PUSH 32             ; ' '
POP [14]

; --- output x1 ---
PUSH [2]
PUSH 1
DIV
PUSH 48             ; '0'
ADD
POP [15]

; --- output ", x2 = " ---
PUSH 44             ; ','
POP [16]
PUSH 32             ; ' '
POP [17]
PUSH 120            ; 'x'
POP [18]
PUSH 50             ; '2'
POP [19]
PUSH 32             ; ' '
POP [20]
PUSH 61             ; '='
POP [21]
PUSH 32             ; ' '
POP [22]

; --- output x2 ---
PUSH [3]
PUSH 1
DIV
PUSH 48             ; '0'
ADD
POP [23]

PUSH 0
POP [24]
PRINT
HLT

; ============================================================
;  :no_roots
; ============================================================
:no_roots
    PUSH 110            ; 'n'
    POP [10]
    PUSH 111            ; 'o'
    POP [11]
    PUSH 32             ; ' '
    POP [12]
    PUSH 114            ; 'r'
    POP [13]
    PUSH 111            ; 'o'
    POP [14]
    PUSH 111            ; 'o'
    POP [15]
    PUSH 116            ; 't'
    POP [16]
    PUSH 115            ; 's'
    POP [17]
    PUSH 0
    POP [18]
    PRINT
    HLT

; ============================================================
;  :one_root_quad
; ============================================================
:one_root_quad
    PUSH 2
    PUSH AX
    MUL
    POP [1]             ; RAM[1] = 2a

    PUSH 0
    PUSH BX
    SUB
    PUSH [1]
    DIV
    POP [2]             ; RAM[2] = x

; --- output "x = " ---
PUSH 120            ; 'x'
POP [10]
PUSH 32             ; ' '
POP [11]
PUSH 61             ; '='
POP [12]
PUSH 32             ; ' '
POP [13]

PUSH [2]
PUSH 1
DIV
PUSH 48             ; '0'
ADD
POP [14]

PUSH 0
POP [15]
PRINT
HLT

; ============================================================
;  :linear
; ============================================================
:linear
    PUSH BX
    PUSH 0
    JE :linear_degenerate

    PUSH 0
    PUSH CX
    SUB
    PUSH BX
    DIV
    POP [2]             ; RAM[2] = x

    ; --- output "x = " ---
    PUSH 120            ; 'x'
    POP [10]
    PUSH 32             ; ' '
    POP [11]
    PUSH 61             ; '='
    POP [12]
    PUSH 32             ; ' '
    POP [13]

    PUSH [2]
    PUSH 1
    DIV
    PUSH 48             ; '0'
    ADD
    POP [14]

    PUSH 0
    POP [15]
    PRINT
    HLT

; ============================================================
;  :linear_degenerate
; ============================================================
:linear_degenerate
    PUSH CX
    PUSH 0
    JE :inf_solutions

    ; --- output "no sol" ---
    PUSH 110            ; 'n'
    POP [10]
    PUSH 111            ; 'o'
    POP [11]
    PUSH 32             ; ' '
    POP [12]
    PUSH 115            ; 's'
    POP [13]
    PUSH 111            ; 'o'
    POP [14]
    PUSH 108            ; 'l'
    POP [15]
    PUSH 0
    POP [16]
    PRINT
    HLT

:inf_solutions
; --- output "inf" ---
    PUSH 105            ; 'i'
    POP [10]
    PUSH 110            ; 'n'
    POP [11]
    PUSH 102            ; 'f'
    POP [12]
    PUSH 0
    POP [13]
    PRINT
    HLT
