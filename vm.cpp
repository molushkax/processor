#include "stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

enum number_of_needed_cells { ONE_CELL = 1, TWO_CELLS = 2 };
enum commands { HLT = 0, PUSH, ADD, SUB, MUL, DIV, OUT, IN };

int  count_ints_in_file               (const char* code_lst);
void create_buffer_with_commands      (int* buffer_with_commands, int number_of_int_for_buf, const char* code_lst);
stack_elem_t  do_commands_from_buffer (int* buffer_with_commands, int number_of_int_for_buf);
stack_elem_t pop_value                (stack_t* stk);

int main () {
    const char* code_lst = "code.lst";

    int number_of_int_for_buf = count_ints_in_file (code_lst);
    if (number_of_int_for_buf <= 0) {
        fprintf (stderr, "empty or missing %s\n", code_lst);
        return 1;
    }
    printf ("%d int elements\n", number_of_int_for_buf);

    int* buffer_with_commands = (int*)calloc (number_of_int_for_buf, sizeof(int));

    create_buffer_with_commands (buffer_with_commands, number_of_int_for_buf, code_lst);

    stack_elem_t answer = do_commands_from_buffer (buffer_with_commands, number_of_int_for_buf);

    printf ("answer: " SP "\n", answer);

    free (buffer_with_commands);
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

void create_buffer_with_commands (int* buffer_with_commands, int number_of_int_for_buf, const char* code_lst) {
    assert (buffer_with_commands != NULL);

    FILE* f = fopen (code_lst, "r");
    if (f == NULL) {
        fprintf (stderr, "no such file\n");
        return;
    }

    size_t i = 0;
    int x = 0;

    while (fscanf (f, "%d", &x) == 1 && i < (size_t)number_of_int_for_buf) {
        buffer_with_commands[i] = x;
        i++;
    }

    fclose (f);
}

stack_elem_t do_commands_from_buffer (int* buffer_with_commands, int number_of_int_for_buf) {
    assert (buffer_with_commands != NULL);

    stack_t stk = {};
    stk.capacity = 5;
    stack_init (&stk);

    int i = 0;
    stack_elem_t answer = 0;

    while (i < number_of_int_for_buf) {

        switch (buffer_with_commands[i]) {
            case PUSH: {
                PUSH_AND_CHECK(&stk, buffer_with_commands[i + 1]);
                i += 2;
                break;
            }

            case ADD: {
                stack_elem_t value1 = pop_value (&stk);
                stack_elem_t value2 = pop_value (&stk);
                stack_elem_t result = value2 + value1;
                PUSH_AND_CHECK (&stk, result);
                i += 1;
                break;
            }

            case SUB: {
                stack_elem_t value1 = pop_value (&stk);
                stack_elem_t value2 = pop_value (&stk);
                stack_elem_t result = value2 - value1;
                PUSH_AND_CHECK (&stk, result);
                i += 1;
                break;
            }

            case MUL: {
                stack_elem_t value1 = pop_value (&stk);
                stack_elem_t value2 = pop_value (&stk);
                stack_elem_t result = value2 * value1;
                PUSH_AND_CHECK (&stk, result);
                i += 1;
                break;
            }

            case DIV: {
                stack_elem_t value1 = pop_value (&stk);
                stack_elem_t value2 = pop_value (&stk);
                stack_elem_t result = value2 / value1;
                PUSH_AND_CHECK (&stk, result);
                i += 1;
                break;
            }

            case OUT: {
                answer = stack_top (&stk);
                STACK_DUMP (&stk);
                i++;
                break;
            }

            case HLT: {
                return answer;
            }

            default: {
                fprintf (stderr, "Unknown command\n");
                return -1;
            }
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
