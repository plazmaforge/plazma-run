#ifndef PLAZMA_LIB_MAP_H
#define PLAZMA_LIB_MAP_H

#include <stdlib.h>

typedef struct lib_map_t {
    void* entities;
    size_t size;
    size_t capacity;
    size_t key_size;
    size_t value_size;
} lib_map_t;

size_t lib_map_size(lib_map_t* map);

size_t lib_map_capacity(lib_map_t* map);

size_t lib_map_key_size(lib_map_t* map);

size_t lib_map_value_size(lib_map_t* map);

#endif /* PLAZMA_LIB_MAP_H */
