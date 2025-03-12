#ifndef PLAZMA_LIB_SET_H
#define PLAZMA_LIB_SET_H

#include <stdlib.h>

typedef struct lib_set_t {
    void* data;
    size_t size;
    size_t capacity;
    size_t element_size;
} lib_set_t;

int lib_set_init(lib_set_t* set, size_t capacity, size_t element_size);

size_t lib_set_size(lib_set_t* set);

size_t lib_set_capacity(lib_set_t* set);

size_t lib_set_element_size(lib_set_t* set);

#endif /* PLAZMA_LIB_SET_H */

