#include <stdio.h>

#include "memlib.h"
#include "array.h"

static int _array_init(lib_array_t* array, size_t size, size_t element_size);

static void* _array_get(lib_array_t* array, size_t index);

static int _array_set(lib_array_t* array, size_t index, void* element);

int lib_array_init(lib_array_t* array, size_t size, size_t element_size) {
    return _array_init(array, size, element_size);
}

int lib_array_init_cast(lib_array_t* array, size_t size) {
    int retval = _array_init(array, size, sizeof(void*));
    if (retval != 0) {
        return retval;
    }
    array->mem_type = LIB_CLT_MEM_TYPE_CAST;
    return 0;
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

int lib_array_set(lib_array_t* array, size_t index, void* element) {
    if (!array || !array->data || index >= array->size ) {
        return -1;
    }
    return _array_set(array, index, element);
}

////

static int _array_init(lib_array_t* array, size_t size, size_t element_size) {
    if (!array || element_size == 0) {
        return -1;
    }
    array->mem_type = 0;
    array->size = size;
    array->element_size = element_size;
    array->data = lib_malloc(size * element_size);
    if (!array->data) {
        return -1;
    }
    //fprintf(stderr, "_array_init: %p %lu %lu\n", array, size, element_size);
    return 0;
}

////

static void* _array_get(lib_array_t* array, size_t index) {
    return lib_clt_get_mem(array->mem_type, array->data, index, array->element_size);
}

static int _array_set(lib_array_t* array, size_t index, void* value) {
    return lib_clt_set_mem(array->mem_type, array->data, index, array->element_size, value);
}

