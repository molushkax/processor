#include "hash.h"

uint32_t dgb2_hash_bytecode (const int32_t* bytecode_buf, size_t size) {
    assert (bytecode_buf != NULL);

    uint32_t h = 0;
    for (size_t i = 0; i < size; i++) {
        uint64_t value_bits = 0;
        memcpy (&value_bits, &bytecode_buf[i], sizeof (int32_t));
        h = hash_u64_bytecode (h, value_bits);
    }

    return h;
}

// замешивает один байт в текущее значение хэша: h = h * 33 + byte.
uint32_t hash_one_byte_bytecode (uint32_t h, uint8_t byte) {
    return h * 33 + byte;
}

// замешивает 64-битное число в хэш, идя по его байтам.
uint32_t hash_u64_bytecode (uint32_t h, uint64_t value) {
    for (size_t i = 0; i < sizeof (value); i++) {
        h = hash_one_byte_bytecode (h, (uint8_t)(value >> (8 * i))); //побитовый сдвиг вправо
    }

    return h;
}
