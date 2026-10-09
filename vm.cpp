#include "stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

enum number_of_needed_cells { ONE_CELL = 1, TWO_CELLS = 2 };
enum commands { HLT = 0, PUSH, ADD, SUB, MUL, DIV, OUT, IN };

struct processor_t {
    stack_t stk;
    int     number_of_int_for_buf;
    int*    buffer_with_commands;
};

int  count_ints_in_file               (const char* code_lst);
void create_buffer_with_commands      (processor_t* processor, const char* code_lst);
stack_elem_t  do_commands_from_buffer (processor_t* processor);
stack_elem_t pop_value                (stack_t* stk);

void push_command (stack_t* stk, int* buffer_with_commands, int* i);
void add_command  (stack_t* stk, int* i);
void sub_command  (stack_t* stk, int* i);
void mul_command  (stack_t* stk, int* i);
void div_command  (stack_t* stk, int* i);
void out_command  (stack_t* stk, int* i, stack_elem_t* answer);

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

    int i = 0;
    stack_elem_t answer = 0;

    while (i < processor->number_of_int_for_buf) {

        switch ((processor->buffer_with_commands)[i]) {
            case PUSH: push_command (&(processor->stk), processor->buffer_with_commands, &i);
                break;

            case ADD:  add_command (&(processor->stk), &i);
                break;

            case SUB:  sub_command (&(processor->stk), &i);
                break;

            case MUL:  mul_command (&(processor->stk), &i);
                break;

            case DIV:  div_command (&(processor->stk), &i);
                break;

            case OUT:  out_command (&(processor->stk), &i, &answer);
                break;

            case HLT:
                return answer;

            default: fprintf (stderr, "Unknown command\n");
                return -1;
        }
    }

    return 0;
}

stack_elem_t pop_value(stack_t* stk) {
    STACK_ASSERT(stk);

    stack_elem_t val = stack_top(stk);
    POP_AND_CHECK(stk);
    return val;
}

void push_command (stack_t* stk, int* buffer_with_commands, int* i) {
    assert (stk != NULL);
    assert (i != NULL);

    PUSH_AND_CHECK(stk, buffer_with_commands[*i + 1]);
    *i += 2;
}

void add_command (stack_t* stk, int* i) {
    assert (stk != NULL);
    assert (i != NULL);

    stack_elem_t value1 = pop_value (stk);
    stack_elem_t value2 = pop_value (stk);
    stack_elem_t result = value2 + value1;

    PUSH_AND_CHECK (stk, result);
    *i += 1;
}

void sub_command (stack_t* stk, int* i) {
    assert (stk != NULL);
    assert (i != NULL);

    stack_elem_t value1 = pop_value (stk);
    stack_elem_t value2 = pop_value (stk);
    stack_elem_t result = value2 - value1;

    PUSH_AND_CHECK (stk, result);
    *i += 1;
}

void mul_command (stack_t* stk, int* i) {
    assert (stk != NULL);
    assert (i != NULL);

    stack_elem_t value1 = pop_value (stk);
    stack_elem_t value2 = pop_value (stk);
    stack_elem_t result = value2 * value1;

    PUSH_AND_CHECK (stk, result);
    *i += 1;
}

void div_command (stack_t* stk, int* i) {
    assert (stk != NULL);
    assert (i != NULL);

    stack_elem_t value1 = pop_value (stk);
    stack_elem_t value2 = pop_value (stk);
    stack_elem_t result = value2 / value1;

    PUSH_AND_CHECK (stk, result);
    *i += 1;
}

void out_command (stack_t* stk, int* i, stack_elem_t* answer) {
    assert (stk != NULL);
    assert (i != NULL);

    *answer = stack_top (stk);
    STACK_DUMP (stk);
    *i += 1;
}
