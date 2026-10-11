#ifndef COMMANDS_H
#define COMMANDS_H

enum commands { HLT = 0, PUSH, ADD, SUB, MUL, DIV, OUT, IN };

struct comp_ass {
    const char* name_command_assembler;
    int         code_of_command;
    int         number_of_needed_cells;
};

const struct comp_ass code_of_commands[] {
    { "HLT",  0},
    { "PUSH", 1},
    { "ADD",  2},
    { "SUB",  3},
    { "MUL",  4},
    { "DIV",  5},
    { "OUT",  6},
    { "IN",   7}
};

#endif
