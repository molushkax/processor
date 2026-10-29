#ifndef COMMANDS_H
#define COMMANDS_H

#define QUANTITY_OF_REGS 4

enum commands { HLT = 0, PUSH, ADD, SUB, MUL, DIV, OUT, IN, POP, PUSHR, JMP, CALL,
                JA, JAE, JB, JBE, JE, JNE, RET, SQRT, PUSHM, POPM, DRAW, PRINT, POPRM };

struct comp_ass {
    const char* name_command_assembler;
    int         code_of_command;
};

const struct comp_ass code_of_commands[] = { //TODO check
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
    { "SQRT", 19},
    { "PUSHM", 20},
    { "POPM", 21},
    { "DRAW", 22},
    { "PRINT", 23},
    { "POPRM", 24}
};

#endif
