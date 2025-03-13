#include <stdio.h>

#include "memlib.h"
#include "array.h"

static int _array_init(lib_array_t* array, int type, size_t size, size_t value_size);

static void* _array_get(lib_array_t* array, size_t index);

static int _array_set(lib_array_t* array, size_t index, void* value);

int lib_array_init(lib_array_t* array, size_t size, size_t value_size) {
    return _array_init(array, LIB_CLT_MEM_TYPE_DEF, size, value_size);
}

int lib_array_init_ptr(lib_array_t* array, size_t size) {
    return _array_init(array, LIB_CLT_MEM_TYPE_PTR, size, sizeof(void*));
}

int lib_array_init_val(lib_array_t* array, size_t size, size_t value_size) {
    return _array_init(array, LIB_CLT_MEM_TYPE_VAL, size, value_size);
}

void lib_array_free(lib_array_t* array) {
    if (!array || !array->data) {
        return;
    }
    free(array->data);
}

size_t lib_array_size(lib_array_t* array) {
    return !array ? 0 : array->size;
}

void* lib_array_get(lib_array_t* array, size_t index) {
    if (!array || index >= array->size ) {
        return NULL;
    }
    return _array_get(array, index);
}

int lib_array_set(lib_array_t* array, size_t index, void* value) {
    if (!array || !array->data || index >= array->size ) {
        return -1;
    }
    return _array_set(array, index, value);
}

////

static int _array_init(lib_array_t* array, int type, size_t size, size_t value_size) {
    if (!array || value_size == 0) {
        return -1;
    }
    array->mem_type = type;
    array->size = size;
    array->value_size = value_size;
    array->data = lib_mallocz(size * value_size);
    if (!array->data) {
        return -1;
    }
    //fprintf(stderr, "_array_init: %p %lu %lu\n", array, size, value_size);
    return 0;
}

////

static void* _array_get(lib_array_t* array, size_t index) {
    return lib_clt_get_mem(array->mem_type, array->data, index, array->value_size);
}

static int _array_set(lib_array_t* array, size_t index, void* value) {
    return lib_clt_set_mem(array->mem_type, array->data, index, array->value_size, value);
}

