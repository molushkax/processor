#ifndef COMMANDS_H
#define COMMANDS_H

enum commands { HLT = 0, PUSH, ADD, SUB, MUL, DIV, OUT, IN, POP, PUSHR, JMP, CALL, JA, JAE, JB, JBE, JE, JNE, RET, SQRT };

struct comp_ass {
    const char* name_command_assembler;
    int         code_of_command;
};

const struct comp_ass code_of_commands[] {
    { "HLT",   0},
    { "PUSH",  1},
    { "ADD",   2},
    { "SUB",   3},
    { "MUL",   4},
    { "DIV",   5},
    { "OUT",   6},
    { "IN",    7},
    { "POP",   8},
    { "PUSHR", 9},
    { "JMP",  10},
    { "CALL", 11},
    { "JA",   12},
    { "JAE",  13},
    { "JB",   14},
    { "JBE",  15},
    { "JE",   16},
    { "JNE",  17},
    { "RET",  18},
    { "SQRT", 19}
};

struct register_ini
{
    char name_of_register[10];
    stack_elem_t value_of_reg;
    int num_among_registers;
};

register_ini array_of_registers[] = {//TODO
    {"AX", 0, 1},
    {"BX", 0, 2},
    {"CX", 0, 3},
    {"DX", 0, 4}
};

struct processor_t {
    stack_t stk;
    stack_t call_stk;
    int     number_of_int_for_buf;
    int*    buffer_with_commands;
    int     instruction_pointer;
    register_ini array_of_registers[];
};

#endif
