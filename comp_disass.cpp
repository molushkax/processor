#include "comp_disass.h"
#include "commands.h"

int main () {
    const char* code_bin = "code.bin";

    info_for_disass disass_info = {};

    if (read_bytecode_for_disass (&disass_info, code_bin, __FUNCTION__) < 0) {
        return 1;
    }

    write_disassembly (&disass_info, __FUNCTION__);

    free_bytecode_for_disass (&disass_info);

    return 0;
}

int read_bytecode_for_disass (info_for_disass* disass_info, const char* code_bin, const char* caller) {
    assert (disass_info != NULL);
    assert (code_bin    != NULL);

    FILE* fp = fopen (code_bin, "rb");
    if (check_of_fopening_file (fp, caller) < 0) return -1;

    if (get_size_of_bytecode_file (fp, &(disass_info->bytes_for_bytecode), caller) < 0) {
        fclose (fp);
        return -1;
    }

    if (check_bytecode_size_alignment (disass_info->bytes_for_bytecode, caller) < 0) {
        fclose (fp);
        return -1;
    }

    disass_info->number_of_cells = (int)(disass_info->bytes_for_bytecode / sizeof (uint32_t));

    disass_info->buffer_with_commands = (int32_t*)calloc (disass_info->number_of_cells, sizeof (uint32_t));
    if (check_of_memory_allocation ((char*)disass_info->buffer_with_commands, caller) < 0) {
        fclose (fp);
        return -1;
    }

    if (read_bytecode_cells (fp, disass_info, caller) < 0) {
        fclose (fp);
        free_bytecode_for_disass (disass_info);
        return -1;
    }

    fclose (fp);
    return 0;
}

int get_size_of_bytecode_file (FILE* fp, size_t* bytes_for_bytecode, const char* caller) {
    assert (fp                 != NULL);
    assert (bytes_for_bytecode != NULL);

    if (fseek (fp, 0, SEEK_END) != 0) {
        print_custom_error (EIO, caller, __LINE__);
        return -1;
    }

    long size = ftell (fp);
    if (size < 0) {
        print_custom_error (EIO, caller, __LINE__);
        return -1;
    }

    if (fseek (fp, 0, SEEK_SET) != 0) {
        print_custom_error (EIO, caller, __LINE__);
        return -1;
    }

    *bytes_for_bytecode = (size_t)size;
    printf ("Number of bytes for bytecode: %zu\n\n", *bytes_for_bytecode);
    return 0;
}

int check_bytecode_size_alignment (size_t bytes_for_bytecode, const char* caller) {
    if (bytes_for_bytecode == 0) {
        fprintf (stderr, "bytecode file is empty\n");
        return -1;
    }

    if (bytes_for_bytecode % sizeof (uint32_t) != 0) {
        fprintf (stderr, "bytecode size %zu is not a multiple of %zu\n",
                 bytes_for_bytecode, sizeof (uint32_t));
        return -1;
    }

    return 0;
}

int read_bytecode_cells (FILE* fp, info_for_disass* disass_info, const char* caller) {
    assert (fp          != NULL);
    assert (disass_info != NULL);

    size_t got = fread (disass_info->buffer_with_commands,
                        sizeof (uint32_t),
                        (size_t)(disass_info->number_of_cells),
                        fp);

    if ((int)got != disass_info->number_of_cells) {
        fprintf (stderr, "short read: %zu of %d cells\n", got, disass_info->number_of_cells);
        return -1;
    }

    printf ("Number of cells in bytecode: %d\n\n", disass_info->number_of_cells);
    return 0;
}

int free_bytecode_for_disass (info_for_disass* disass_info) {
    assert (disass_info != NULL);

    free (disass_info->buffer_with_commands);
    disass_info->buffer_with_commands = NULL;
    disass_info->number_of_cells      = 0;
    disass_info->bytes_for_bytecode   = 0;

    return 0;
}

int write_disassembly (info_for_disass* disass_info, const char* caller) {
    assert (disass_info != NULL);

    int ip = 0;
    while (ip < disass_info->number_of_cells) {
        int next = disass_one (disass_info, ip, caller);
        if (next <= ip) {
            fprintf (stderr, "disass: stuck at ip=%d\n", ip);
            return -1;
        }
        ip = next;
    }

    return 0;
}

int disass_one (info_for_disass* disass_info, int ip, const char* caller) {
    assert (disass_info != NULL);
    assert (ip >= 0 && ip < disass_info->number_of_cells);

    int opcode       = (disass_info->buffer_with_commands)[ip];
    const char* name = cmd_name (opcode);

    if (name == NULL) {
        printf ("%04d:  <unknown opcode %d>\n", ip, opcode);
        return ip + 1;
    }

    int cells = cells_for_opcode (opcode);

    if (cells == 1) {
        printf ("%04d:  %s\n", ip, name);
        return ip + 1;
    }

    if (cells == 0) {
        printf ("%04d:  <unknown opcode %d>\n", ip, opcode);
        return ip + 1;
    }

    if (ip + 1 >= disass_info->number_of_cells) {
        printf ("%04d:  %s <truncated, no argument>\n", ip, name);
        return disass_info->number_of_cells;
    }

    uint32_t arg = (disass_info->buffer_with_commands)[ip + 1];

    if (opcode == PUSHR || opcode == POP) {
        printf ("%04d:  %-5s %s\n", ip, name, reg_name_by_num (arg));
        return ip + 2;
    }

    if (opcode == PUSHM || opcode == POPM) {
        printf ("%04d:  %-5s [%d]\n", ip, name, arg);
        return ip + 2;
    }

    if (opcode == PUSH) {
        printf ("%04d:  %-5s %d\n", ip, name, arg);
        return ip + 2;
    }

    printf ("%04d:  %-5s <%d>\n", ip, name, arg);
    return ip + 2;
}

const char* cmd_name (int code) {
    size_t n = sizeof (code_of_commands) / sizeof (code_of_commands[0]);
    for (size_t i = 0; i < n; i++) {
        if (code_of_commands[i].code_of_command == code) {
            return code_of_commands[i].name_command_assembler;
        }
    }
    return NULL;
}

const char* reg_name_by_num (int reg_num) {
    size_t n = sizeof (disass_registers) / sizeof (disass_registers[0]);
    for (size_t i = 0; i < n; i++) {
        if (disass_registers[i].num_among_registers == reg_num) {
            return disass_registers[i].name_of_register;
        }
    }
    return "?";
}

int cells_for_opcode (int opcode) {
    switch (opcode) {
        case HLT: case ADD:
        case SUB: case MUL:
        case DIV: case OUT:
        case IN:  case RET:
        case SQRT:
            return 1;
        case PUSH: case PUSHR:
        case POP:  case JMP:
        case CALL: case JA:
        case JAE:  case JB:
        case JBE:  case JE:
        case JNE:  case PUSHM:
        case POPM:
            return 2;
        default:
            return 0;
    }
}

int check_of_fopening_file (FILE* f, const char* caller) {
    if (f == NULL) {
        print_custom_error (ENOENT, caller, __LINE__);
        return -1;
    }
    return 0;
}

int check_of_memory_allocation (const char* buffer, const char* caller) {
    if (buffer == NULL) {
        print_custom_error (ENOMEM, caller, __LINE__);
        return -1;
    }
    return 0;
}

void print_custom_error (int error_code, const char* function_name, int line) {
    const char* message = "Error";

    fprintf (stderr, "ERROR %d %s in function <<%s>> in line %d\n",
             error_code, message, function_name, line);
}
