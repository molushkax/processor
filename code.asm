; --- 1. Reading coefficients ---
IN
POP AX      ; AX = A
IN
POP BX      ; BX = B
IN
POP CX      ; CX = C

; --- 2. Calculating Discriminant D = B^2 - 4AC ---
PUSH BX
PUSH BX
MUL
POP DX      ; DX = B^2

PUSH 4
PUSH AX
MUL
PUSH CX
MUL
POP CX      ; CX = 4AC

PUSH DX
PUSH CX
SUB         ; D = B^2 - 4AC (DX - CX)
POP DX      ; DX = D

; --- 3. Checking D ---
PUSH 0
PUSH DX
JE :one_root

PUSH 0
PUSH DX
JB :no_roots    ; If 0 < D, means D < 0 (signed check)

; --- 4. Two roots (D > 0) ---
; Calculate denominator 2A
PUSH 2
PUSH AX
MUL
POP CX      ; CX = 2A

; x1 = (-B + sqrt(D)) / 2A
PUSH 0
PUSH BX
SUB         ; 0 - BX = -B
PUSH DX
SQRT        ; sqrt(D)
ADD         ; -B + sqrt(D)
PUSH CX
DIV         ; (-B + sqrt(D)) / 2A
OUT

; x2 = (-B - sqrt(D)) / 2A
PUSH 0
PUSH BX
SUB         ; -B
PUSH DX
SQRT        ; sqrt(D)
SUB         ; -B - sqrt(D)
PUSH CX
DIV
OUT
JMP :end

:one_root
PUSH 2
PUSH AX
MUL
POP CX      ; CX = 2A

PUSH 0
PUSH BX
SUB         ; -B
PUSH CX
DIV         ; -B / 2A
OUT
JMP :end

:no_roots
PUSH 0
OUT

:end
HLT
