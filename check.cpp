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
