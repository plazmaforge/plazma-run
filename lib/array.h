#ifndef PLAZMA_LIB_ARRAY_H
#define PLAZMA_LIB_ARRAY_H

#include <stdlib.h>
#include "data.h"

#define LIB_ARRAY_CLASS_STRUCT(S, T)               \
typedef struct S {                                 \
    T* data;                                       \
    size_t size;                                   \
} S;                                               \

#define LIB_ARRAY_CLASS_INIT(C, S, T)              \
int C##_init(S* array, size_t size) {              \
    if (!array) {                                  \
        return -1;                                 \
    }                                              \
    array->size = size;                            \
    array->data = lib_mallocz(size * sizeof(T));   \
    if (!array->data) {                            \
        return -1;                                 \
    }                                              \
    return 0;                                      \
}                                                  \

#define LIB_ARRAY_CLASS_GET(C, S, T)               \
T C##_get(S* array, size_t index) {                \
    return array->data[index];                     \
}                                                  \

#define LIB_ARRAY_CLASS_SET(C, S, T)               \
void C##_set(S* array, size_t index, T value) {    \
    array->data[index] = value;                    \
}                                                  \

#define LIB_ARRAY_CLASS_FREE(C, S, T)              \
void C##_free(S* array) {                          \
    if (!array || !array->data) {                  \
        return;                                    \
    }                                              \
    free(array->data);                             \
    array->data = NULL;                            \
}                                                  \

////

#define LIB_ARRAY_CLASS(C, S, T)                   \
LIB_ARRAY_CLASS_STRUCT(S, T)                       \
LIB_ARRAY_CLASS_INIT(C, S, T)                      \
LIB_ARRAY_CLASS_GET(C, S, T)                       \
LIB_ARRAY_CLASS_SET(C, S, T)                       \
LIB_ARRAY_CLASS_FREE(C, S, T)                      \

////

#define LIB_ARRAY_TYPE(T) LIB_ARRAY_CLASS(lib_##T##_array, lib_##T##_array_t, T)
#define LIB_ARRAY_TYPE_PTR(T) LIB_ARRAY_CLASS(lib_##T##_ptr_array, lib_##T##_ptr_array_t, T*)

////

typedef struct lib_array_t {
    void* data;
    size_t size;
    size_t value_size;
    int mem_type;
} lib_array_t;

int lib_array_init(lib_array_t* array, size_t size, size_t value_size);

int lib_array_init_ptr(lib_array_t* array, size_t size);

int lib_array_init_val(lib_array_t* array, size_t size, size_t value_size);

void lib_array_free(lib_array_t* array);

void lib_array_free_all(lib_array_t* array);

size_t lib_array_size(lib_array_t* array);

void* lib_array_get(lib_array_t* array, size_t index);

int lib_array_set(lib_array_t* array, size_t index, void* value);

#endif /* PLAZMA_LIB_ARRAY_H */

