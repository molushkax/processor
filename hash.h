#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "commands.h"

uint32_t    dgb2_hash_bytecode     (const int32_t* bytecode_buf, size_t size);
uint32_t    hash_u64_bytecode      (uint32_t h, uint64_t value);
uint32_t    hash_one_byte_bytecode (uint32_t h, uint8_t byte);

#endif
