#ifndef COMP_ASS_H
#define COMP_ASS_H

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

#define MAX_NUMBERS_OF_LABELS 20

struct string_info {
    char*  start_ptr;   // указатель на начало строки в общем буфере
    size_t length;      // длина строки (без '\0')
};

struct info_for_compile {
    size_t bytes_for_code_lst;
    size_t number_of_read_info;
    int    file_code_asm;
    char*  buffer;
    size_t number_of_strings;
    string_info* index;
    int*   labels;
    int    instruction_pointer;
};

struct error_map_2 {
    int         error_code;
    const char* error_message;
};

const struct error_map_2 errors_2[] = {
    {ENOMEM, "Memory allocation failed"},
    {EACCES, "Permission denied"},
    {ENOENT, "File not found"},
    {EIO,    "Input/output error"}
};

void   print_custom_error                 (int error_code, const char* function_name, int line);

int    get_size_of_file                   (const char* code_txt, size_t* bytes_for_code_txt);
int    read_words                         (FILE* code_lst, info_for_compile* compile_info);
int    process_line                       (FILE* code_lst, info_for_compile* compile_info, size_t line, int pass);

size_t read_commands_from_file            (info_for_compile* compile_info, const char* filename, const char* caller);
size_t count_strings_in_buffer            (char* buffer);
size_t fill_index_with_string_pointers    (info_for_compile* compile_info);

int    read_file                          (int file_onegin, char** buffer, int bytes_for_onegin, const char* caller);
int    check_of_memory_allocation         (const char* buffer, const char* caller);
int    check_of_opening_file              (const int file_onegin, const char* caller);
int    check_of_fopening_file             (FILE* onegin_out, const char* caller);
int    check_of_reading_file              (const int file_onegin, const ssize_t number_of_read_info, const char* caller);
int    check_of_memory_allocation_index   (string_info* index, const char* caller);
int    write_pointers_to_strings_in_index (info_for_compile* compile_info, const char* caller);
int    free_text_and_index                (string_info* index, char* buffer, size_t bytes_for_onegin, size_t number_of_strings);

#endif
