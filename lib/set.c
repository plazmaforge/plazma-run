#include "memlib.h"
#include "clt.h"
#include "set.h"

static int _set_init(lib_set_t* set, size_t capacity, size_t value_size);

int lib_set_init(lib_set_t* set, size_t capacity, size_t value_size) {
    return _set_init(set, capacity, value_size);
}

size_t lib_set_size(lib_set_t* set) {
    return !set ? 0 : set->size;
}

size_t lib_set_capacity(lib_set_t* set) {
    return !set ? 0 : set->capacity;
}

size_t lib_set_value_size(lib_set_t* set) {
    return !set ? 0 : set->value_size;
}

static int _set_init(lib_set_t* set, size_t capacity, size_t value_size) {
    if (!set || value_size == 0) {
        return -1;
    }
    set->size = 0;
    set->capacity = LIB_MAX(LIB_DATA_CAPACITY_MIN, capacity);
    set->value_size = value_size;
    set->data = lib_mallocz(capacity * value_size);
    if (!set->data) {
        return -1;
    }
    return 0;
}
