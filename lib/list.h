#ifndef PLAZMA_LIB_LIST_H
#define PLAZMA_LIB_LIST_H

#include <stdlib.h>

#define LIB_LIST_CLASS_STRUCT(S, T)                \
typedef struct S {                                 \
    T* data;                                       \
    size_t size;                                   \
    size_t capacity;                               \
} S;                                               \

#define LIB_LIST_CLASS_INIT(C, S, T)               \
int C##_init(S* list, size_t capacity) {           \
    if (!list) {                                   \
        return -1;                                 \
    }                                              \
    list->size = 0;                                \
    list->capacity = LIB_MAX(LIB_DATA_CAPACITY_MIN, capacity);\
    list->data = lib_mallocz(capacity * sizeof(T));\
    if (!list->data) {                             \
        return -1;                                 \
    }                                              \
    return 0;                                      \
}                                                  \

#define LIB_LIST_CLASS_FREE(C, S, T)               \
void C##_free(S* list) {                           \
    if (!list || !list->data) {                    \
        return;                                    \
    }                                              \
    free(list->data);                              \
    list->data = NULL;                             \
}                                                  


#define LIB_LIST_CLASS_GET(C, S, T)                \
T C##_get(S* list, size_t index) {                 \
    return list->data[index];                      \
}                                                  \

#define LIB_LIST_CLASS_SET(C, S, T)                \
void C##_set(S* list, size_t index, T value) {     \
    list->data[index] = value;                     \
}                                                  \

#define LIB_LIST_CLASS_ADD(C, S, T)                \
void C##_add(S* list, T value) {                   \
    if (!C##_try_realloc(list)) {                  \
        return;                                    \
    }                                              \
    C##_set(list, list->size, value);              \
    list->size++;                                  \
}                                                  \

#define LIB_LIST_CLASS_INSERT(C, S, T)             \
void C##_insert(S* list, size_t index, T value) {  \
    if (!C##_try_realloc(list)) {                  \
        return;                                    \
    }                                              \
    lib_data_move_next(list->data, list->size, sizeof(T), index); \
    C##_set(list, index, value);                   \
    list->size++;                                  \
}                                                  \

#define LIB_LIST_CLASS_REMOVE_INDEX(C, S, T)       \
void C##_remove_index(S* list, size_t index) {      \
    lib_data_move_prev(list->data, list->size, sizeof(T), index); \
    lib_data_reset(list->data, list->size - 1, sizeof(T));        \
    list->size--;                                  \
}                                                  \

#define LIB_LIST_CLASS_REMOVE(C, S, T)             \
void C##_remove(S* list, T value) {                \
    size_t index;                                  \
    if (C##_find(list, value, &index) != 0) {      \
        return;                                    \
    }                                              \
    C##_remove_index(list, index);                 \
}                                                  \


#define LIB_LIST_CLASS_PRIVATE(C, S, T)            \
static void* C##_realloc(S* list) {                \
    size_t capacity = lib_data_new_capacity(list->capacity);        \
    void* data = lib_data_realloc(list->data, capacity, sizeof(T)); \
    if (!data) {                                   \
        return NULL;                               \
    }                                              \
    list->data = data;                             \
    list->capacity = capacity;                     \
    return data;                                   \
}                                                  \
                                                   \
static bool C##_try_realloc(S* list) {             \
    if (!lib_data_need_realloc(list->size, list->capacity)) {       \
        return true;                               \
    }                                              \
    return C##_realloc(list);                      \
}                                                  \
                                                   \
static int C##_find(S* list, T value, size_t* index) {              \
    if (!list || !list->data || list->size == 0) { \
        return -1;                                 \
    }                                              \
    T* data = list->data;                          \
    size_t size = list->size;                      \
    for (size_t i = 0; i < size; i++) {            \
        if (value == data[i]) {                    \
            *index = i;                            \
            return 0;                              \
        }                                          \
    }                                              \
    return -1;                                     \
}                                                  \

////

#define LIB_LIST_CLASS(C, S, T)                    \
LIB_LIST_CLASS_STRUCT(S, T)                        \
LIB_LIST_CLASS_INIT(C, S, T)                       \
LIB_LIST_CLASS_FREE(C, S, T)                       \
LIB_LIST_CLASS_PRIVATE(C, S, T)                    \
LIB_LIST_CLASS_GET(C, S, T)                        \
LIB_LIST_CLASS_SET(C, S, T)                        \
LIB_LIST_CLASS_ADD(C, S, T)                        \
LIB_LIST_CLASS_INSERT(C, S, T)                     \
LIB_LIST_CLASS_REMOVE_INDEX(C, S, T)               \
LIB_LIST_CLASS_REMOVE(C, S, T)                     \

////

#define LIB_LIST_STATIC_CLASS(C, S, T)             \
LIB_LIST_CLASS_STRUCT(S, T)                        \
static LIB_LIST_CLASS_INIT(C, S, T)                \
static LIB_LIST_CLASS_FREE(C, S, T)                \
LIB_LIST_CLASS_PRIVATE(C, S, T)                    \
static LIB_LIST_CLASS_GET(C, S, T)                 \
static LIB_LIST_CLASS_SET(C, S, T)                 \
static LIB_LIST_CLASS_ADD(C, S, T)                 \
static LIB_LIST_CLASS_INSERT(C, S, T)              \
static LIB_LIST_CLASS_REMOVE_INDEX(C, S, T)        \
static LIB_LIST_CLASS_REMOVE(C, S, T)              \

////

#define LIB_LIST_TYPE(T) LIB_LIST_CLASS(lib_##T##_list, lib_##T##_list_t, T)
#define LIB_LIST_TYPE_PTR(T) LIB_LIST_CLASS(lib_##T##_ptr_list, lib_##T##_ptr_list_t, T*)

////

#define LIB_LIST_STATIC_TYPE(T) LIB_LIST_STATIC_CLASS(lib_##T##_list, lib_##T##_list_t, T)
#define LIB_LIST_STATIC_TYPE_PTR(T) LIB_LIST_STATIC_CLASS(lib_##T##_ptr_list, lib_##T##_ptr_list_t, T*)

////

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

int lib_list_insert(lib_list_t* list, size_t index, void* value);

int lib_list_add(lib_list_t* list, void* value);

int lib_list_remove(lib_list_t* list, void* value);

int lib_list_remove_index(lib_list_t* list, size_t index);

////

int lib_list_iterator_init(lib_list_t* list, lib_iterator_t* iterator);

bool lib_list_iterator_has_next(lib_list_t* list, lib_iterator_t* iterator);

void* lib_list_iterator_next(lib_list_t* list, lib_iterator_t* iterator);

#endif /* PLAZMA_LIB_LIST_H */

