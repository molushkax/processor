#ifndef BYTECODE_H
#define BYTECODE_H

#include <stdint.h>

const uint8_t BYTECODE_SIGNATURE[3] = {'M', 'O', 'L'};

const uint32_t MAJOR_VERSION = 1;
const uint32_t MINOR_VERSION = 0;

struct bytecode_header {
    uint8_t  signature[3];
    uint32_t major_version;
    uint32_t minor_version;
    uint32_t code_size;
    uint32_t code_hash;
}

#endif
