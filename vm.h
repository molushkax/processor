#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#include "stack.h"
#include "commands.h"

#define RAM_SIZE 100
#define YELLOW   "\033[1;33m"

struct processor_t {
    stack_t  stk;
    stack_t  call_stk;
    int      number_of_int_for_buf;
    int32_t* buffer_with_commands;
    int      instruction_pointer;
    double   reg_values[QUANTITY_OF_REGS];
    double   ram[RAM_SIZE];
};

int  count_ints_in_file               (const char* code_lst);
void create_buffer_with_commands      (processor_t* processor, const char* code_lst);
stack_elem_t  do_commands_from_buffer (processor_t* processor);
stack_elem_t pop_value                (processor_t* processor);

void push_command    (processor_t* processor);
void pushr_command   (processor_t* processor);
void pop_command     (processor_t* processor);
void add_command     (processor_t* processor);
void sub_command     (processor_t* processor);
void mul_command     (processor_t* processor);
void div_command     (processor_t* processor);
void out_command     (processor_t* processor);
void jmp_command     (processor_t* processor);
void ja_command      (processor_t* processor);
void jae_command     (processor_t* processor);
void jb_command      (processor_t* processor);
void jbe_command     (processor_t* processor);
void je_command      (processor_t* processor);
void jne_command     (processor_t* processor);
void pop_two_values  (processor_t* processor, stack_elem_t* value2, stack_elem_t* value1);
void conditional_jump (processor_t* processor, bool cond);
void call_command    (processor_t* processor);
void ret_command     (processor_t* processor);
void in_command      (processor_t* processor);
void sqrt_command    (processor_t* processor);
void dump_stack      (processor_t* processor, const char* func, int line);
void print_data_elements_vm    (const stack_elem_t* data, size_t size);
void dump_buffer_with_commands (processor_t* processor);
void dump_registers_and_ram    (processor_t* processor);
void pushm_command   (processor_t* processor);
void popm_command    (processor_t* processor);
void draw_command    (processor_t* processor);
void print_command   (processor_t* processor);

#endif
