#ifndef BYTECODE_H
#define BYTECODE_H

#include <stdint.h>

#define SIZE_OF_SIGNATURE 3

const uint32_t BYTECODE_SIGNATURE[SIZE_OF_SIGNATURE] = {'M', 'O', 'L'};
const uint32_t MAJOR_VERSION = 1;
const uint32_t MINOR_VERSION = 0;

struct bytecode_header {
    uint32_t signature[SIZE_OF_SIGNATURE];
    uint32_t major_version;
    uint32_t minor_version;
    uint32_t code_size;
    uint32_t code_hash;
};

#endif
