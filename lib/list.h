#ifndef PLAZMA_LIB_LIST_H
#define PLAZMA_LIB_LIST_H

#include <stdlib.h>

typedef struct lib_list_t {
    void* data;
    size_t size;
    size_t capacity;
    size_t value_size;
    int mem_type;
} lib_list_t;

int lib_list_init(lib_list_t* list, size_t capacity, size_t value_size);

int lib_list_init_ptr(lib_list_t* list, size_t size);

int lib_list_init_val(lib_list_t* list, size_t size, size_t value_size);

void lib_list_free(lib_list_t* list);

void lib_list_free_all(lib_list_t* list);

size_t lib_list_size(lib_list_t* list);

size_t lib_list_capacity(lib_list_t* list);

size_t lib_list_value_size(lib_list_t* list);

void* lib_list_get(lib_list_t* list, size_t index);

int lib_list_set(lib_list_t* list, size_t index, void* value);

int lib_list_add(lib_list_t* list, void* value);

#endif /* PLAZMA_LIB_LIST_H */

