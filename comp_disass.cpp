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

int  get_size_of_file      (const char* code_txt, size_t* bytes_for_code_txt);
void print_buffer_elements (FILE* log_file, char* buffer, size_t size);
void read_words            (FILE* code_txt, char* buffer, size_t bytes_for_code_txt);

int main () {
    const char* code_txt = "code.txt";

    size_t bytes_for_code_txt = 0;
    get_size_of_file (code_txt, &bytes_for_code_txt);

    char* buffer = (char*)calloc(bytes_for_code_txt + 1, sizeof(char));
    int file_code_txt = open (code_txt, O_RDONLY);
    size_t bytes_read = read (file_code_txt, (void*)buffer, (int)bytes_for_code_txt);
    buffer[bytes_read] = '\0';
    printf ("buffer: %s\n", buffer);

    FILE* code_txt = fopen("code.txt", "w");

    read_words (code_txt, buffer, bytes_read);

    //print_buffer_elements (log_file, buffer, 30);
    //for (int i = 0; )
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

void make_from_codes_words (FILE* code_asm, char* buffer, size_t bytes_for_code_txt) {
    assert (buffer != NULL);

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

    size_t i = 0;
    size_t j = 0;
    char word[10] = {};
    size_t number_of_codes = sizeof (code_of_commands) / sizeof (code_of_commands[0]);
    printf ("number of codes: %zu\n\n", number_of_codes);

    while (buffer[i] != '\0') {
        while (isalpha(buffer[i]) && buffer[i] != ' ' && buffer[i] != '\n') {
            word[j] = buffer[i];
            i++;
            j++;
        }

        if (word[0] != '\0') {
            for (size_t k = 0; k < number_of_codes; k++) {
                if (strcmp(word, code_of_commands[k].name_command_assembler) == 0) {
                    fprintf (code_txt, "%d", code_of_commands[k].code_of_command);
                }
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
        }
    }
}

