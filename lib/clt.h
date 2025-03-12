#ifndef PLAZMA_LIB_CLT_H
#define PLAZMA_LIB_CLT_H

#define LIB_DATA_CAPACITY_MIN 10
#define LIB_DATA_CAPACITY_DEF 10

#define LIB_MIN(a, b) a < b ? a : b
#define LIB_MAX(a, b) a > b ? a : b

#define LIB_CLT_MEM_TYPE_CAST 1
#define LIB_CLT_MEM_TYPE_COPY 2

#include <stdlib.h>
#include <string.h>

static void lib_clt_copy(void* offset, void* value, size_t value_size) {
    memcpy(offset, value, value_size);
}

static void* lib_clt_offset(void* data, size_t index, size_t value_size) {
	return data + (index * value_size);
}

////

static void* lib_clt_get(void* data, size_t index, size_t value_size) {
	return lib_clt_offset(data, index, value_size);
}

static int lib_clt_set(void* data, size_t index, size_t value_size, void* value) {
    void* offset = lib_clt_offset(data, index, value_size);
    if (!offset) {
        return -1;
    }
    lib_clt_copy(offset, value, value_size);
    return 0;
}

////

static void* lib_clt_get_mem(int type, void* data, size_t index, size_t value_size) {
    void* value = NULL;
    if (type == LIB_CLT_MEM_TYPE_CAST /* && value_size == sizeof(void*)*/ ) {
        void** table = (void**) data;
        value = table[index]; 
    } else {
        value = lib_clt_get(data, index, value_size);
    }
    //fprintf(stderr, "get >> %p\n",value);
    return value;
}

static int lib_clt_set_mem(int type, void* data, size_t index, size_t value_size, void* value) {
    if (type == LIB_CLT_MEM_TYPE_CAST /* && value_size == sizeof(void*)*/ ) {
        //offset = value;
        //fprintf(stderr, "set << %p\n", value);
        void** table = (void**) data;
        table[index] = value;
        //fprintf(stderr, "set >> %p\n", data[index]);
        return 0;
    } else {
        return lib_clt_set(data, index, value_size, value);
    }    
}


#endif // PLAZMA_LIB_CLT_H
