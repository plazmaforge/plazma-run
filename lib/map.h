#ifndef PLAZMA_LIB_MAP_H
#define PLAZMA_LIB_MAP_H

#include <stdlib.h>

#define LIB_MAP_ENTRY_STRUCT(S, K, V)              \
typedef struct S {                                 \
    K key;                                         \
    V value;                                       \
} S;

#define LIB_MAP_STRUCT(S, E)                       \
typedef struct S {                                 \
    E** entries;                                   \
    size_t size;                                   \
    size_t capacity;                               \
} S;                                               \

#define LIB_MAP_CLASS_STRUCT(S, E, K, V)           \
LIB_MAP_ENTRY_STRUCT(E, K, V)                      \
LIB_MAP_STRUCT(S, E)                               \

#define LIB_TYPE_PRINT(T) fprintf(stdout, "TYPE-%s\n", #T)

#define LIB_MAP_CLASS_INIT(C, S, E)               \
int C##_init(S* map, size_t capacity) {            \
    if (!map) {                                    \
        return -1;                                 \
    }                                              \
    map->size = 0;                                 \
    map->capacity = LIB_MAX(LIB_DATA_CAPACITY_MIN, capacity); \
    map->entries = lib_mallocz(capacity * sizeof(E*));        \
    if (!map->entries) {                           \
        return -1;                                 \
    }                                              \
    LIB_TYPE_PRINT(C);                             \
    return 0;                                      \
}                                                  \

#define LIB_MAP_CLASS_FREE(C, S, E)                \
static void C##_free(S* map) {                     \
    if (!map || !map->entries) {                   \
        return;                                    \
    }                                              \
                                                   \
    E** entries = map->entries;                    \
    E* entry    = NULL;                            \
                                                   \
    for (size_t i = 0; i < map->size; i++) {       \
        entry = *entries;                          \
        free(entry);                               \
        entries++;                                 \
    }                                              \
    map->entries = NULL;                           \
}                                                  \


////

//    entry->key   = NULL;                          \
//    entry->value = NULL;                          \

#define LIB_MAP_CLASS_PRIVATE(C, S, E, K, V)      \
                                                  \
static E* C##_entry_new() {                       \
    E* entry = (E*) malloc(sizeof(E));            \
    if (!entry) {                                 \
        return NULL;                              \
    }                                             \
    return entry;                                 \
}                                                 \
                                                  \
static E** C##_entries_realloc(S* map, size_t size) {       \
    return (E**) realloc(map->entries, sizeof(E*) * size);  \
}                                                 \
                                                  \
static bool C##_realloc(S* map) {                 \
    size_t capacity = lib_data_new_capacity(map->capacity); \
    E** entries = C##_entries_realloc(map, capacity);       \
    if (!entries) {                               \
        return false;                             \
    }                                             \
    map->entries = entries;                       \
    return true;                                  \
}                                                 \
                                                  \
static bool C##_try_realloc(S* map) {             \
    if (!lib_data_need_realloc(map->size, map->capacity)) { \
        return true;                              \
    }                                             \
    return C##_realloc(map);                      \
}                                                 \
                                                  \
static E* C##_entry_find_index(S* map, K key, size_t* index) { \
    if (map->size == 0) {                         \
        return NULL;                              \
    }                                             \
    E** entries = map->entries;                   \
    E* entry    = NULL;                           \
    for (size_t i = 0; i < map->size; i++) {      \
        entry = *entries;                         \
        if (entry->key == key) {                  \
            if (index) {                          \
                *index = i;                       \
            }                                     \
            return entry;                         \
        }                                         \
        entries++;                                \
    }                                             \
    return NULL;                                  \
}                                                 \
                                                  \
static E* C##_entry_find(S* map, K key) {         \
    return C##_entry_find_index(map, key, NULL);  \
}                                                 \
                                                  \
static E* C##_entry_put(S* map, K key) {          \
                                                  \
    E* entry = C##_entry_find(map, key);          \
    if (entry) {                                  \
        return entry;                             \
    }                                             \
                                                  \
    entry = C##_entry_new();                      \
    if (!entry) {                                 \
        return NULL;                              \
    }                                             \
                                                  \
    if (!C##_try_realloc(map)) {                  \
        free(entry);                              \
        return NULL;                              \
    }                                             \
                                                  \
    E** entries = map->entries;                   \
    entries[map->size++] = entry;                 \
                                                  \
    return entry;                                 \
}                                                 \

////

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
