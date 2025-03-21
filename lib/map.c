#include "memlib.h"
#include "data.h"
#include "map.h"

////

static int _map_init(lib_map_t* map, size_t capacity, size_t key_size, size_t value_size);

static void _map_free(lib_map_t* map);

////

static lib_entry_t* _map_entry_new();

static void _map_entry_free(lib_entry_t* entry);

static lib_entry_t** _map_entries_new(size_t size);

static lib_entry_t** _map_entries_realloc(lib_map_t* map, size_t size);

static void _map_entries_free(lib_map_t* map);

static lib_entry_t* _map_entry_find_key_index(lib_map_t* map, void* key, size_t* index);

static lib_entry_t* _map_entry_find_value_index(lib_map_t* map, void* value, size_t* index);

static lib_entry_t* _map_entry_find_key(lib_map_t* map, void* key);

static lib_entry_t* _map_entry_find_value(lib_map_t* map, void* value);

static lib_entry_t* _map_entry_find(lib_map_t* map, void* key);

static lib_entry_t* _map_entry_put(lib_map_t* map, void* key);

static void _map_entry_set(lib_map_t* map, lib_entry_t* entry, void* key, void* value);

static void* _map_entry_get(lib_map_t* map, lib_entry_t* entry);

static void* _map_get(lib_map_t* map, void* key);

static int _map_add(lib_map_t* map, void* key, void* value);

static int _map_remove_index(lib_map_t* map, size_t index);

static int _map_remove(lib_map_t* map, void* key);

////

int lib_map_init(lib_map_t* map, size_t capacity) {
    return _map_init(map, capacity, sizeof(void*), sizeof(void*));
}

void lib_map_free(lib_map_t* map) {
    _map_free(map);
}

////

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

////

int lib_map_add(lib_map_t* map, void* key, void* value) {
    if (!map) {
        return -1;
    }
    return _map_add(map, key, value);
}

void* lib_map_get(lib_map_t* map, void* key) {
    if (!map) {
        return NULL;
    }
    return _map_get(map, key);
}

int lib_map_remove(lib_map_t* map, void* key) {
    if (!map) {
        return -1;
    }
    return _map_remove(map, key);
}

bool lib_map_contains_key(lib_map_t* map, void* key) {
    if (!map) {
        return false;
    }
    lib_entry_t* entry = _map_entry_find_key(map, key);
    return entry != NULL;
}

bool lib_map_contains_value(lib_map_t* map, void* key) {
    if (!map) {
        return false;
    }
    lib_entry_t* entry = _map_entry_find_value(map, key);
    return entry != NULL;
}

bool lib_map_contains(lib_map_t* map, void* key) {
    return lib_map_contains_key(map, key);
}

////

static int _map_init(lib_map_t* map, size_t capacity, size_t key_size, size_t value_size) {
    if (!map) {
        return -1;
    }
    map->size = 0;
    map->capacity = LIB_MAX(LIB_DATA_CAPACITY_MIN, capacity);
    map->key_size = key_size;
    map->key_size = value_size;
    map->entries = _map_entries_new(capacity);
    if (!map->entries) {
        return -1;
    }
    return 0;
}

static void _map_free(lib_map_t* map) {
    if (!map) {
        return;
    }
    _map_entries_free(map);
    return;
}

static void _map_entry_reset(lib_entry_t* entry) {
    entry->key   = NULL;
    entry->value = NULL;
}

static lib_entry_t* _map_entry_new() {
    lib_entry_t* entry = (lib_entry_t*) malloc(sizeof(lib_entry_t));
    if (!entry) {
        return NULL;
    }
    _map_entry_reset(entry);
    return entry;
}

static void _map_entry_free(lib_entry_t* entry) {
    if (!entry) {
        return;
    }
    _map_entry_reset(entry);
    free(entry);
}

static lib_entry_t** _map_entries_new(size_t size) {
    return lib_mallocz(size * sizeof(lib_entry_t*));
}

static lib_entry_t** _map_entries_realloc(lib_map_t* map, size_t size) {
    return (lib_entry_t**) realloc(map->entries, sizeof(lib_entry_t*) * size);
}

static void _map_entries_free(lib_map_t* map) {
    if (!map->entries) {
        return;
    }

    lib_entry_t** entries = (lib_entry_t**) map->entries;
    lib_entry_t* entry    = NULL;

    for (size_t i = 0; i < map->size; i++) {
        entry = *entries;
        _map_entry_free(entry);
        entries++;
    }
 
}

////

static void* _map_realloc(lib_map_t* map) {
    size_t capacity = lib_data_new_capacity(map->capacity);
    void* entries = _map_entries_realloc(map, capacity);
    if (!entries) {
        return NULL;
    }

    map->entries = entries;
    return entries;
}

static bool _map_try_realloc(lib_map_t* map) {
    if (!lib_data_need_realloc(map->size, map->capacity)) {
        return true;
    }
    return _map_realloc(map);
}

//// HASH_TYPE

static size_t _map_hash(void* key) {
    return (size_t) key;
}

static size_t _map_hash2index(lib_map_t* map, size_t hash) {
    return hash % map->size;
}

static lib_entry_t* _map_entry_find_hash_index(lib_map_t* map, void* key, size_t* index) {
    
    if (map->size == 0) {
        return NULL;
    }

    size_t hash = _map_hash(key);
    *index = _map_hash2index(map, hash);
    lib_entry_t** entries = (lib_entry_t**) map->entries;
    lib_entry_t* entry    = entries[*index];

    return entry;
}

//// ARRAY_TYPE

static lib_entry_t* _map_entry_find_key_index(lib_map_t* map, void* key, size_t* index) {

    if (map->size == 0) {
        return NULL;
    }

    lib_entry_t** entries = (lib_entry_t**) map->entries;
    lib_entry_t* entry    = NULL;

    for (size_t i = 0; i < map->size; i++) {
        entry = *entries;
        if (entry->key == key) {
            // Found
            if (index) {
                *index = i;
            }
            return entry;
        }
        entries++;
    }

    return NULL;
}

static lib_entry_t* _map_entry_find_value_index(lib_map_t* map, void* value, size_t* index) {

    if (map->size == 0) {
        return NULL;
    }

    lib_entry_t** entries = (lib_entry_t**) map->entries;
    lib_entry_t* entry    = NULL;

    for (size_t i = 0; i < map->size; i++) {
        entry = *entries;
        if (entry->value == value) {
            // Found
            if (index) {
                *index = i;
            }
            return entry;
        }
        entries++;
    }

    return NULL;
}

////

static lib_entry_t* _map_entry_find_key(lib_map_t* map, void* key) {
    // ARRAY_TYPE
    return _map_entry_find_key_index(map, key, NULL);
}

static lib_entry_t* _map_entry_find_value(lib_map_t* map, void* value) {
    // ARRAY_TYPE
    return _map_entry_find_value_index(map, value, NULL);
}

static lib_entry_t* _map_entry_find(lib_map_t* map, void* key) {
    // ARRAY_TYPE
    return _map_entry_find_key_index(map, key, NULL);
}

////

static lib_entry_t* _map_entry_put(lib_map_t* map, void* key) {

    lib_entry_t* entry = _map_entry_find(map, key);
    if (entry) {
        // Found - Return
        return entry;
    }

    // ARRAY_TYPE
    if (!_map_try_realloc(map)) {
        //free(entry);
        return NULL;
    }

    // Not found - Create new
    entry = _map_entry_new();
    if (!entry) {
        return NULL;
    }

    // ARRAY_TYPE
    lib_entry_t** entries = (lib_entry_t**) map->entries;
    entries[map->size++] = entry;

    return entry;

}

static void _map_entry_set(lib_map_t* map, lib_entry_t* entry, void* key, void* value) {
    entry->key   = key;
    entry->value = value;
}

static void* _map_entry_get(lib_map_t* map, lib_entry_t* entry) {
    return entry->value;
}

////

static void* _map_get(lib_map_t* map, void* key) {
    lib_entry_t* entry = _map_entry_find(map, key);
    if (!entry) {
        return NULL;
    }
    return _map_entry_get(map, entry);
}

static int _map_add(lib_map_t* map, void* key, void* value) {
    lib_entry_t* entry = _map_entry_put(map, key);
    if (!entry) {
        return -1;
    }
    _map_entry_set(map, entry, key, value);
    return 0;
}

// ARRAY_TYPE
static int _map_remove_index(lib_map_t* map, size_t index) {
    size_t value_size = sizeof(lib_entry_t*);
    lib_data_move_prev(map->entries, map->size, value_size, index);
    lib_data_reset(map->entries, map->size - 1, value_size);
    map->size--;
    return 0;
}

static int _map_remove(lib_map_t* map, void* key) {
    // ARRAY_TYPE
    size_t index;
    lib_entry_t* entry = _map_entry_find_key_index(map, key, &index);
    if (!entry) {
        return -1;
    }
    _map_entry_free(entry);
    return _map_remove_index(map, index);
}
