#ifndef PLAZMA_LIB_LIST_H
#define PLAZMA_LIB_LIST_H

#include <stdlib.h>

typedef struct lib_list_t {
    void* data;
    size_t size;
    size_t capacity;
    size_t element_size;
} lib_list_t;

int lib_list_init(lib_list_t* list, size_t capacity, size_t element_size);

size_t lib_list_size(lib_list_t* list);

size_t lib_list_capacity(lib_list_t* list);

size_t lib_list_element_size(lib_list_t* list);

#endif /* PLAZMA_LIB_LIST_H */

