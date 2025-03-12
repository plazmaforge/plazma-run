#ifndef PLAZMA_LIB_STACK_H
#define PLAZMA_LIB_STACK_H

#include <stdlib.h>

typedef struct lib_stack_t {
    void* data;
    size_t size;
    size_t capacity;
    size_t element_size;
} lib_stack_t;

int lib_stack_init(lib_stack_t* stack, size_t capacity, size_t element_size);

size_t lib_stack_size(lib_stack_t* stack);

size_t lib_stack_capacity(lib_stack_t* stack);

size_t lib_stack_element_size(lib_stack_t* stack);

#endif /* PLAZMA_LIB_STACK_H */

