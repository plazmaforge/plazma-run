#include "memlib.h"
#include "data.h"
#include "map.h"

size_t lib_map_size(lib_map_t* map) {
    return !map ? 0 : map->size;
}

size_t lib_map_capacity(lib_map_t* map) {
    return !map ? 0 : map->capacity;
}

size_t lib_map_key_size(lib_map_t* map) {
    return !map ? 0 : map->key_size;
}

size_t lib_map_value_size(lib_map_t* map) {
    return !map ? 0 : map->value_size;
}

static int _map_init(lib_map_t* map, size_t capacity, size_t key_size, size_t value_size) {
    if (!map) {
        return -1;
    }
    map->size = 0;
    map->capacity = LIB_MAX(LIB_DATA_CAPACITY_MIN, capacity);
    map->key_size = key_size;
    map->key_size = value_size;
    map->entities = lib_mallocz(capacity * sizeof(void*));
    if (!map->entities) {
        return -1;
    }
    return 0;
}

