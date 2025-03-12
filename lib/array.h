#ifndef PLAZMA_LIB_ARRAY_H
#define PLAZMA_LIB_ARRAY_H

#include <stdlib.h>

#include "clt.h"

typedef struct lib_array_t {
    void* data;
    size_t size;
    size_t element_size;
    int mem_type;
} lib_array_t;

int lib_array_init(lib_array_t* array, size_t size, size_t element_size);

int lib_array_init_cast(lib_array_t* array, size_t size);

size_t lib_array_size(lib_array_t* array);

void* lib_array_get(lib_array_t* array, size_t index);

int lib_array_set(lib_array_t* array, size_t index, void* element);

#endif /* PLAZMA_LIB_ARRAY_H */

