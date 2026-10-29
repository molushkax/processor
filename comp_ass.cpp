#include "bytecode.h"
#include "comp_ass.h"
#include "commands.h"
#include "stack.h"
#include "hash.h"

int main () {
    const char* code_asm = "code.asm";

    info_for_compile compile_info = {};
    compile_info.labels = (label_info*)calloc (MAX_NUMBERS_OF_LABELS, sizeof (label_info));
    compile_info.bytecode_buf = (int32_t*)calloc (MAX_NUMBER_OF_COMMANDS, sizeof (int32_t)); //TODO подумать норм колво памяти нужное
    compile_info.is_cmd = (bool*)calloc (MAX_NUMBER_OF_COMMANDS, sizeof (bool));
    compile_info.num_of_labels = 0;

    read_commands_from_file (&compile_info, code_asm, __FUNCTION__);
    write_pointers_to_strings_in_index (&compile_info, __FUNCTION__);

    for (size_t i = 0; i < compile_info.number_of_strings; i++) {
        printf ("string %zu: %s\n", i+1, (compile_info.index)[i].start_ptr);
    }

    bytecode_header my_bytecode_header = {};

    read_words (&my_bytecode_header, &compile_info);

    write_listing (&compile_info, "listing.lst");

    free (compile_info.buffer);
    free (compile_info.bytecode_buf);
    free (compile_info.is_cmd);

    return 0;
}

size_t read_commands_from_file (info_for_compile* compile_info, const char* filename, const char* caller) {
    assert (compile_info != NULL);
    assert (filename  != NULL);

    get_size_of_file (filename, &(compile_info->bytes_for_code_bin));

    compile_info->file_code_asm = open (filename, O_RDONLY);
    check_of_opening_file (compile_info->file_code_asm, caller);

    compile_info->buffer = (char*)malloc (compile_info->bytes_for_code_bin + 1);
    check_of_memory_allocation (compile_info->buffer, caller);

    compile_info->number_of_read_info = read_file (compile_info->file_code_asm,
                                                   &(compile_info->buffer),
                                                   (int)(compile_info->bytes_for_code_bin),
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
        (compile_info->index)[j - 1].length = (size_t)(&(compile_info->buffer)[compile_info->bytes_for_code_bin] - (compile_info->index)[j - 1].start_ptr);
    }

    return j;
}


size_t count_strings_in_buffer (char* buffer) {
    assert (buffer != NULL);

    size_t num = 1;
    int i = 0;
    while (buffer[i] != '\0') {
        if (buffer[i] == '\n') num++; //TODO подсчет строк здесь
        i++;
    }

    return num;
}

int check_of_opening_file (const int file_code_bin, const char* caller) {
    if (file_code_bin == -1) {
        print_custom_error (ENOENT, caller, __LINE__);
        return -1;
    }
    else {
        printf ("Descriptor of file: %d\n\n", file_code_bin);
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

int read_file (int file_code_bin, char** buffer, int bytes_for_code_bin, const char* caller) {
    ssize_t read_info = read (file_code_bin, *buffer, (int)bytes_for_code_bin);
    if (check_of_reading_file (file_code_bin, read_info, caller) < 0) return -1;
    (*buffer)[read_info] = '\0';

    return (int)read_info;
}

int check_of_reading_file (const int file_code_bin, const ssize_t number_of_read_info, const char* caller) {
    if (number_of_read_info == -1) {
        print_custom_error (EIO, caller, __LINE__);
        close (file_code_bin);
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

int free_text_and_index (string_info* index, char* buffer, size_t bytes_for_code_bin, size_t number_of_strings) {
    memset (index,  0, (number_of_strings + 1) * sizeof (string_info));
    memset (buffer, 0, bytes_for_code_bin + 1);

    free (buffer);
    free (index);

    buffer = NULL;
    index  = NULL;

    return 0;
}

int read_words (bytecode_header* my_bytecode_header, info_for_compile* compile_info) {
    assert (compile_info != NULL);
    assert (compile_info->index != NULL);

    const int HEADER_WORDS = SIZE_OF_SIGNATURE + 4;

    compile_info->instruction_pointer = HEADER_WORDS;
    compile_info->size_of_bytecode    = HEADER_WORDS;

    // ---------- первый проход: только метки ----------
    compile_info->capacity_of_bytecode = MAX_NUMBER_OF_COMMANDS;
    for (size_t line = 0; line < compile_info->number_of_strings; line++) {
        process_line (compile_info, line, /*pass=*/1);
    }

    // ---------- второй проход: генерация кода ----------

    compile_info->instruction_pointer = HEADER_WORDS;
    for (size_t line = 0; line < compile_info->number_of_strings; line++) {
        int result = process_line (compile_info, line, /*pass=*/2);
        if (result < 0) return -1;
    }

    fill_header (compile_info, my_bytecode_header);
    // -------------- инфа из header'а --------------
    fill_with_header_info (my_bytecode_header, compile_info);

    FILE* out = fopen("code.bin", "wb");
    if (!out) {
        perror("code.bin");
        return -1;
    }
    // ------------- запись остального --------------
    size_t written = fwrite(compile_info->bytecode_buf,
                            sizeof(int32_t),
                            compile_info->size_of_bytecode,
                            out);
    fclose(out);

    if (written != compile_info->size_of_bytecode) {
        fprintf(stderr, "short write to code.bin\n");
        return -1;
    }

    return compile_info->instruction_pointer;
}

int process_line (info_for_compile* compile_info, size_t line, int pass) {
    assert (compile_info != NULL);
    assert (compile_info->index != NULL);

    string_info* str = &(compile_info->index[line]);

    // --------------- обрезаем комменты ---------------
    skip_comments(str);

    const char* start  = str->start_ptr;
    size_t      length = str->length;

    if (start == NULL || start[0] == '\0') {
        return 0;
    }

    // ------- пропускаем пробелы, скипаем пустые строки -----
    int no_commands = filter_codestyle (&start, &length);
    if (no_commands) return 0;

    str->start_ptr = start;
    str->length    = length;

    size_t i = 0;
    int j = 0;
    //--------- обработка меток --------
    int label = labels_processing (&i, compile_info, line, start, pass);
    if (label == 1) return 0;
    if (label < 0)  return -1;

    // ---------- имя команды ----------
    char word[16] = {};
    get_name_of_command (&i, &j, compile_info, line, start, word);

    // --------- поиск команды ----------
    int         command_code = -1;
    const char* command_name = NULL;
    search_of_command (&command_code, &command_name, word, pass);

    // ------- пропускаем пробелы -------
    while (i < length && start[i] == ' ') {
        i++;
    }

    int arg = 0;
    // ---------- нет аргумента ----------------
    if (arg == 0) arg = processing_commands_with_no_arguments (&i, compile_info, start, command_code, length, pass);

    // ---------- если аргумент - регистр ----------
    if (arg == 0) arg = processing_commands_with_registers    (&i, compile_info, start, command_name, line, pass);

    // ---------- если аргумент - число ----------
    if (arg == 0) arg = processing_commands_with_numbers      (&i, compile_info, start, command_name, command_code, line, pass);

    // ---------- если аргумент - метка ----------
    if (arg == 0) arg = processing_commands_with_labels       (&i, compile_info, start, command_name, command_code, line, pass);

    // ------------- что-либо другое -------------
    if (arg == 0) {
        printf ("Unknown argument after %s\n", command_name);
        return -1;
    }

    return compile_info->instruction_pointer;
}

int labels_processing (size_t* i, info_for_compile* compile_info, size_t line, const char* start, int pass) {
    assert (i != NULL);
    assert (compile_info != NULL);

    size_t length = compile_info->index[line].length;

    if (start[0] == ':') {
        (*i)++;

        char label_name[MAX_LABEL_NAME_LEN] = {};
        int j = 0;

        while (*i < length && (isalnum((unsigned char)start[*i]) || start[*i] == '_')) {
            label_name[j++] = start[*i];
            (*i)++;
        }
        label_name[j] = '\0';

        if (j == 0) {
            printf ("ERROR: empty label anme on line %zu\n", line + 1);
            return -1;
        }

        if (pass == 1) {
            set_label (compile_info, label_name, compile_info->instruction_pointer);
            printf ("{pass 1} label :%s -> ip %d\n", label_name, compile_info->instruction_pointer);
        }

        return 1;
    }

    return 0;
}

int get_name_of_command (size_t* i, int* j, info_for_compile* compile_info,
                         size_t line, const char* start, char* word) {
    assert (i != NULL);
    assert (j != NULL);
    assert (compile_info != NULL);

    size_t length = compile_info->index[line].length;

    while (*i < length && isalpha((unsigned char)start[*i])) {
        word[*j] = start[*i];
        (*i)++;
        (*j)++;
    }

    word[*j] = '\0';

    if ((*j) == 0) return 0;

    return 0;
}

int search_of_command (int* command_code, const char** command_name, const char* word, int pass) {
    assert (command_code != NULL);
    assert (command_name != NULL);
    assert (word != NULL);

    bool   found = false;
    size_t number_of_codes  = sizeof (code_of_commands) / sizeof (code_of_commands[0]);
    for (size_t k = 0; k < number_of_codes; k++) {
        if (strcmp(word, code_of_commands[k].name_command_assembler) == 0) {
            (*command_code) = code_of_commands[k].code_of_command;
            (*command_name) = code_of_commands[k].name_command_assembler;
            found = true;
            break;
        }
    }

    if (!found) {
        if (pass == 2) {
            printf ("You write in code_asm something else\nI finish my attempt\n");
            return -1;
        }
    }

    return 0;
}

int filter_codestyle (const char** start, size_t* length) {
    while ((**start == ' ' || **start == '\t') && *length > 0) {
        (*start)++;
        if (*length > 0) (*length)--;
    }

    if (*length == 0) {
        return 1;
    }

    return 0;
}

int processing_commands_with_no_arguments (size_t* i, info_for_compile* compile_info, const char* start,
                                           int command_code, size_t length, int pass) {
    assert (i != NULL);
    assert (compile_info != NULL);

    if ((*i) >= length || start[*i] == '\0') {
        if (pass == 2) {
            int32_t code = command_code;
            emit (compile_info, code, true);
        }

        compile_info->instruction_pointer += 1;
        return 1;
    }

    return 0;
}

int processing_commands_with_registers (size_t* i, info_for_compile* compile_info, const char* start,
                                        const char* command_name, size_t line, int pass) {
    assert (i != NULL);
    assert (compile_info != NULL);

    size_t length = compile_info->index[line].length;

    if (*i < length && isalpha((unsigned char)start[*i])) {
        int r = 0;
        char reg_word[10] = {};
        while ((*i) < length && isalpha((unsigned char)start[*i])) {
            reg_word[r] = start[*i];
            (*i)++;
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
        }

        int reg_command_code = 0;
        if (strcmp(command_name, "PUSH") == 0)      reg_command_code = PUSHR; // 9
        else if (strcmp(command_name, "POP") == 0)  reg_command_code = POP;   // 8
        else {
            if (pass == 2) {
                printf ("Command %s doesn't support register argument\n", command_name);
                return -1;
            }
        }

        if (pass == 2) {
            int32_t com_code = reg_command_code;
            int32_t r_num = reg_num;
            emit (compile_info, com_code, true);
            emit (compile_info, r_num, false);
        }

        compile_info->instruction_pointer += 2;
        return 1;
    }

    return 0;
}

int processing_commands_with_numbers (size_t* i, info_for_compile* compile_info, const char* start,
                                      const char* command_name, int command_code, size_t line, int pass) {
    assert (i != NULL);
    assert (compile_info != NULL);

    size_t length = compile_info->index[line].length;

    // --- обработка оперативной памяти [X] ---
    if (*i < length && start[*i] == '[') {
        if (strcmp(command_name, "PUSH") != 0 && strcmp(command_name, "POP") != 0) {
            printf ("Only PUSH and POP support memory argument []\n");
            return -1;
        }

        (*i)++; // пропускаем '['

        // парсим число
        char num_word[10] = {};
        int n = 0;
        while ((*i) < length && isdigit((unsigned char)start[*i])) {
            num_word[n++] = start[*i];
            (*i)++;
        }
        num_word[n] = '\0';

        if (n == 0) {
            printf ("Empty memory address in []\n");
            return -1;
        }

        if (*i >= length || start[*i] != ']') {
            printf ("Expected ']' after memory address\n");
            return -1;
        }
            (*i)++; // пропускаем ']'

            int address = atoi(num_word);

            int actual_code = 0;
            if (strcmp(command_name, "PUSH") == 0) actual_code = PUSHM; // 20
            else if (strcmp(command_name, "POP") == 0) actual_code = POPM; // 21

            if (pass == 2) {
                int32_t act_code = actual_code;
                int32_t add = address;
                emit (compile_info, act_code, true);
                emit (compile_info, add, false);
            }

            compile_info->instruction_pointer += 2;
            return 1;
        }

    // ------- обработка обычных push --------
    if (*i < length && isdigit((unsigned char)start[*i])) {
        if (strcmp(command_name, "POP") == 0) {
            printf ("POP requires a register, not a number\n");
            return -1;
        }

        char num_word[10] = {};
        int  n = 0;
        while ((*i) < length && isdigit((unsigned char)start[*i])) {
            num_word[n] = start[*i];
            (*i)++;
            n++;
        }
        num_word[n] = '\0';

        if (pass == 2) {
            int32_t com_code = command_code;
            int32_t num_w    = atoi(num_word);
            emit (compile_info, com_code, true);
            emit (compile_info, num_w, false);
        }

        compile_info->instruction_pointer += 2;
        return 1;
    }

    return 0;
}

int processing_commands_with_labels (size_t* i, info_for_compile* compile_info, const char* start,
                                    const char* command_name, int command_code, size_t line, int pass) {
    assert (i != NULL);
    assert (compile_info != NULL);

    size_t length = compile_info->index[line].length;

    if (start[*i] == ':' && *i < length) {
        (*i)++;

        char label_name[MAX_LABEL_NAME_LEN] = {};
        int j = 0;

        while ((*i) < length && (isalnum((unsigned char)start[*i]) || start[*i] == '_')) {
            label_name[j++] = start[*i];
            (*i)++;
        }
        label_name[j] = '\0';

        if (j == 0) {
            printf ("Label argument without name after %s\n", command_name);
            return -1;
        }

        int address = find_label_by_name (compile_info, label_name);

        if (address == -1) {
            if (pass == 2) {
                printf ("ERROR: undefined label ':%s' on line %zu\n", label_name, line + 1);
                return -1;
            }

            compile_info->instruction_pointer += 2;
            return 1;
        }

        if (pass == 2) {
            int32_t com_code = command_code;
            int32_t add = address;
            emit (compile_info, com_code, true);
            emit (compile_info, add, false);
        }

        compile_info->instruction_pointer += 2;
        return 1;
    }

    return 0;
}

void skip_comments (string_info* str) {
    assert (str != NULL);

    for (size_t p = 0; p < str->length; p++) {
        if ((str->start_ptr)[p] == ';') {
            str->length = p;
            return;
        }
    }
}

int find_label_by_name (info_for_compile* compile_info, const char* name) {
    for (int i = 0; i < compile_info->num_of_labels; i++) {
        if (strcmp (compile_info->labels[i].name, name) == 0) {
            return compile_info->labels[i].address;
        }
    }

    return -1; //не найдена
}

void set_label (info_for_compile* compile_info, const char* name, int address) {
    for (int i = 0; i < compile_info->num_of_labels; i++) {
        if (strcmp (compile_info->labels[i].name, name) == 0) return;
    }

    strncpy (compile_info->labels[compile_info->num_of_labels].name, name, MAX_LABEL_NAME_LEN - 1);
    compile_info->labels[compile_info->num_of_labels].address = address;
    compile_info->num_of_labels++;
}

void emit (info_for_compile* compile_info, int32_t value, bool cmd) {
    assert (compile_info != NULL);
    assert (compile_info->size_of_bytecode < compile_info->capacity_of_bytecode);
    compile_info->bytecode_buf[compile_info->size_of_bytecode] = value;
    compile_info->is_cmd[compile_info->size_of_bytecode] = cmd;
    compile_info->size_of_bytecode++;
}

const char* mnemonic_by_code (int code) {
    size_t n = sizeof (code_of_commands) / sizeof (code_of_commands[0]);
    for (size_t i = 0; i < n; i++) {
        if (code_of_commands[i].code_of_command == code) {
            return code_of_commands[i].name_command_assembler;
        }
    }

    return NULL;
}

void write_listing (const info_for_compile* compile_info, const char* path) {
    FILE* fp = fopen (path, "w");
    if (fp == NULL) {
        perror (path);
        return;
    }

    const int HEADER_WORDS = SIZE_OF_SIGNATURE + 4; // = 7
    const int32_t* buf = compile_info->bytecode_buf;

    // ---------- блок заголовка ----------
    fprintf (fp, "=== BYTECODE HEADER ===\n");
    fprintf (fp, "signature : %c%c%c\n",
             (char)buf[0], (char)buf[1], (char)buf[2]);
    fprintf (fp, "version   : %u.%u\n",
             (unsigned)buf[3], (unsigned)buf[4]);
    fprintf (fp, "code_size : %u words\n", (unsigned)buf[5]);
    fprintf (fp, "code_hash : 0x%08X\n",  (unsigned)buf[6]);
    fprintf (fp, "\n");

    fprintf (fp, "   off   |  bytecode            |  mnemonic  |  arg\n");
    fprintf (fp, " --------+----------------------+------------+--------\n");

    for (size_t w = HEADER_WORDS; w < compile_info->size_of_bytecode; w++) {
        int32_t word = compile_info->bytecode_buf[w];
        unsigned char* b = (unsigned char*)&word;

        char bytes[32];
        snprintf (bytes, sizeof (bytes), "%02X %02X %02X %02X",
                  b[0], b[1], b[2], b[3]);

        if (compile_info->is_cmd[w]) {
            const char* mnem = mnemonic_by_code (word);

            if (mnem != NULL) {
                fprintf (fp, "  %04zX   |  %-20s|  %-10s|\n",
                         w * 4, bytes, mnem);
            }
            else {
                fprintf (fp, "  %04zX   |  %-20s|  %-10s|  %d\n",
                         w * 4, bytes, "???", word);
            }
        }

        else {
            fprintf (fp, "  %04zX   |  %-20s|  %-10s|  %d\n",
                     w * 4, bytes, "(arg)", word);
        }
    }

    fclose (fp);
}

void fill_header (info_for_compile* compile_info, bytecode_header* my_bytecode_header) {
    assert (compile_info != NULL);
    assert (my_bytecode_header != NULL);

    const size_t HEADER_WORDS = SIZE_OF_SIGNATURE + 4;

    size_t body_words = compile_info->size_of_bytecode - HEADER_WORDS;

    uint32_t hash = dgb2_hash_bytecode (compile_info->bytecode_buf + HEADER_WORDS, body_words);
    printf ("hash: %u\n", hash);

    my_bytecode_header->code_hash = hash;
    my_bytecode_header->code_size = (uint32_t)body_words;

    for (int i = 0; i < SIZE_OF_SIGNATURE; i++) {
        (my_bytecode_header->signature)[i] = BYTECODE_SIGNATURE[i];
    }
    my_bytecode_header->major_version = 1;
    my_bytecode_header->minor_version = 0;
}

void fill_with_header_info (bytecode_header* my_bytecode_header, info_for_compile* compile_info) {
    assert (my_bytecode_header != NULL);
    assert (compile_info       != NULL);

    size_t position = 0;
    for (int i = 0; i < SIZE_OF_SIGNATURE; i++) {
        compile_info->bytecode_buf[position++] = (uint32_t)my_bytecode_header->signature[i];
    }

    compile_info->bytecode_buf[3] = my_bytecode_header->major_version;
    compile_info->bytecode_buf[4] = my_bytecode_header->minor_version;
    compile_info->bytecode_buf[5] = my_bytecode_header->code_size;
    compile_info->bytecode_buf[6] = my_bytecode_header->code_hash;
}
