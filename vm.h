#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

int  count_ints_in_file               (const char* code_lst);
void create_buffer_with_commands      (processor_t* processor, const char* code_lst);
stack_elem_t  do_commands_from_buffer (processor_t* processor);
stack_elem_t pop_value                (processor_t* processor);

void push_command (processor_t* processor);
void pushr_command (processor_t* processor);
void pop_command  (processor_t* processor);
void add_command  (processor_t* processor);
void sub_command  (processor_t* processor);
void mul_command  (processor_t* processor);
void div_command  (processor_t* processor);
void out_command  (processor_t* processor, stack_elem_t* answer);
void jmp_command  (processor_t* processor);
void ja_command   (processor_t* processor);
void jae_command  (processor_t* processor);
void jb_command   (processor_t* processor);
void jbe_command  (processor_t* processor);
void je_command   (processor_t* processor);
void jne_command  (processor_t* processor);
void peek_two_values  (processor_t* processor, stack_elem_t* value2, stack_elem_t* value1);
void conditional_jump (processor_t* processor, bool cond);
void call_command (processor_t* processor);
void ret_command  (processor_t* processor);
void in_command   (processor_t* processor);
void sqrt_command (processor_t* processor);
void dump_stack   (processor_t* processor, const char* func, int line);
void print_data_elements_vm (const stack_elem_t* data, size_t size);
void dump_buffer_with_commands (processor_t* processor);
void dump_registers (processor_t* processor);
