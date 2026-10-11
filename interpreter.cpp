#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

stack_elem_t pop_value(stack_t* stk);
void do_commands_from_file (FILE* code_asm, stack_t* stk);

int main() {
    stack_t stk = {};
    stk.capacity = 5;
    stackerr_t err = stack_init(&stk);

    if (err != STACK_OK) {
        printf("Stack init error: %d\n", err);
        return 1;
    }

    FILE* code_asm = fopen("code_interp.asm", "r");
    if (code_asm == NULL) {
        printf("Cannot open file code.asm\n");
        return 1;
    }

    do_commands_from_file (code_asm, &stk);

    return 0;
}

stack_elem_t pop_value(stack_t* stk) {
    STACK_ASSERT(stk);

    stack_elem_t val = stack_top(stk);
    POP_AND_CHECK(stk);
    return val;
}

void do_commands_from_file (FILE* code_asm, stack_t* stk) {
    if (code_asm == NULL) {
        printf("Cannot open file code.asm\n");
        return;
    }

    printf("--- Interpreter started ---\n");

    char name[10];

    while (!feof(code_asm)) {
        if (fscanf(code_asm, "%9s", name) != 1) {
            break;
        }

        if (strcmp(name, "PUSH") == 0) {
            stack_elem_t value = 0;
            fscanf(code_asm, "%lf", &value);
            PUSH_AND_CHECK(stk, value);
            printf("PUSH " SP "\n", value);
        }
        else if (strcmp(name, "ADD") == 0) {
            stack_elem_t b = pop_value(stk);
            stack_elem_t a = pop_value(stk);
            stack_elem_t res = a + b;

            PUSH_AND_CHECK(stk, res);
            printf("ADD: " SP " + " SP " = " SP "\n", a, b, res);
        }

        else if (strcmp(name, "SUB") == 0) {
            stack_elem_t b = pop_value(stk);
            stack_elem_t a = pop_value(stk);
            stack_elem_t res = a - b;

            PUSH_AND_CHECK(stk, res);
            printf("SUB: " SP " - " SP " = " SP "\n", a, b, res);
        }

        else if (strcmp(name, "MUL") == 0) {
            stack_elem_t b = pop_value(stk);
            stack_elem_t a = pop_value(stk);
            stack_elem_t res = a * b;

            PUSH_AND_CHECK(stk, res);
            printf("MUL: " SP " * " SP " = " SP "\n", a, b, res);
        }

        else if (strcmp(name, "DIV") == 0) {
            stack_elem_t b = pop_value(stk);
            stack_elem_t a = pop_value(stk);

            if (b == 0) {
                printf("Error: Division by zero!\n");
                break;
            }

            stack_elem_t res = a / b;
            PUSH_AND_CHECK(stk, res);
            printf("DIV: " SP " / " SP " = " SP "\n", a, b, res);
        }

        else if (strcmp(name, "OUT") == 0) {
            double val = pop_value(stk);
            printf("OUT: " SP "\n", val);
        }

        else if (strcmp(name, "HLT") == 0) {
            printf("HLT: Execution stopped.\n");
            break;
        }

        else {
            printf("Unknown command: %s\n", name);
        }
    }

    fclose(code_asm);
    stack_destroy(stk);
    printf("--- Interpreter finished ---\n");
}
