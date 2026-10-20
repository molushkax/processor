#include "comp_ass.h"
#include "stack.h"
#include "commands.h"

int main () {
    const char* code_asm = "code.asm";
    const char* code_lst = "code.lst";

    info_for_compile compile_info = {};
    compile_info.labels = (int*)calloc (MAX_NUMBERS_OF_LABELS, sizeof (int));

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

    // ---------- первый проход: только метки ----------
    compile_info->instruction_pointer = 0;
    for (size_t line = 0; line < compile_info->number_of_strings; line++) {
        process_line (NULL, compile_info, line, /*pass=*/1);
    }

    // ---------- второй проход: генерация кода ----------
    compile_info->instruction_pointer = 0;
    for (size_t line = 0; line < compile_info->number_of_strings; line++) {
        int result = process_line (code_lst, compile_info, line, /*pass=*/2);
        if (result < 0) return -1;
    }

    return compile_info->instruction_pointer;
}

int process_line (FILE* code_lst, info_for_compile* compile_info, size_t line, int pass) {
    assert (compile_info != NULL);
    assert (compile_info->index != NULL);

    const char* start  = compile_info->index[line].start_ptr;
    size_t      length = compile_info->index[line].length;

    if (start == NULL || start[0] == '\0') {
        return 0;
    }

    size_t i = 0;
    int j = 0;
    char word[16] = {};

    //--------- обработка меток --------
    if (start[0] == ':') {
        i++;

        int label_number = 0;
        bool has_digits = false;
        while (i < length && isdigit((unsigned char)start[i])) {
            label_number = label_number * 10 + (start[i] - '0');
            i++;
            has_digits = true;
        }

        if (!has_digits) {
            printf ("Label without number: %s\n", start);
            return -1;
        }

        if (label_number < 0 || label_number >= MAX_NUMBERS_OF_LABELS) {
            printf ("Label number %d out of range [0, %d)\n", label_number, MAX_NUMBERS_OF_LABELS);
            return -1;
        }

        if (pass == 1) {
            (compile_info->labels)[label_number] = compile_info->instruction_pointer;
            printf ("{pass 1} label :%d -> ip %d\n", label_number, compile_info->instruction_pointer);
        }

        return 0;
    }

    // ---------- имя команды ----------
    while (i < length && isalpha((unsigned char)start[i])) {
        word[j] = start[i];
        i++;
        j++;
    }

    word[j] = '\0';

    // ---------- поиск команды ----------
    size_t k = 0;
    size_t number_of_codes  = sizeof (code_of_commands) / sizeof (code_of_commands[0]);
    bool   found = false;
    int    command_code = -1;
    const char* command_name = NULL;

    if (start[i] == ';') {
        found = true;
        return 0;
    }

    for (k = 0; k < number_of_codes; k++) {
        if (strcmp(word, code_of_commands[k].name_command_assembler) == 0) {
            command_code = code_of_commands[k].code_of_command;
            command_name = code_of_commands[k].name_command_assembler;
            found = true;
            break;
        }
    }


    if (!found) {
        if (pass == 2) {
            printf ("You write in code_asm something else\nI finish my attempt\n");
            return -1;
        }

        return 0;
    }

    // ---------- команда без аргумента ----------
    if (length <= 4) {
        if (pass == 2) fprintf (code_lst, "%d\n", command_code);
        compile_info->instruction_pointer++;

        return 0;
    }

    // ---------- пропускаем пробелы ----------
    while (i < length && start[i] == ' ') {
        i++;
    }

    // ---------- если аргумент - регистр ----------
    if (isalpha((unsigned char)start[i])) {
        int r = 0;
        char reg_word[10] = {};
        while (i < length && isalpha((unsigned char)start[i])) {
            reg_word[r] = start[i];
            i++;
            r++;
        }
        reg_word[r] = '\0';

        size_t num_of_regs = sizeof (array_of_registers) / sizeof (array_of_registers[0]);
        bool reg_found = false;
        int  reg_num   = -1;

        for (size_t q = 0; q < num_of_regs; q++) {
            if (strcmp(reg_word, array_of_registers[q].name_of_register) == 0) {
                reg_num   = array_of_registers[q].num_among_registers;
                reg_found = true;
                break;
            }
        }

        if (!reg_found) {
            if (pass == 2) {
                printf ("Unknown register: %s\n", reg_word);
                return -1;
            }

            return 0;
        }

        int reg_command_code = 0;
        if (strcmp(command_name, "PUSH") == 0)      reg_command_code = PUSHR; // 9
        else if (strcmp(command_name, "POP") == 0)  reg_command_code = POP;   // 8
        else {
            if (pass == 2) {
                printf ("Command %s doesn't support register argument\n", command_name);
                return -1;
            }

            return 0;
        }

        if (pass == 2) fprintf (code_lst, "%d %d\n", reg_command_code, reg_num);
        compile_info->instruction_pointer += 2;
    }
    // ---------- если аргумент - число ----------
    else if (isdigit((unsigned char)start[i])) {
        if (strcmp(command_name, "POP") == 0) {
            printf ("POP requires a register, not a number\n");
            return -1;
        }

        char num_word[10] = {};
        int  n = 0;
        while (i < length && isdigit((unsigned char)start[i])) {
            num_word[n] = start[i];
            i++;
            n++;
        }
        num_word[n] = '\0';

        if (pass == 2) fprintf (code_lst, "%d %s\n", command_code, num_word);
        compile_info->instruction_pointer += 2;
    }

    else if (start[i] == ':') {
        i++;

        int label_number = 0;
        bool has_digits = false;

        while (i < length && isdigit((unsigned char)start[i])) {
            label_number = label_number * 10 + (start[i] - '0');
            i++;
            has_digits = true;
        }

        if (!has_digits) {
            printf ("Label argument without number after %s\n", command_name);
            return -1;
        }

        if (label_number < 0 || label_number >= MAX_NUMBERS_OF_LABELS) {
            printf ("Label number %d out of range [0, %d)\n", label_number, MAX_NUMBERS_OF_LABELS);
            return -1;
        }

        if (pass == 2) {
            fprintf (code_lst, "%d %d\n", command_code, (compile_info->labels)[label_number]);
        }
        compile_info->instruction_pointer += 2;
    }

    else if (start[i] == ';') {
        return 0;
    }

    else {
        printf ("Unknown argument after %s\n", command_name);
        return -1;
    }

    /*for (int i = 0; i < MAX_NUMBERS_OF_LABELS; i++) {
        fprintf (code_lst, "%d elem: %d\n", i, (compile_info->labels)[i]);
    }*/

    return compile_info->instruction_pointer;
}
