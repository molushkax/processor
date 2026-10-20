#include "stack.h"
#include "commands.h"
#include "vm.h"

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
    stack_destroy (&(processor.stk));
    stack_destroy (&(processor.call_stk));

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

    processor->call_stk = {};
    (processor->call_stk).capacity = 5;
    stack_init (&(processor->call_stk));

    processor->instruction_pointer = 0;
    stack_elem_t answer = 0;

    while (processor->instruction_pointer < processor->number_of_int_for_buf) {
        int c = getchar ();
        dump_buffer_with_commands (processor);
        printf ("\ncommand number %d:\n\n", (processor->buffer_with_commands)[processor->instruction_pointer]);

        switch ((processor->buffer_with_commands)[processor->instruction_pointer]) {
            case PUSH: push_command (processor); break;
            case POP:  pop_command  (processor); break;
            case ADD:  add_command  (processor); break;
            case SUB:  sub_command  (processor); break;
            case MUL:  mul_command  (processor); break;
            case DIV:  div_command  (processor); break;
            case JMP:  jmp_command  (processor); break;
            case JA:   ja_command   (processor); break;
            case JAE:  jae_command  (processor); break;
            case JB:   jb_command   (processor); break;
            case JBE:  jbe_command  (processor); break;
            case JE:   je_command   (processor); break;
            case JNE:  jne_command  (processor); break;
            case CALL: call_command (processor); break;
            case RET:  ret_command  (processor); break;
            case IN:   in_command   (processor); break;
            case SQRT: sqrt_command (processor); break;
            case OUT:  out_command  (processor, &answer); break;
            case HLT:  return answer;
            default:   fprintf (stderr, "Unknown command\n"); return -1;
        }

        dump_stack (processor, __FUNCTION__, __LINE__);
    }

    return 0;
}

stack_elem_t pop_value(processor_t* processor) {
    STACK_ASSERT(&(processor->stk));

    stack_elem_t val = stack_top(&(processor->stk));
    POP_AND_CHECK(&(processor->stk));
    return val;
}

void push_command (processor_t* processor) {
    assert (processor != NULL);

    PUSH_AND_CHECK(&(processor->stk),
                   (processor->buffer_with_commands)[processor->instruction_pointer + 1]);
    processor->instruction_pointer += 2;
}

void pop_command (processor_t* processor) {
    assert (processor != NULL);

    int reg_num = (processor->buffer_with_commands)[processor->instruction_pointer + 1];

    stack_elem_t value = pop_value (processor);

    for (int i = 0; i < 4; i++) {
        if (processor->array_of_registers[i].num_among_registers == reg_num) {
            processor->array_of_registers[i].value_of_reg = value;
            break;
        }
    }

    processor->instruction_pointer += 2;
}

void add_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value1 = pop_value (processor);
    stack_elem_t value2 = pop_value (processor);
    stack_elem_t result = value2 + value1;

    PUSH_AND_CHECK (&(processor->stk), result);
    processor->instruction_pointer += 1;
}

void sub_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value1 = pop_value (processor);
    stack_elem_t value2 = pop_value (processor);
    stack_elem_t result = value2 - value1;

    PUSH_AND_CHECK (&(processor->stk), result);
    processor->instruction_pointer += 1;
}

void mul_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value1 = pop_value (processor);
    stack_elem_t value2 = pop_value (processor);
    stack_elem_t result = value2 * value1;

    PUSH_AND_CHECK (&(processor->stk), result);
    processor->instruction_pointer += 1;
}

void div_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value1 = pop_value (processor);
    stack_elem_t value2 = pop_value (processor);
    if (equal_double(value1, 0)) {
        fprintf (stderr, "You're trying to do: " SP "/" SP " ??????\nDIV by zero is not allowed.\n",
                 value2, value1);
        abort();
    }
    stack_elem_t result = value2 / value1;//TODO

    PUSH_AND_CHECK (&(processor->stk), result);
    processor->instruction_pointer += 1;
}

void out_command (processor_t* processor, stack_elem_t* answer) {
    assert (processor != NULL);

    *answer = stack_top (&(processor->stk));
    STACK_DUMP (&(processor->stk));
    processor->instruction_pointer += 1;
}

void peek_two_values (processor_t* processor,
                      stack_elem_t* value2,
                      stack_elem_t* value1) {
    assert (processor != NULL);
    assert (value1   != NULL);
    assert (value2   != NULL);

    assert ((processor->stk).size >= 2);

    *value1 = (processor->stk).data[(processor->stk).size - 1];   // вершина
    *value2 = (processor->stk).data[(processor->stk).size - 2];   // второй сверху
}

void jmp_command (processor_t* processor) {
    assert (processor != NULL);

    int target = (processor->buffer_with_commands)[processor->instruction_pointer + 1];

    if (target < 0 || target >= processor->number_of_int_for_buf) {
        fprintf (stderr, "JMP: invalid target address %d\n", target);
        processor->instruction_pointer = processor->number_of_int_for_buf - 1;
        return;
    }

    processor->instruction_pointer = target;
}

void ja_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value2 = 0, value1 = 0;
    peek_two_values (processor, &value2, &value1);

    conditional_jump (processor, value2 > value1);
}

void jae_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value2 = 0, value1 = 0;
    peek_two_values (processor, &value2, &value1);

    conditional_jump (processor, value2 >= value1);
}

void jb_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value2 = 0, value1 = 0;
    peek_two_values (processor, &value2, &value1);

    conditional_jump (processor, value2 < value1);
}

void jbe_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value2 = 0, value1 = 0;
    peek_two_values (processor, &value2, &value1);

    conditional_jump (processor, value2 <= value1);
}

void je_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value2 = 0, value1 = 0;
    peek_two_values (processor, &value2, &value1);

    conditional_jump (processor, equal_double(value2, value1));
}

void jne_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value2 = 0, value1 = 0;
    peek_two_values (processor, &value2, &value1);

    conditional_jump (processor, !equal_double(value2, value1));
}

void conditional_jump (processor_t* processor, bool condition) {
    assert (processor != NULL);

    int target = (processor->buffer_with_commands)[processor->instruction_pointer + 1];

    if (condition) {
        if (target < 0 || target >= processor->number_of_int_for_buf) {
            fprintf (stderr, "conditional jump: invalid target %d\n", target);
            processor->instruction_pointer = processor->number_of_int_for_buf;
            return;
        }

        processor->instruction_pointer = target;
    }

    else {
        processor->instruction_pointer += 2;
    }
}

void call_command (processor_t* processor) {
    assert (processor != NULL);

    int target       = (processor->buffer_with_commands)[processor->instruction_pointer + 1];
    int return_addr  = processor->instruction_pointer + 2;   // адрес следующей команды

    if (target < 0 || target >= processor->number_of_int_for_buf) {
        fprintf (stderr, "CALL: invalid target address %d\n", target);
        processor->instruction_pointer = processor->number_of_int_for_buf;
        return;
    }

    PUSH_AND_CHECK (&(processor->call_stk), return_addr);    // запомнить возврат
    processor->instruction_pointer = target;                 // обратно
}

void ret_command (processor_t* processor) {
    assert (processor != NULL);

    if ((processor->call_stk).size == 0) {
        fprintf (stderr, "RET: call stack is empty\n");
        processor->instruction_pointer = processor->number_of_int_for_buf;
        return;
    }

    stack_elem_t return_addr = 0;

    return_addr = stack_top (&(processor->call_stk));
    POP_AND_CHECK (&(processor->call_stk));

    processor->instruction_pointer = int(return_addr);
}

void in_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value = 0;
    int scanf_result = 0;

    do {
        printf ("Enter number: ");
        scanf_result = scanf ( SP , &value);
        int c = 0;
        while ((c = getchar()) != '\n' && c != EOF) { }

    } while (scanf_result != 1);

    PUSH_AND_CHECK (&(processor->stk), value);

    processor->instruction_pointer += 1;
}

void sqrt_command (processor_t* processor) {
    assert (processor != NULL);

    stack_elem_t value = pop_value (processor);
    stack_elem_t sqrt_value = sqrt (value);

    PUSH_AND_CHECK (&(processor->stk), sqrt_value);

    processor->instruction_pointer += 1;
}

void dump_stack (processor_t* processor, const char* func, int line) {
    assert (processor != NULL);

    printf ("File:      %s\n", __FILE__);
    printf ("Function:  %s\n", func);
    printf ("Line:      %d\n", line);
    printf ("Struct:    stack_t at [%p]\n\n", (const void*)(&(processor->stk)));

    printf ("struct stack_t {\n");
    printf ("    struct address: %p\n", (const void*)(&(processor->stk)));
    printf ("    data address:   %p\n", (const void*)(processor->stk).data);

    if ((processor->stk).data != NULL && (processor->stk).capacity > 0)
        print_data_elements_vm ((processor->stk).data, (processor->stk).capacity, (processor->stk).size);

    printf ("\n    size:           %zd\n", (processor->stk).size);
    printf ("    capacity:       %zd\n", (processor->stk).capacity);

    #ifndef NDEBUG
        printf ("    canary left:    0x%llX ", (processor->stk).canary_left);
        stackerr_t left_canary_check = check_first_struct_canary ((processor->stk).canary_left);
        if (left_canary_check == STACK_OK) printf (" (ALIVE)\n");
        else printf ("(NEED YOUR HELP, PROGRAMMER)\n");

        printf ("    canary right:   0x%llX ", (processor->stk).canary_right);
        stackerr_t right_canary_check = check_second_struct_canary ((processor->stk).canary_right);
        if (right_canary_check == STACK_OK) printf ( "(ALIVE)\n");
        else printf ("(NEED YOUR HELP, PROGRAMMER)\n");

        uint32_t expected_hash = dgb2_hash (&(processor->stk));
        printf ("    hash:           0x%08X (stored 0x%08X) ",
                 expected_hash, (processor->stk).hash);

        if ((processor->stk).hash == expected_hash)
            printf ("(HASH IS VALID)\n");
        else
            printf ("(HASH MISMATCH - STRUCTURE DAMAGED)\n");
    #endif

    printf ("}\n");

    if ((processor->stk).size > (processor->stk).capacity)
        printf ("WARNING: size > capacity\n");
    if ((processor->stk).size < 0)
        printf ("Stack has unusual size...\n");
    if ((processor->stk).data == NULL)
        printf ("ERROR: data pointer is NULL\n");
}

void print_data_elements_vm (const stack_elem_t* data, size_t capacity, size_t size) {
    assert (data != NULL);

    #ifndef NDEBUG
        if (size != 0) printf ("\n    data in data:\n");
    #else
        printf ("\n    data in data:\n");
    #endif

    #ifndef NDEBUG
        printf ("        [-1] canary: <" SP "> ", data[-1]);
        stackerr_t left_canary_check = check_first_data_canary (data[-1]);
        if (left_canary_check == STACK_OK) printf ("(ALIVE)\n");
        else printf ("(NEED YOUR HELP, PROGRAMMER)\n");
    #endif

    for (size_t i = 0; i < capacity; i++)
    {
        if (i < size)
            printf ("       *[%zd] element: <" SP ">\n", i, data[i]);
        else {
            #ifndef NDEBUG
            printf ("        [%zd] element: <" SP "> POISON\n", i, data[i]);
            #endif
        }
    }

    #ifndef NDEBUG
        printf ("        [%zd] canary:  <" SP"> ", capacity, data[capacity]);
        stackerr_t right_canary_check = check_second_data_canary (data[capacity]);
        if (right_canary_check == STACK_OK) printf ("(ALIVE)\n");
        else printf ("(NEED YOUR HELP, PROGRAMMER)\n");
    #endif
}

void dump_buffer_with_commands (processor_t* processor) {
    assert (processor != NULL);

    printf ("==================================================");
    printf ("\nbuffer_with commands: (" GREEN_BACKGROUND "green" NO_COLOR " command is doing now) \n\n");

    printf ("number of cell in buffer: ");
    for (int i = 0; i < processor->number_of_int_for_buf; i++) {
        if (i == processor->instruction_pointer) {
            printf (GREEN_BACKGROUND "%3d" NO_COLOR, i);
        }

        else printf ("%3d", i);
    }
    printf ("\n");

    printf ("value of cell in buffer:  ");
    for (int i = 0; i < processor->number_of_int_for_buf; i++) {
        printf ("%3d", (processor->buffer_with_commands)[i]);
    }
    printf ("\n");
}
