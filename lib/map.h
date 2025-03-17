#ifndef PLAZMA_LIB_MAP_H
#define PLAZMA_LIB_MAP_H

#include <stdlib.h>

typedef struct lib_map_t {
    void* entries;
    size_t size;
    size_t capacity;
    size_t key_size;
    size_t value_size;
    int mem_type;
} lib_map_t;

int lib_map_init(lib_map_t* map, size_t capacity);

void lib_map_free(lib_map_t* map);

////

size_t lib_map_size(lib_map_t* map);

size_t lib_map_capacity(lib_map_t* map);

size_t lib_map_key_size(lib_map_t* map);

size_t lib_map_value_size(lib_map_t* map);

////

int lib_map_add(lib_map_t* map, void* key, void* value);

void* lib_map_get(lib_map_t* map, void* key);

int lib_map_remove(lib_map_t* map, void* key);

bool lib_map_contains(lib_map_t* map, void* key);

#endif /* PLAZMA_LIB_MAP_H */
