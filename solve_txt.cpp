#include "stack.h"

#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int get_size_of_file (const char* code_txt, size_t* bytes_for_code_txt);

int main () {
// TODO from .ass to .text
    const char* code_txt = "code.txt";

    size_t bytes_for_code_txt = 0;
    get_size_of_file (code_txt, &bytes_for_code_txt);

    int* buffer = (int*)calloc(bytes_for_code_txt, sizeof(int));
    int file_code_txt = open (code_txt, O_RDONLY);
    ssize_t read_info = read (file_code_txt, (void*)buffer, (int)bytes_for_code_txt+1);

    printf ("%d\n", buffer[0]);
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
