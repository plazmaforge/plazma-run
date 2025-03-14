#include <stdbool.h>

#include "memlib.h"
#include "data.h"
#include "list.h"

static int _list_init(lib_list_t* list, int type, size_t capacity, size_t value_size);

static void* _list_get(lib_list_t* list, size_t index);

static int _list_set(lib_list_t* list, size_t index, void* value);

int lib_list_init(lib_list_t* list, size_t capacity, size_t value_size) {
    return _list_init(list, LIB_DATA_MEM_TYPE_DEF, capacity, value_size);
}

int lib_list_init_ptr(lib_list_t* list, size_t size) {
    return _list_init(list, LIB_DATA_MEM_TYPE_PTR, size, sizeof(void*));
}

int lib_list_init_val(lib_list_t* list, size_t size, size_t value_size) {
    return _list_init(list, LIB_DATA_MEM_TYPE_VAL, size, value_size);
}

void lib_list_free_ptr(lib_list_t* list) {
    if (!list || !list->data || list->mem_type != LIB_DATA_MEM_TYPE_PTR) {
        return;
    }
    lib_data_free_ptr(list->data, list->size);
}

void lib_list_free(lib_list_t* list) {
    if (!list || !list->data) {
        return;
    }
    free(list->data);
    list->data = NULL;
}

void lib_list_free_all(lib_list_t* list) {
    if (!list || !list->data) {
        return;
    }
    lib_list_free_ptr(list);
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

static void* _list_realloc(lib_list_t* list) {
    size_t capacity = lib_data_new_capacity(list->capacity);
    void* data = lib_data_realloc(list->data, capacity, list->value_size);
    if (!data) {
        return NULL;
    }
    list->data = data;
    list->capacity = capacity;
    //fprintf(stderr, "> list: realloc: size=%lu, capacity=%lu\n", list->size, list->capacity);
    return data;
}

static bool _list_try_realloc(lib_list_t* list) {
    if (!lib_data_need_realloc(list->size, list->capacity)) {
        return true;
    }
    return _list_realloc(list);
}

////

static int _list_find(lib_list_t* list, void* value, size_t* index) {
    if (!list || !list->data || list->size == 0) {
        return -1;
    }
    return lib_data_find_mem(list->mem_type, list->data, list->size, list->value_size, value, index);
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
    lib_data_move_next(list->data, list->size, list->value_size, index);
    _list_set(list, index, value);

    list->size++;
    return 0;
}

int lib_list_remove_index(lib_list_t* list, size_t index) {
    if (!list || !list->data || index >= list->size ) {
        return -1;
    }

    //fprintf(stderr, ">> list: remove : index=%lu, size=%lu\n", index, list->size);
    lib_data_move_prev(list->data, list->size, list->value_size, index);
    lib_data_reset(list->data, list->size - 1, list->value_size);

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

int lib_list_iterator_init(lib_list_t* list, lib_iterator_t* iterator) {
    if (!list || !iterator) {
        return -1;
    }
    iterator->data = list->data;
    iterator->size = list->size;
    iterator->value_size = list->value_size;
    iterator->mem_type = list->mem_type;
    iterator->index = 0;
    return 0;
}

bool lib_list_iterator_has_next(lib_list_t* list, lib_iterator_t* iterator) {
    if (!list || !iterator) {
        return false;
    }
    return lib_data_iterator_has_next(iterator);
}

void* lib_list_iterator_next(lib_list_t* list, lib_iterator_t* iterator) {
    if (!lib_list_iterator_has_next(list, iterator)) {
        return NULL;
    }
    return lib_data_array_iterator_next(iterator);
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
    return lib_data_get_mem(list->mem_type, list->data, index, list->value_size);
}

static int _list_set(lib_list_t* list, size_t index, void* value) {
    return lib_data_set_mem(list->mem_type, list->data, index, value, list->value_size);
}

// https://github.com/goldsborough/vector