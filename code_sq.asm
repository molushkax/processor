;--- ВВОД КОЭФФИЦИЕНТОВ ---
IN          ;ввели A
POP AX      ;A в AX

IN          ;ввели B
POP BX      ;B в BX

IN          ;ввели C
POP CX      ;C в CX

;--- ВЫЧИСЛЕНИЕ ДИСКРИМИНАНТА: D = B^2 - 4AC ---
;1. B * B
PUSH BX    ;кладем B
PUSH BX    ;кладем B еще раз (теперь стек [B, B])
MUL        ;перемножаем, получаем B^2 на стеке

;2. 4 * A * C
PUSH 4
PUSH AX    ;A
MUL        ;4 * A
PUSH CX    ;C
MUL        ;4 * A * C

;3. B^2 - 4AC
SUB         ;(B^2) - (4AC)
POP DX      ;сохраняем дискриминант в DX

;--- ПРОВЕРКА НА ЛИНЕЙНОЕ УРАВНЕНИЕ (A == 0) ---
PUSH 0
PUSH AX    ;кладем A
JE :5      ;если A == 0, это линейное уравнение

;--- ПРОВЕРКА ДИСКРИМИНАНТА (D < 0) ---
PUSH 0
PUSH DX    ;кладем D
JB :6      ;если 0 < D (то есть D < 0), корней нет

;--- ВЫЧИСЛЕНИЕ КОРНЕЙ (D >= 0) ---
;x = (-B + sqrt(D)) / 2A

;Считаем -B
PUSH 0
PUSH BX
SUB         ;0 - B = -B

//TODO sqrt
PUSH DX     ; sqrt nado
ADD         ;-B + D

;Делим на 2A
PUSH 2
PUSH AX
MUL         ;2A
DIV         ;(-B + D) / (2A)

OUT         ;печатаем первый корень
JMP :7

:5
    ;--- РЕШЕНИЕ ЛИНЕЙНОГО УРАВНЕНИЯ (BX * x + CX = 0) ---
    ;x = -C / B
    PUSH 0
    PUSH CX
    SUB         ;-C
    PUSH BX     ;B
    DIV         ;-C / B
    OUT
    JMP :7

:6
    PUSH 0
    OUT         ;просто выводим 0, так как корней нет

:7
    HLT
