#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "stack.h"
#include "commands.h"

int  count_ints_in_file               (const char* code_lst);
void create_buffer_with_commands      (processor_t* processor, const char* code_lst);
stack_elem_t  do_commands_from_buffer (processor_t* processor);
stack_elem_t pop_value                (processor_t* processor);

void push_command (processor_t* processor);
void add_command  (processor_t* processor);
void sub_command  (processor_t* processor);
void mul_command  (processor_t* processor);
void div_command  (processor_t* processor);
void out_command  (processor_t* processor, stack_elem_t* answer);
void peek_two_values (processor_t* processor, stack_elem_t* value2, stack_elem_t* value1);
void jmp_command  (processor_t* processor);
void ja_command   (processor_t* processor);
void jae_command  (processor_t* processor);
void jb_command   (processor_t* processor);
void jbe_command  (processor_t* processor);
void je_command   (processor_t* processor);
void jne_command  (processor_t* processor);
void conditional_jump (processor_t* processor, bool cond);

int main () {
    processor_t processor = {};
    const char* code_lst = "code.lst";

    processor.number_of_int_for_buf = count_ints_in_file (code_lst);
    if (processor.number_of_int_for_buf <= 0) {
        fprintf (stderr, "empty or missing %s\n", code_lst);
        return 1;
    }

    printf ("%d int elements\n", processor.number_of_int_for_buf);

    processor.buffer_with_commands = (int*)calloc (processor.number_of_int_for_buf, sizeof(int)); //TODO с проверкой

    create_buffer_with_commands (&processor, code_lst);

    stack_elem_t answer = do_commands_from_buffer (&processor);

    printf ("answer: " SP "\n", answer);

    free (processor.buffer_with_commands);
    return 0;
}

int count_ints_in_file (const char* code_lst) {
    assert (code_lst != NULL);

    FILE* f = fopen (code_lst, "r");
    if (!f) { perror ("count: open"); return 0; }

    int counter = 0;
    int x = 0;
    while (fscanf (f, "%d", &x) == 1) {
        counter++;
    }
    fclose (f);
    return counter;
}

void create_buffer_with_commands (processor_t* processor, const char* code_lst) {
    assert (processor != NULL);

    FILE* f = fopen (code_lst, "r");
    if (f == NULL) {
        fprintf (stderr, "no such file\n");
        return;
    }

    size_t i = 0;
    int x = 0;

    while (fscanf (f, "%d", &x) == 1 && i < (size_t)(processor->number_of_int_for_buf)) {
        (processor->buffer_with_commands)[i] = x;
        i++;
    }

    fclose (f);
}

stack_elem_t do_commands_from_buffer (processor_t* processor) {
    assert (processor != NULL);

    processor->stk = {};
    (processor->stk).capacity = 5;
    stack_init (&(processor->stk));

    processor->instruction_pointer = 0;
    stack_elem_t answer = 0;

    while (processor->instruction_pointer < processor->number_of_int_for_buf) {

        switch ((processor->buffer_with_commands)[processor->instruction_pointer]) {
            case PUSH: push_command (processor);
                break;

            case ADD:  add_command (processor);
                break;

            case SUB:  sub_command (processor);
                break;

            case MUL:  mul_command (processor);
                break;

            case DIV:  div_command (processor);
                break;

            case JMP:  jmp_command (processor);
                break;

            case JA:   ja_command (processor);
                break;

            case JAE:  jae_command (processor);
                break;

            case JB:   jb_command  (processor);
                break;

            case JBE:  jbe_command (processor);
                break;

            case JE:   je_command  (processor);
                break;

            case JNE:  jne_command (processor);
                break;

            case OUT:  out_command (processor, &answer);
                break;

            case HLT:
                return answer;

            default: fprintf (stderr, "Unknown command\n");
                return -1;
        }
        printf ("hey instr pointer: %d\n", processor->instruction_pointer);
    }

    return 0;
}

stack_elem_t pop_value(processor_t* processor) {
    STACK_ASSERT(&(processor->stk));

    stack_elem_t val = stack_top(&(processor->stk));
    POP_AND_CHECK(&(processor->stk));
    return val;
}

void push_command (processor_t* processor) {
    assert (processor != NULL);

    PUSH_AND_CHECK(&(processor->stk),
                   (processor->buffer_with_commands)[processor->instruction_pointer + 1]);
    processor->instruction_pointer += 2;
}

void add_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value1 = pop_value (processor);
    stack_elem_t value2 = pop_value (processor);
    stack_elem_t result = value2 + value1;

    PUSH_AND_CHECK (&(processor->stk), result);
    processor->instruction_pointer += 1;
}

void sub_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value1 = pop_value (processor);
    stack_elem_t value2 = pop_value (processor);
    stack_elem_t result = value2 - value1;

    PUSH_AND_CHECK (&(processor->stk), result);
    processor->instruction_pointer += 1;
}

void mul_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value1 = pop_value (processor);
    stack_elem_t value2 = pop_value (processor);
    stack_elem_t result = value2 * value1;

    PUSH_AND_CHECK (&(processor->stk), result);
    processor->instruction_pointer += 1;
}

void div_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value1 = pop_value (processor);
    stack_elem_t value2 = pop_value (processor);
    stack_elem_t result = value2 / value1;

    PUSH_AND_CHECK (&(processor->stk), result);
    processor->instruction_pointer += 1;
}

void out_command (processor_t* processor, stack_elem_t* answer) {
    assert (processor != NULL);

    *answer = stack_top (&(processor->stk));
    STACK_DUMP (&(processor->stk));
    processor->instruction_pointer += 1;
}

void peek_two_values (processor_t* processor,
                      stack_elem_t* value2,
                      stack_elem_t* value1) {
    assert (processor != NULL);
    assert (value1   != NULL);
    assert (value2   != NULL);

    stack_t* stk = &(processor->stk);

    assert (stk->size >= 2);

    *value1 = stk->data[stk->size - 1];   // вершина
    *value2 = stk->data[stk->size - 2];   // второй сверху
}

void jmp_command (processor_t* processor) {
    assert (processor != NULL);

    int target = (processor->buffer_with_commands)[processor->instruction_pointer + 1];

    if (target < 0 || target >= processor->number_of_int_for_buf) {
        fprintf (stderr, "JMP: invalid target address %d\n", target);
        processor->instruction_pointer = processor->number_of_int_for_buf - 1;
        return;
    }

    processor->instruction_pointer = target;
}

void ja_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value2 = 0, value1 = 0;
    peek_two_values (processor, &value2, &value1);

    conditional_jump (processor, value2 > value1);
}

void jae_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value2 = 0, value1 = 0;
    peek_two_values (processor, &value2, &value1);

    conditional_jump (processor, value2 >= value1);
}

void jb_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value2 = 0, value1 = 0;
    peek_two_values (processor, &value2, &value1);

    conditional_jump (processor, value2 < value1);
}

void jbe_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value2 = 0, value1 = 0;
    peek_two_values (processor, &value2, &value1);

    conditional_jump (processor, value2 <= value1);
}

void je_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value2 = 0, value1 = 0;
    peek_two_values (processor, &value2, &value1);

    conditional_jump (processor, equal_double(value2, value1));
}

void jne_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value2 = 0, value1 = 0;
    peek_two_values (processor, &value2, &value1);

    conditional_jump (processor, !equal_double(value2, value1));
}

void conditional_jump (processor_t* processor, bool condition) {
    assert (processor != NULL);

    int target = (processor->buffer_with_commands)[processor->instruction_pointer + 1];

    if (condition) {
        if (target < 0 || target >= processor->number_of_int_for_buf) {
            fprintf (stderr, "conditional jump: invalid target %d\n", target);
            processor->instruction_pointer = processor->number_of_int_for_buf;
            return;
        }

        processor->instruction_pointer = target;
    }

    else {
        processor->instruction_pointer += 2;
    }
}
