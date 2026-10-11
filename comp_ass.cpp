#include "comp_ass.h"
#include "stack.h"
#include "commands.h"

int main () {
    const char* code_asm = "code.asm";
    const char* code_lst = "code.lst";

    info_for_compile compile_info = {};

    read_commands_from_file (&compile_info, code_asm, __FUNCTION__);
    write_pointers_to_strings_in_index (&compile_info, __FUNCTION__);
    FILE* file_code_lst = fopen (code_lst, "w");

    for (size_t i = 0; i < compile_info.number_of_strings; i++) {
        printf ("string %zu: %s\n", i+1, (compile_info.index)[i].start_ptr);
    }

    read_words (file_code_lst, &compile_info);
    fclose (file_code_lst);

    free (compile_info.buffer);
    return 0;
}

size_t read_commands_from_file (info_for_compile* compile_info, const char* filename, const char* caller) {
    assert (compile_info != NULL);
    assert (filename  != NULL);

    get_size_of_file (filename, &(compile_info->bytes_for_code_lst));

    compile_info->file_code_asm = open (filename, O_RDONLY);
    check_of_opening_file (compile_info->file_code_asm, caller);

    compile_info->buffer = (char*)malloc (compile_info->bytes_for_code_lst + 1);
    check_of_memory_allocation (compile_info->buffer, caller);

    compile_info->number_of_read_info = read_file (compile_info->file_code_asm,
                                            &(compile_info->buffer),
                                            (int)(compile_info->bytes_for_code_lst),
                                            caller);
    compile_info->buffer[compile_info->number_of_read_info] = '\0';
    compile_info->number_of_strings = count_strings_in_buffer (compile_info->buffer);

    return compile_info->number_of_strings;
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

int write_pointers_to_strings_in_index (info_for_compile* compile_info, const char* caller) {
    assert (compile_info != NULL);

    compile_info->index = (string_info*)calloc (compile_info->number_of_strings + 1, sizeof (string_info));
    check_of_memory_allocation_index (compile_info->index, caller);

    fill_index_with_string_pointers (compile_info);

    return 0;
}

size_t fill_index_with_string_pointers (info_for_compile* compile_info) {
    assert (compile_info != NULL);

    (compile_info->index)[0].start_ptr = compile_info->buffer;
    (compile_info->index)[0].length    = 0;
    size_t i = 0;
    size_t j = 1;

    for (i = 0; i < compile_info->number_of_read_info; i++) {
        if ((compile_info->buffer)[i] == '\n') {
            (compile_info->buffer)[i] = '\0';

            (compile_info->index)[j - 1].length = (size_t)(&(compile_info->buffer)[i] - (compile_info->index)[j - 1].start_ptr);

            if (i + 1 < compile_info->number_of_read_info) {
                (compile_info->index)[j].start_ptr = &(compile_info->buffer)[i] + 1;
                (compile_info->index)[j].length    = 0;
                j++;
            }
        }
    }

    if (j > 0 && (compile_info->index)[j - 1].length == 0) {
        (compile_info->index)[j - 1].length = (size_t)(&(compile_info->buffer)[compile_info->bytes_for_code_lst] - (compile_info->index)[j - 1].start_ptr);
    }

    return j;
}


size_t count_strings_in_buffer (char* buffer) {
    assert (buffer != NULL);

    size_t num = 0;
    int i = 0;
    while (buffer[i] != '\0') {
        if (buffer[i] == '\n') num++; //TODO подсчет строк здесь
        i++;
    }

    return num;
}

int check_of_opening_file (const int file_code_lst, const char* caller) {
    if (file_code_lst == -1) {
        print_custom_error (ENOENT, caller, __LINE__);
        return -1;
    }
    else {
        printf ("Descriptor of file: %d\n\n", file_code_lst);
        return 0;
    }
}

int check_of_fopening_file (FILE* onegin_out, const char* caller) {
    if (onegin_out == NULL) {
        print_custom_error (ENOENT, caller, __LINE__);
        return -1;
    }
    else return 0;
}

int check_of_memory_allocation (const char* buffer, const char* caller) {
    if (buffer == NULL) {
        print_custom_error (ENOMEM, caller, __LINE__);
        return -1;
    }
    else return 0;
}

int read_file (int file_code_lst, char** buffer, int bytes_for_code_lst, const char* caller) {
    ssize_t read_info = read (file_code_lst, *buffer, (int)bytes_for_code_lst);
    check_of_reading_file (file_code_lst, read_info, caller);
    (*buffer)[read_info] = '\0';

    return (int)read_info;
}

int check_of_reading_file (const int file_code_lst, const ssize_t number_of_read_info, const char* caller) {
    if (number_of_read_info == -1) {
        print_custom_error (EIO, caller, __LINE__);
        close (file_code_lst);
        return -1;
    }
    else {
        printf ("Result of read function: %zd\n\n", number_of_read_info);
        return 0;
    }
}

int check_of_memory_allocation_index (string_info* index, const char* caller) {
    if (index == NULL) {
        print_custom_error (ENOMEM, caller, __LINE__);
        return -1;
    }
    else return 0;
}

void print_custom_error (int error_code, const char* function_name, int line) {
    const char* message = "Error";
    size_t number_of_errors = sizeof (errors_2) / sizeof (errors_2[0]);

    for (size_t i = 0; i < number_of_errors; i++) {
        if (errors_2[i].error_code == error_code) {
            message = errors_2[i].error_message;
            break;
        }
    }

    fprintf (stderr, RED "ERROR %d %s in function <<%s>> in line %d\n" NO_COLOR,
             error_code, message, function_name, line);
}

int free_text_and_index (string_info* index, char* buffer, size_t bytes_for_code_lst, size_t number_of_strings) {
    memset (index,  0, (number_of_strings + 1) * sizeof (string_info));
    memset (buffer, 0, bytes_for_code_lst + 1);

    free (buffer);
    free (index);

    buffer = NULL;
    index  = NULL;

    return 0;
}

int read_words (FILE* code_lst, info_for_compile* compile_info) {
    assert (code_lst     != NULL);
    assert (compile_info != NULL);
    assert (compile_info->index != NULL);

    size_t number_of_codes  = sizeof (code_of_commands) / sizeof (code_of_commands[0]);
    int    final_number_of_int = 0;

    printf ("number of codes: %zu\n\n", number_of_codes);

    for (size_t line = 0; line < compile_info->number_of_strings; line++) {

        const char* start  = compile_info->index[line].start_ptr;
        size_t      length = compile_info->index[line].length;

        if (start == NULL || start[0] == '\0') {
            continue;
        }

        int i = 0;
        int j = 0;
        char word[16] = {};

        while (i < (int)length && isalpha((unsigned char)start[i])) {
            word[j] = start[i];
            i++;
            j++;
        }
        word[j] = '\0';

        size_t k = 0;
        bool   found = false;

        for (k = 0; k < number_of_codes; k++) {
            if (strcmp(word, code_of_commands[k].name_command_assembler) == 0) {
                fprintf (code_lst, "%d", code_of_commands[k].code_of_command);
                final_number_of_int++;
                found = true;
                break;
            }
        }

        if (!found) {
            printf ("You write in code_asm something else\nI finish your attempt\n");
            return -1;
        }

        if (length <= 4) {
            fprintf (code_lst, "\n");
            continue;
        }

        while (i < (int)length && (start[i] == ' ' || isdigit((unsigned char)start[i]))) {
            fprintf (code_lst, "%c", start[i]);
            i++;
        }

        fprintf (code_lst, "\n");
    }

    return final_number_of_int;
}
