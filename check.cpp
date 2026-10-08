#include "stack.h"

#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

enum number_of_needed_cells { ONE_CELL = 1, TWO_CELLS = 2 };

struct comp_ass {
    const char* name_command_assembler;
    int         code_of_command;
    int         number_of_needed_cells;
};

const struct comp_ass code_of_commands[] {
    { "PUSH", 1, TWO_CELLS },
    { "ADD",  2, ONE_CELL },
    { "SUB",  3, ONE_CELL },
    { "DIV",  4, ONE_CELL },
    { "OUT",  5, ONE_CELL },
    { "HLT",  0, ONE_CELL }
};

int  get_size_of_file            (const char* code_txt, size_t* bytes_for_code_txt);
void print_buffer_elements       (FILE* log_file, char* buffer, size_t size);
int  read_words                  (FILE* code_txt, char* buffer);
void create_buffer_with_commands (int* buffer_with_commands, int number_of_int_for_buf);
int  do_commands_from_buffer     (int* buffer_with_commands, int number_of_int_for_buf);

//TODO почему htl не читается

int main () {
    const char* code_asm = "code.asm";

    size_t bytes_for_code_txt = 0;
    get_size_of_file (code_asm, &bytes_for_code_txt);

    char* buffer = (char*)calloc (bytes_for_code_txt + 1, sizeof(char));

    int file_code_txt = open (code_asm, O_RDONLY);
    size_t bytes_read = read (file_code_txt, (void*)buffer, (int)bytes_for_code_txt);
    buffer[bytes_read] = '\0';

    printf ("buffer: %s\n", buffer);

    FILE* code_txt = fopen("code.txt", "w");

    int number_of_int_for_buf = read_words (code_txt, buffer);
    printf ("%d int elements\n", number_of_int_for_buf);

    fclose (code_txt);

    int* buffer_with_commands = (int*)calloc (number_of_int_for_buf, sizeof(int));

    create_buffer_with_commands (buffer_with_commands, number_of_int_for_buf);

    int answer = do_commands_from_buffer (buffer_with_commands, number_of_int_for_buf);

    printf ("answer: %d", answer);

    free (buffer);
    free (buffer_with_commands);
}

int get_size_of_file (const char* code_txt, size_t* bytes_for_code_txt) {

    assert(code_txt != NULL);
    assert(bytes_for_code_txt != NULL);

    struct stat file_stat = {};

    if (stat (code_txt, &file_stat) == 0) {
        *bytes_for_code_txt = file_stat.st_size;
        printf ("\nNumber of bytes for code_txt: %zu\n\n", *bytes_for_code_txt);
        return 0;
    }

    else
    { // TODO custom error
        return -1;
    }
}

int read_words (FILE* code_txt, char* buffer) {
    assert (buffer != NULL);

    int i = 0;
    int j = 0;

    char word[10] = {};
    size_t number_of_codes = sizeof (code_of_commands) / sizeof (code_of_commands[0]);
    int final_number_of_int = 0;

    printf ("number of codes: %zu\n\n", number_of_codes);

    while (buffer[i] != '\0') { //TODO sscanf
        while (isalpha(buffer[i]) && buffer[i] != ' ' && buffer[i] != '\n') {
            word[j] = buffer[i];
            i++;
            j++;
        }

        if (word[0] != '\0') {
            size_t k = 0;

            for (k = 0; k < number_of_codes; k++) {
                if (strcmp(word, code_of_commands[k].name_command_assembler) == 0) {
                    fprintf (code_txt, "%d", code_of_commands[k].code_of_command);
                    final_number_of_int++;
                }
            }

            if (k == number_of_codes - 1 && strcmp(word, code_of_commands[k].name_command_assembler) != 0) { //TODO  норм чтоб ошибки ввода обрабатывала
                printf ("You write in code_asm something else\nI finish your attempt\n");
                return -1;
            }

            for (int h = 0; h <= 4; h++) {
                word[h] = '\0';
            }

            j = 0;
        }

        if (buffer[i] == ' ' || buffer[i] == '\n') {
            fprintf (code_txt, "%c", buffer[i]);
            i++;
        }

        if (isdigit(buffer[i])) {
            for (int h = 0; isdigit(buffer[i]); h++) {
                word[h] = buffer[i];
                i++;
            }

            fprintf (code_txt, "%s", word);
            final_number_of_int++;
        }
    }

    return final_number_of_int;
}

void create_buffer_with_commands (int* buffer_with_commands, int number_of_int_for_buf) {
    assert (buffer_with_commands != NULL);

    FILE* code_txt = fopen ("code.txt", "r");
    if (code_txt == NULL) {
        fprintf (stderr, "no such file\n");
        return;
    }

    size_t i = 0;
    int x = 0;
    printf ("lalalala\n");

    while (fscanf(code_txt, "%d", &x) == 1 && i < number_of_int_for_buf) {
        buffer_with_commands[i] = x;
        i++;
    }

    fclose(code_txt);
}

int do_commands_from_buffer (int* buffer_with_commands, int number_of_int_for_buf) {
    assert (buffer_with_commands != NULL);

    stack_t stk = {};
    stk.capacity = 5;
    stack_init (&stk);

    int i = 0;
    int answer = 0;

    while (i < number_of_int_for_buf) {

        if (buffer_with_commands[i] == 1) {
            PUSH_AND_CHECK(&stk, buffer_with_commands[i + 1]);
            i += 2;
        }

        if (buffer_with_commands[i] == 2) {
            int value1 = (int)stack_top (&stk);
            POP_AND_CHECK (&stk);
            int value2 = (int)stack_top (&stk);
            POP_AND_CHECK (&stk);
            int result = value2 + value1;
            PUSH_AND_CHECK (&stk, result);
            i += 1;
        }

        if (buffer_with_commands[i] == 3) {
            int value1 = (int)stack_top (&stk);
            POP_AND_CHECK (&stk);
            int value2 = (int)stack_top (&stk);
            POP_AND_CHECK (&stk);
            int result = value2 - value1;
            PUSH_AND_CHECK (&stk, result);
            i += 1;
        }

        if (buffer_with_commands[i] == 4) {
            int value1 = (int)stack_top (&stk);
            POP_AND_CHECK (&stk);
            int value2 = (int)stack_top (&stk);
            POP_AND_CHECK (&stk);
            int result = value2 / value1;
            PUSH_AND_CHECK (&stk, result);
            i += 1;
        }

        if (buffer_with_commands[i] == 5) {
            answer = stack_top (&stk);
            STACK_DUMP (&stk);
            i++;
        }

        if (buffer_with_commands[i] == 0) {
            return answer;
        }
    }

    return -1;
}
