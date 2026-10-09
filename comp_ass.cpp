#include "stack.h"

#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

int  get_size_of_file (const char* code_txt, size_t* bytes_for_code_txt);
int  read_words       (FILE* code_lst, char* buffer);

int main () {
    const char* code_asm = "code.asm";
    const char* code_lst = "code.lst";

    size_t bytes_for_code_lst = 0;
    get_size_of_file (code_asm, &bytes_for_code_lst);

    char* buffer = (char*)calloc (bytes_for_code_lst + 1, sizeof(char));

    int file_code_lst = open (code_asm, O_RDONLY);
    size_t bytes_read = read (file_code_lst, (void*)buffer, (int)bytes_for_code_lst);
    buffer[bytes_read] = '\0';
    close (file_code_lst);

    printf ("buffer: %s\n", buffer);

    FILE* code_lst_file = fopen (code_lst, "w");
    if (code_lst_file == NULL) { perror ("code.lst"); return 1; }

    int number_of_int = read_words (code_lst_file, buffer);
    printf ("%d int elements\n", number_of_int);

    fclose (code_lst_file);

    free (buffer);
    return 0;
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
    else {
        return -1;
    }
}

int read_words (FILE* code_lst, char* buffer) {
    assert (buffer != NULL);

    int i = 0;
    int j = 0;

    char word[10] = {};
    size_t number_of_codes = sizeof (code_of_commands) / sizeof (code_of_commands[0]);
    int final_number_of_int = 0;

    printf ("number of codes: %zu\n\n", number_of_codes);

    while (buffer[i] != '\0') {
        while (isalpha(buffer[i]) && buffer[i] != ' ' && buffer[i] != '\n') {
            word[j] = buffer[i];
            i++;
            j++;
        }

        if (word[0] != '\0') {
            size_t k = 0;

            for (k = 0; k < number_of_codes; k++) {
                if (strcmp(word, code_of_commands[k].name_command_assembler) == 0) {
                    fprintf (code_lst, "%d", code_of_commands[k].code_of_command);
                    final_number_of_int++;
                }
            }

            if (k == number_of_codes - 1 && strcmp(word, code_of_commands[k].name_command_assembler) != 0) {
                printf ("You write in code_asm something else\nI finish your attempt\n");
                return -1;
            }

            for (int h = 0; h <= 4; h++) {
                word[h] = '\0';
            }

            j = 0;
        }

        if (buffer[i] == ' ' || buffer[i] == '\n') {
            fprintf (code_lst, "%c", buffer[i]);
            i++;
        }

        if (isdigit(buffer[i])) {
            for (int h = 0; isdigit(buffer[i]); h++) {
                word[h] = buffer[i];
                i++;
            }

            fprintf (code_lst, "%s", word);
            final_number_of_int++;
        }
    }

    return final_number_of_int;
}
