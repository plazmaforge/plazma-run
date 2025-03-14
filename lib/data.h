#ifndef PLAZMA_LIB_DATA_H
#define PLAZMA_LIB_DATA_H

#define LIB_DATA_CAPACITY_MIN 10
#define LIB_DATA_CAPACITY_DEF 10

#define LIB_MIN(a, b) a < b ? a : b
#define LIB_MAX(a, b) a > b ? a : b

#define LIB_DATA_MEM_TYPE_PTR 1
#define LIB_DATA_MEM_TYPE_VAL 2
#define LIB_DATA_MEM_TYPE_DEF LIB_DATA_MEM_TYPE_PTR

#define LIB_DATA_MOVE_TYPE_PREV 1
#define LIB_DATA_MOVE_TYPE_NEXT 2

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

static bool lib_data_is_ptr(int type) {
    return (type == LIB_DATA_MEM_TYPE_PTR /* && value_size == sizeof(void*)*/);
}

static void* lib_data_get_mem(int type, void* data, size_t index, size_t value_size) {
    void* value = NULL;
    if (lib_data_is_ptr(type)) {
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
    if (lib_data_is_ptr(type)) {
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

////

static void lib_data_move(int type, void* data, size_t size, size_t value_size, size_t index) {
    void* curr = lib_data_offset(data, index, value_size);
    void* next = curr + value_size;
    size_t data_size = size - index;
    size_t byte_size = data_size * value_size;

    void* dst;
    void* src;

    if (type == LIB_DATA_MOVE_TYPE_PREV) {
        dst = curr;
        src = next;
    } else {
        dst = next;
        src = curr;
    }
    //fprintf(stderr, ">> list: memmove: data_size=%lu, byte_size=%lu\n", data_size, byte_size);
    memmove(dst, src, byte_size);
}

static void lib_data_move_prev(void* data, size_t size, size_t value_size, size_t index) {
    lib_data_move(LIB_DATA_MOVE_TYPE_PREV, data, size, value_size, index);
}

static void lib_data_move_next(void* data, size_t size, size_t value_size, size_t index) {
    lib_data_move(LIB_DATA_MOVE_TYPE_NEXT, data, size, value_size, index);
}

static void lib_data_reset(void* data, size_t index, size_t value_size) {
    void* offset = lib_data_offset(data, index, value_size);
    memset(offset, 0, value_size);
}

////

static int lib_data_find_mem(int type, void* data, size_t size, size_t value_size, void* value, size_t* index) {

    if (!value) {
        for (size_t i = 0; i < size; i++) {
            if (!lib_data_get_mem(type, data, i, value_size)) {
                *index = i;
                return 0;
            }
        }
    } else {
        bool is_ptr = lib_data_is_ptr(type);
        for (size_t i = 0; i < size; i++) {
            void* v = lib_data_get_mem(type, data, i, value_size);
            if (!v) {
                continue;
            }
            if (is_ptr) {
                if (v == value) {
                    *index = i;
                    return 0;
                }
            } else {
                if (memcmp(v, value, value_size) == 0) {
                    *index = i;
                    return 0;
                }
            }
        }
    }
    return -1;
}

////

static bool lib_data_need_realloc(size_t size, size_t capacity) {
    return (size >= capacity);
}

static size_t lib_data_new_capacity(size_t capacity) {
    return capacity * 2;
}

static void* lib_data_realloc(void* data, size_t size, size_t value_size) {
    return realloc(data, size * value_size);
}

////

static void lib_data_free_ptr(void* data, size_t size) {
    void** table = (void**) data;
    void* value = NULL;
    for (size_t i = 0; i < size; i++) {
        value = table[i];
        if (value) {
            //fprintf(stderr, ">> free: %s\n", value);
            free(value);
        }
    }
}

#endif // PLAZMA_LIB_DATA_H
