#include <stdbool.h>

#include "memlib.h"
#include "data.h"
#include "list.h"

static int _list_init(lib_list_t* list, int type, size_t capacity, size_t value_size);

static void* _list_get(lib_list_t* list, size_t index);

static int _list_set(lib_list_t* list, size_t index, void* value);

int lib_list_init(lib_list_t* list, size_t capacity, size_t value_size) {
    return _list_init(list, LIB_CLT_MEM_TYPE_DEF, capacity, value_size);
}

int lib_list_init_ptr(lib_list_t* list, size_t size) {
    return _list_init(list, LIB_CLT_MEM_TYPE_PTR, size, sizeof(void*));
}

int lib_list_init_val(lib_list_t* list, size_t size, size_t value_size) {
    return _list_init(list, LIB_CLT_MEM_TYPE_VAL, size, value_size);
}

void lib_list_free(lib_list_t* list) {
    if (!list || !list->data) {
        return;
    }
    free(list->data);
    list->data = NULL;
}

void lib_list_free_values(lib_list_t* list) {
    if (!list || !list->data || list->mem_type != LIB_CLT_MEM_TYPE_PTR) {
        return;
    }
    void** table = (void**) list->data;
    void* value = NULL;
    size_t value_size = list->value_size;
    for (size_t i = 0; i < list->size; i++) {
        value = table[i];
        if (value) {
            //fprintf(stderr, ">> free: %s\n", value);
            free(value);
        }
    }    
}

void lib_list_free_all(lib_list_t* list) {
    if (!list || !list->data) {
        return;
    }
    lib_list_free_values(list);
    lib_list_free(list);
}

size_t lib_list_size(lib_list_t* list) {
    return !list ? 0 : list->size;
}

size_t lib_list_capacity(lib_list_t* list) {
    return !list ? 0 : list->capacity;
}

size_t lib_list_value_size(lib_list_t* list) {
    return !list ? 0 : list->value_size;
}

void* lib_list_get(lib_list_t* list, size_t index) {
    if (!list || index >= list->size ) {
        return NULL;
    }
    return _list_get(list, index);
}

int lib_list_set(lib_list_t* list, size_t index, void* value) {
    if (!list || !list->data || index >= list->size ) {
        return -1;
    }
    return _list_set(list, index, value);
}


////

static size_t _data_new_capacity(size_t capacity) {
    return capacity * 2;
}

static void* _data_realloc(void* data, size_t size, size_t value_size) {
    return realloc(data, size * value_size);
}

////

static bool _list_need_realloc(lib_list_t* list) {
    return (list->size >= list->capacity);
}

static void* _list_realloc(lib_list_t* list) {
    size_t capacity = _data_new_capacity(list->capacity);
    void* data = _data_realloc(list->data, capacity, list->value_size);
    if (!data) {
        return NULL;
    }
    list->data = data;
    list->capacity = capacity;
    //fprintf(stderr, "> list: realloc: size=%lu, capacity=%lu\n", list->size, list->capacity);
    return data;
}

static bool _list_try_realloc(lib_list_t* list) {
    if (!_list_need_realloc(list)) {
        return true;
    }
    return _list_realloc(list);
}

////

static int _list_find(lib_list_t* list, void* value, size_t* index) {
    if (!list || !list->data || list->size == 0) {
        return -1;
    }
    size_t size = list->size;

    if (!value) {
        for (size_t i = 0; i < size; i++) {
            if (!_list_get(list, i)) {
                *index = i;
                return 0;
            }
        }
    } else {
        bool is_ptr = list->mem_type == LIB_CLT_MEM_TYPE_PTR;
        size_t value_size = list->value_size;
        for (size_t i = 0; i < size; i++) {
            void* v = _list_get(list, i);
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

int lib_list_insert(lib_list_t* list, size_t index, void* value) {
    if (!list || !list->data || index >= list->size ) {
        return -1;
    }

    if (!_list_try_realloc(list)) {
        return -1;
    }

    //fprintf(stderr, ">> list: insert : index=%lu, size=%lu\n", index, list->size);

    void* data = list->data;
    void* offset = lib_clt_offset(list->data, index, list->value_size);
    void* curr = offset;
    void* next = curr + list->value_size;
    size_t data_size = list->size - index;
    size_t byte_size = data_size * list->value_size;

    //fprintf(stderr, ">> list: memmove: data_size=%lu, byte_size=%lu\n", data_size, byte_size);
    memmove(next, curr, byte_size);

    _list_set(list, index, value);
    list->size++;
    return 0;
}

int lib_list_remove_index(lib_list_t* list, size_t index) {
    if (!list || !list->data || index >= list->size ) {
        return -1;
    }

    //fprintf(stderr, ">> list: remove : index=%lu, size=%lu\n", index, list->size);

    void* data = list->data;
    void* offset = lib_clt_offset(list->data, index, list->value_size);
    void* curr = offset;
    void* next = curr + list->value_size;
    size_t data_size = list->size - index;
    size_t byte_size = data_size * list->value_size;

    //fprintf(stderr, ">> list: memmove: data_size=%lu, byte_size=%lu\n", data_size, byte_size);
    memmove(curr, next, byte_size);

    void* last = lib_clt_offset(list->data, list->size - 1, list->value_size);
    memset(last, 0, list->value_size);

    list->size--;
    return 0;
}

int lib_list_remove(lib_list_t* list, void* value) {
    if (!list || !list->data) {
        return -1;
    }
    size_t index;
    if (_list_find(list, value, &index) != 0) {
        return -1;
    }
    return lib_list_remove_index(list, index);
}

int lib_list_add(lib_list_t* list, void* value) {
    if (!list || !list->data) {
        return -1;
    }

    if (!_list_try_realloc(list)) {
        return -1;
    }

    _list_set(list, list->size, value);
    list->size++;    
    return 0;
}

////

static int _list_init(lib_list_t* list, int type, size_t capacity, size_t value_size) {
    if (!list || value_size == 0) {
        return -1;
    }
    list->mem_type = type;
    list->size = 0;
    list->capacity = LIB_MAX(LIB_DATA_CAPACITY_MIN, capacity);
    list->value_size = value_size;
    list->data = lib_mallocz(capacity * value_size);
    if (!list->data) {
        return -1;
    }
    return 0;
}

////

static void* _list_get(lib_list_t* list, size_t index) {
    return lib_clt_get_mem(list->mem_type, list->data, index, list->value_size);
}

static int _list_set(lib_list_t* list, size_t index, void* value) {
    return lib_clt_set_mem(list->mem_type, list->data, index, list->value_size, value);
}

// https://github.com/goldsborough/vector