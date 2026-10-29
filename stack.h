#ifndef STACK_FUNCS_H
#define STACK_FUNCS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

#define STACK_VERIFY(stk) stack_verify_func (stk, __FUNCTION__, __LINE__)
#define PRINT_ERROR_CONTEXT(stk) print_error_context_func (stk, __FUNCTION__, __LINE__)
#define STACK_DUMP(stk) stack_dump_func (stk, __FUNCTION__, __LINE__)
#define PUSH_AND_CHECK(stk, value) push_and_check_func (stk, value)
#define POP_AND_CHECK(stk) pop_and_check_func (stk)
#define CHECK_OF_PUSHING(stk, err) check_of_pushing_func (stk, err)
#define PRINT_POISON_ERROR(difference) print_poison_error_func  (difference, __FUNCTION__, __LINE__)
#define PRINT_CUSTOM_ERROR(stk, error_code) print_custom_error_func  (stk, error_code)
#define PRINT_CANARY_ERROR(which, expected, got) print_canary_error_func  (which, expected, got, __FUNCTION__, __LINE__)

typedef double stack_elem_t;

const stack_elem_t POISON = 67*67;

const stack_elem_t DATA_CANARY_VALUE1 = (stack_elem_t)0xBEDABE;
const stack_elem_t DATA_CANARY_VALUE2 = (stack_elem_t)0xBEDABED;
const unsigned long long STRUCT_CANARY_VALUE1 = 0xBEDABEDA;
const unsigned long long STRUCT_CANARY_VALUE2 = 0xBEDABEDAA;
const int COEFFICIENT = 2;
const int SHRINK_COEFFICIENT = 3;
const int MIN_CAPACITY = 5;
const int TWO_FOR_CANARIES = 2;
const double ALMOST_ZERO_VALUE = 0.00000001;

#define RED "\033[31m"
#define GREEN    "\033[1;32m"
#define GREEN_BACKGROUND "\033[42m"
#define YELLOW_BACKGROUND "\033[43m"
#define NO_COLOR "\033[0m"
#define SP "%lg"
#define FATAL_ERROR 67

enum stackerr_t {
    STACK_OK = 0,
    ERROR_WITH_MEMORY_ALLOCATION, //1
    ERROR_WITH_MEMORY_REALLOCATION, //2
    ERROR_LEFT_STRUCT_CANARY, //3
    ERROR_RIGHT_STRUCT_CANARY, //4
    ERROR_LEFT_DATA_CANARY, //5
    ERROR_RIGHT_DATA_CANARY, //6
    ERROR_SIZE_LESS_THAN_ZERO, //7
    ERROR_CAPACITY_ZERO, //8
    ERROR_SIZE_EXCEEDS_CAPACITY, //9
    ERROR_DATA_IS_NULL, //10
    ERROR_STACK_IS_NULL, //11
    STACK_UNDERFLOW, //12
    ERROR_HASH_MISMATCH, //13
    ERROR_WITH_OPENING_FILE, //14
    ERROR //15
};


struct error_map {
    int         error_code;
    const char* error_message;
};

const struct error_map errors[] = {
    { ERROR_WITH_MEMORY_ALLOCATION,   "memory allocation failed" },
    { ERROR_WITH_MEMORY_REALLOCATION, "memory reallocation failed" },
    { ERROR_LEFT_STRUCT_CANARY,       "left struct canary damaged" },
    { ERROR_RIGHT_STRUCT_CANARY,      "right struct canary damaged" },
    { ERROR_LEFT_DATA_CANARY,         "left data canary damaged" },
    { ERROR_RIGHT_DATA_CANARY,        "right data canary damaged" },
    { ERROR_SIZE_LESS_THAN_ZERO,      "stack size is less than zero" },
    { ERROR_CAPACITY_ZERO,            "stack capacity is zero" },
    { ERROR_SIZE_EXCEEDS_CAPACITY,    "size exceeds capacity" },
    { ERROR_DATA_IS_NULL,             "data pointer is NULL" },
    { ERROR_STACK_IS_NULL,            "stack pointer is NULL" },
    { STACK_UNDERFLOW,                "stack underflow: can't pop from empty stack" },
    { ERROR_HASH_MISMATCH,            "stack hash mismatch: structure damaged" },
    { ERROR_WITH_OPENING_FILE,        "fopen return NULL" },
    { ERROR,                          "generic error" },
};

struct stack_t {
    unsigned long long canary_left;
    stack_elem_t*      data;
    ssize_t            size;
    ssize_t            capacity;
    uint32_t           hash;
    unsigned long long canary_right;
};

stackerr_t run_all_tests ();

#ifndef NDEBUG
    #define STACK_ASSERT_DBG(stk) assert(stack_verify_func (stk, __FUNCTION__, __LINE__) == 0) //TODO разные режимы
#else
    #define STACK_ASSERT_DBG(stk)
#endif

#define STACK_ASSERT(stk)                                                               \
    do {                                                                                \
        if (stack_verify_func ((stk), __FUNCTION__, __LINE__) != 0) {                   \
            fprintf (stderr, "Assert failed in %s, line %d\n", __FUNCTION__, __LINE__); \
        }                                                                               \
    } while (0)

stackerr_t   stack_init         (stack_t* stk);
stackerr_t   stack_push         (stack_t* stk, stack_elem_t value);
stackerr_t   stack_pop          (stack_t* stk);
stackerr_t   realloc_stack_up   (stack_t* stk);
stackerr_t   realloc_stack_down (stack_t* stk);
stackerr_t   stack_destroy      (stack_t* stk);
stack_elem_t stack_top          (const stack_t* stk);
void         stack_copy         (stack_t* stk_copy, stack_t* stk);

int  stack_verify_func        (const stack_t* stk, const char* func, int line);
void stack_dump_func          (const stack_t* stk, const char* func, int line);
void print_data_elements      (FILE* log_file, const stack_elem_t* data, size_t capacity, size_t size);
void print_custom_error_func  (const stack_t* stk, int error_code);// TODO not define
void print_error_context_func (const stack_t* stk, const char* func, int line);
void push_and_check_func      (stack_t* stk, stack_elem_t value);
void pop_and_check_func       (stack_t* stk);
void check_of_pushing_func    (const stack_t* stk, int err);
void poison_memset            (stack_elem_t* data, size_t from, size_t to);
int  poison_check             (const stack_t* stk);
void print_poison_error_func  (int difference, const char* func, int line);

stackerr_t check_first_data_canary    (stack_elem_t canary);
stackerr_t check_second_data_canary   (stack_elem_t canary);
stackerr_t check_first_struct_canary  (unsigned long long canary);
stackerr_t check_second_struct_canary (unsigned long long canary);
void       print_canary_error_func    (const char* which, unsigned long long expected,
                                       unsigned long long got, const char* func, int line);
bool equal_double (stack_elem_t n1, stack_elem_t n2);

uint32_t   hash_one_byte     (uint32_t h, uint8_t byte);
uint32_t   hash_u64          (uint32_t h, uint64_t value);
uint32_t   dgb2_hash         (const stack_t* stk);
void       stack_update_hash (stack_t* stk);

#endif
