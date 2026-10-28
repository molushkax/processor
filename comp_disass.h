#ifndef COMP_DISASS_H
#define COMP_DISASS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "commands.h"

struct disass_register_ini {
    const char* name_of_register;
    int         num_among_registers;
};

const struct disass_register_ini disass_registers[QUANTITY_OF_REGS] = {
    {"AX", 1},
    {"BX", 2},
    {"CX", 3},
    {"DX", 4}
};

struct info_for_disass {
    int32_t* buffer_with_commands;
    int      number_of_cells;
    size_t   bytes_for_bytecode;
};

int         read_bytecode_for_disass      (info_for_disass* disass_info, const char* code_bin, const char* caller);
int         get_size_of_bytecode_file     (FILE* fp, size_t* bytes_for_bytecode, const char* caller);
int         check_bytecode_size_alignment (size_t bytes_for_bytecode, const char* caller);
int         read_bytecode_cells           (FILE* fp, info_for_disass* disass_info, const char* caller);
int         free_bytecode_for_disass      (info_for_disass* disass_info);
int         write_disassembly             (info_for_disass* disass_info, const char* caller);
int         disass_one                    (info_for_disass* disass_info, int ip, const char* caller);
const char* cmd_name                      (int code);
const char* reg_name_by_num               (int reg_num);
int         cells_for_opcode              (int opcode);
int         check_of_fopening_file        (FILE* f, const char* caller);
int         check_of_memory_allocation    (const char* buffer, const char* caller);
void        print_custom_error            (int error_code, const char* function_name, int line);

#endif
