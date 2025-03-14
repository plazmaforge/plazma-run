#ifndef PLAZMA_LIB_DATA_H
#define PLAZMA_LIB_DATA_H

#define LIB_DATA_CAPACITY_MIN 10
#define LIB_DATA_CAPACITY_DEF 10

#define LIB_MIN(a, b) a < b ? a : b
#define LIB_MAX(a, b) a > b ? a : b

#define LIB_DATA_MEM_TYPE_PTR 1
#define LIB_DATA_MEM_TYPE_VAL 2
#define LIB_DATA_MEM_TYPE_DEF LIB_DATA_MEM_TYPE_PTR

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void lib_data_copy(void* offset, void* value, size_t value_size) {
    memcpy(offset, value, value_size);
}

static void* lib_data_offset(void* data, size_t index, size_t value_size) {
	return data + (index * value_size);
}

////

static void* lib_data_get(void* data, size_t index, size_t value_size) {
	return lib_data_offset(data, index, value_size);
}

static int lib_data_set(void* data, size_t index, size_t value_size, void* value) {
    void* offset = lib_data_offset(data, index, value_size);
    //if (!offset) {
    //    return -1;
    //}
    lib_data_copy(offset, value, value_size);
    return 0;
}

////

static bool is_ptr(int type) {
    return (type == LIB_DATA_MEM_TYPE_PTR /* && value_size == sizeof(void*)*/);
}

static void* lib_data_get_mem(int type, void* data, size_t index, size_t value_size) {
    void* value = NULL;
    if (is_ptr(type)) {
        //fprintf(stderr, ">> get-ptr: ");
        void** table = (void**) data;
        value = table[index];
    } else {
        //fprintf(stderr, ">> get-val: ");
        value = lib_data_get(data, index, value_size);
    }
    //fprintf(stderr, "%p OK\n", value);
    return value;
}

static int lib_data_set_mem(int type, void* data, size_t index, size_t value_size, void* value) {
    if (is_ptr(type)) {
        //fprintf(stderr, ">> set-ptr: %p", value);
        //offset = value;
        void** table = (void**) data;
        table[index] = value;
        //fprintf(stderr, " OK\n");
        return 0;
    } else {
        //fprintf(stderr, ">> set-val: %p OK\n", value);
        return lib_data_set(data, index, value_size, value);
    }    
}

#endif // PLAZMA_LIB_DATA_H
