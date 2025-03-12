#include "memlib.h"
#include "clt.h"
#include "list.h"

static int _list_init(lib_list_t* list, size_t capacity, size_t element_size);

int lib_list_init(lib_list_t* list, size_t capacity, size_t element_size) {
    return _list_init(list, capacity, element_size);
}

size_t lib_list_size(lib_list_t* list) {
    return !list ? 0 : list->size;
}

size_t lib_list_capacity(lib_list_t* list) {
    return !list ? 0 : list->capacity;
}

size_t lib_list_element_size(lib_list_t* list) {
    return !list ? 0 : list->element_size;
}

static int _list_init(lib_list_t* list, size_t capacity, size_t element_size) {
    if (!list || element_size == 0) {
        return -1;
    }
    list->size = 0;
    list->capacity = LIB_MAX(LIB_DATA_CAPACITY_MIN, capacity);
    list->element_size = element_size;
    list->data = lib_mallocz(capacity * element_size);
    if (!list->data) {
        return -1;
    }
    return 0;
}

// https://github.com/goldsborough/vector