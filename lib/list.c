#include "memlib.h"
#include "clt.h"
#include "list.h"

static int _list_init(lib_list_t* list, size_t capacity, size_t value_size);

int lib_list_init(lib_list_t* list, size_t capacity, size_t value_size) {
    return _list_init(list, capacity, value_size);
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

static int _list_init(lib_list_t* list, size_t capacity, size_t value_size) {
    if (!list || value_size == 0) {
        return -1;
    }
    list->size = 0;
    list->capacity = LIB_MAX(LIB_DATA_CAPACITY_MIN, capacity);
    list->value_size = value_size;
    list->data = lib_mallocz(capacity * value_size);
    if (!list->data) {
        return -1;
    }
    return 0;
}

// https://github.com/goldsborough/vector