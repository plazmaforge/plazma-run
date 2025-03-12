#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * Variant Value 
 */
typedef union lib_value_t {
    int64_t i_value;
    double  f_value;
    void*   p_value;
} lib_value_t;

/**
 * Typed Variant Value 
 */
typedef struct lib_tvalue_t {
    int8_t type;
    lib_value_t value;
} lib_tvalue_t;

/**
 * Map Simple Entry 
 */
typedef struct lib_map_sentry_t {
    void* key;
    void* value;
} lib_map_sentry_t;

/**
 * Map Variant Entry
 */
typedef struct lib_map_ventry_t {
    lib_value_t key;
    lib_value_t value;
} lib_map_ventry_t;

typedef struct lib_map_t {
    void* entries;
    size_t size;
    size_t capacity;
} lib_map_t;

static int _map_init(lib_map_t* map);

static void _map_free(lib_map_t* map);

static void* _map_entry_new(lib_map_t* map);

static void _map_entry_free(lib_map_t* map, void* entry);

static void* _map_entries_new(lib_map_t* map, size_t size);

static void _map_entries_free(lib_map_t* map);

//// add

static int _map_add(lib_map_t* map, void* key, void* value);

//// add: V-Entry

static int _map_add_ixp(lib_map_t* map, int64_t key, void* value);

static int _map_add_ixi(lib_map_t* map, int64_t key, int64_t value);

static int _map_add_ixf(lib_map_t* map, int64_t key, double value);

////

static int _map_add_fxp(lib_map_t* map, double key, void* value);

static int _map_add_fxi(lib_map_t* map, double key, int64_t value);

static int _map_add_fxf(lib_map_t* map, double key, double value);

//// get

static void* _map_get(lib_map_t* map, void* key);

//// get: V-Entry

static void* _map_get_ixp(lib_map_t* map, int64_t key);

static int64_t _map_get_ixi(lib_map_t* map, int64_t key);

static double _map_get_ixf(lib_map_t* map, int64_t key);

////

static void* _map_get_fxp(lib_map_t* map, double key);

static int64_t _map_get_fxi(lib_map_t* map, double key);

static double _map_get_fxf(lib_map_t* map, double key);

////

static lib_map_ventry_t* _map_ventry_new();

static void _map_ventry_free(lib_map_ventry_t* entry);

static lib_map_ventry_t** _map_ventries_new(size_t size);

static lib_map_ventry_t** _map_ventries_realloc(lib_map_t* map, size_t size);

static void _map_ventries_free(lib_map_t* map);

static lib_map_ventry_t* _map_ventry_find(lib_map_t* map, lib_value_t key);

static lib_map_ventry_t* _map_ventry_put(lib_map_t* map, lib_value_t key);

//// x2v: V-Entry

static lib_value_t p2v(void* p);

static lib_value_t i2v(int64_t i);

static lib_value_t f2v(double f);

////

static void _f2v(double f, lib_value_t* v);

//// v2x: V-Entry

static void* v2p(lib_value_t v);

static int64_t v2i(lib_value_t v);

static double v2f(lib_value_t v);

//// new

lib_map_t* lib_map_new() {
    lib_map_t* map = (lib_map_t*) malloc(sizeof(lib_map_t));
    if (!map) {
        return NULL;
    }
    if (_map_init(map) != 0) {
        _map_free(map);
        return NULL;
    }
    return map;
}

void lib_map_free(lib_map_t* map) {
    //fprintf(stdout, "Start free...\t\t: %p\n", map);
    _map_free(map);
    //fprintf(stdout, "Map free\n");
}

//// add

int lib_map_add(lib_map_t* map, void* key, void* value) {
    if (!map) {
        return -1;
    }
    return _map_add(map, key, value);
}

//// add: V-Entry

int lib_map_add_ixp(lib_map_t* map, int key, void* value) {
    if (!map) {
        return -1;
    }
    return _map_add_ixp(map, key, value);
}

int lib_map_add_ixi(lib_map_t* map, int key, int value) {
    if (!map) {
        return -1;
    }
    return _map_add_ixi(map, key, value);
}

int lib_map_add_ixf(lib_map_t* map, int key, float value) {
    if (!map) {
        return -1;
    }
    return _map_add_ixf(map, key, value);
}

////

int lib_map_add_fxp(lib_map_t* map, float key, void* value) {
    if (!map) {
        return -1;
    }
    return _map_add_fxp(map, key, value);
}

int lib_map_add_fxi(lib_map_t* map, float key, int value) {
    if (!map) {
        return -1;
    }
    return _map_add_fxi(map, key, value);
}

int lib_map_add_fxf(lib_map_t* map, float key, float value) {
    if (!map) {
        return -1;
    }
    return _map_add_fxf(map, key, value);
}

//// get

void* lib_map_get(lib_map_t* map, void* key) {
    if (!map) {
        return NULL;
    }
    return _map_get(map, key);
}

//// get: V-Entry

void* lib_map_get_ixp(lib_map_t* map, int key) {
    if (!map) {
        return NULL;
    }
    return _map_get_ixp(map, key);
}

int lib_map_get_ixi(lib_map_t* map, int key) {
    if (!map) {
        return 0;
    }
    return (int) _map_get_ixi(map, key);
}

float lib_map_get_ixf(lib_map_t* map, int key) {
    if (!map) {
        return 0;
    }
    return (float) _map_get_ixf(map, key);
}

////

void* lib_map_get_fxp(lib_map_t* map, float key) {
    if (!map) {
        return NULL;
    }
    return _map_get_fxp(map, key);
}

int lib_map_get_fxi(lib_map_t* map, float key) {
    if (!map) {
        return 0;
    }
    return (int) _map_get_ixi(map, key);
}

float lib_map_get_fxf(lib_map_t* map, float key) {
    if (!map) {
        return 0;
    }
    return (float) _map_get_fxf(map, key);
}

/////////////////////////////////////////////////////////////////////////////////
//// C-Entry
/////////////////////////////////////////////////////////////////////////////////

static int _map_init(lib_map_t* map) {
    if (!map) {
        return -1;
    }
    map->size = 0;
    map->capacity = 0;
    map->entries = NULL;

    size_t init_size = 10; 

    void* entries = _map_entries_new(map, init_size);
    if (!entries) {
        return -1;
    }
    map->entries = entries;
    map->capacity = init_size;
    //fprintf(stdout, "map-init\t\t: %p %p\n", map, map->entries);
    return 0;
}

static void _map_free(lib_map_t* map) {
    if (!map) {
        return;
    }
    _map_entries_free(map);
    return;
}

static void* _map_entry_new(lib_map_t* map) {

    // V-entries
    return _map_ventry_new();
}

static void _map_entry_free(lib_map_t* map, void* entry) {

    // V-entries
    _map_ventry_free((lib_map_ventry_t*) entry);
}

static void* _map_entries_new(lib_map_t* map, size_t size) {

    // V-entries
    return _map_ventries_new(size);
}

static void _map_entries_free(lib_map_t* map) {

    // V-entries
    _map_ventries_free(map);
}

static bool _map_need_realloc(lib_map_t* map) {
    return (map->size >= map->capacity);
}

static size_t _map_new_capacity(size_t capacity) {
    return capacity * 2;
}

static void* _map_realloc(lib_map_t* map) {
    size_t capacity = _map_new_capacity(map->capacity);

    // V-entries
    void* entries = _map_ventries_realloc(map, capacity);
    if (!entries) {
        return NULL;
    }

    map->entries = entries;
    return entries;
}

static bool _map_try_realloc(lib_map_t* map) {
    if (!_map_need_realloc(map)) {
        return true;
    }
    return _map_realloc(map);
}

/////////////////////////////////////////////////////////////////////////////////
//// V-Entry
/////////////////////////////////////////////////////////////////////////////////

static lib_map_ventry_t* _map_ventry_new() {
    lib_map_ventry_t* entry = (lib_map_ventry_t*) malloc(sizeof(lib_map_ventry_t));
    if (!entry) {
        return NULL;
    }
    entry->key.i_value   = 0;
    entry->value.i_value = 0;
    return entry;
}

static void _map_ventry_free(lib_map_ventry_t* entry) {
    //fprintf(stderr, "Start free: %lld\n", entry->key.i_value);
    free(entry);
}

static lib_map_ventry_t** _map_ventries_new(size_t size) {
    return (lib_map_ventry_t**) malloc(sizeof(lib_map_ventry_t*) * size);
}

static lib_map_ventry_t** _map_ventries_realloc(lib_map_t* map, size_t size) {
    return (lib_map_ventry_t**) realloc(map->entries, sizeof(lib_map_ventry_t*) * size);
}

static void _map_ventries_free(lib_map_t* map) {
    if (!map->entries) {
        return;
    }

    lib_map_ventry_t** entries = (lib_map_ventry_t**) map->entries;
    lib_map_ventry_t* entry    = NULL;

    for (size_t i = 0; i < map->size; i++) {
        entry = *entries;
        _map_ventry_free(entry);
        entries++;
    }
 
}

static void _map_ventry_set(lib_map_ventry_t* entry, void* key, void* value) {
    entry->key.p_value = key;
    entry->value.p_value = value;
}

static void _map_ventry_set_ixp(lib_map_ventry_t* entry, int64_t key, void* value) {
    entry->key.i_value = key;
    entry->value.p_value = value;
}

static void _map_ventry_set_ixi(lib_map_ventry_t* entry, int64_t key, int64_t value) {
    entry->key.i_value = key;
    entry->value.i_value = value;
}

static void _map_ventry_set_ixf(lib_map_ventry_t* entry, int64_t key, double value) {
    entry->key.i_value = key;
    entry->value.f_value = value;
}

////

static void _map_ventry_set_fxp(lib_map_ventry_t* entry, double key, void* value) {
    entry->key.f_value = key;
    entry->value.p_value = value;
}

static void _map_ventry_set_fxi(lib_map_ventry_t* entry, double key, int64_t value) {
    entry->key.f_value = key;
    entry->value.i_value = value;
}

static void _map_ventry_set_fxf(lib_map_ventry_t* entry, double key, double value) {
    entry->key.f_value = key;
    entry->value.f_value = value;
}

// static int64_t p2i(void* value) {
//     //return !value ? 0 : (int64_t) value;
//     lib_value_t v;
//     v.p_value = value;
//     return v.i_value;
// }

// static int64_t f2i(float value) {
//     lib_value_t v;
//     v.f_value = value;
//     return v.i_value;
// }

//// 

static lib_value_t p2v(void* p) {
    lib_value_t v;
    v.p_value = p;
    return v;
}

static lib_value_t i2v(int64_t i) {
    lib_value_t v;
    v.i_value = i;
    return v;
}

static lib_value_t f2v(double f) {
    lib_value_t v;
    v.f_value = f;
    return v;
}

////

static void _f2v(double f, lib_value_t* v) {
    v->f_value = f;
}

////

static void* v2p(lib_value_t v) {
    return v.p_value;
}

static int64_t v2i(lib_value_t v) {
    return v.i_value;
}

static double v2f(lib_value_t v) {
    return v.f_value;
}

//// add

static int _map_add(lib_map_t* map, void* key, void* value) {
    lib_map_ventry_t* entry = _map_ventry_put(map, p2v(key));
    if (!entry) {
        return -1;
    }
    _map_ventry_set(entry, key, value);
    return 0;
}

//// add: V-Entry

static int _map_add_ixp(lib_map_t* map, int64_t key, void* value) {
    lib_map_ventry_t* entry = _map_ventry_put(map, i2v(key));
    if (!entry) {
        return -1;
    }
    _map_ventry_set_ixp(entry, key, value);
    return 0;
}

static int _map_add_ixi(lib_map_t* map, int64_t key, int64_t value) {
    lib_map_ventry_t* entry = _map_ventry_put(map, i2v(key));
    if (!entry) {
        return -1;
    }
    _map_ventry_set_ixi(entry, key, value);
    return 0;
}

static int _map_add_ixf(lib_map_t* map, int64_t key, double value) {
    lib_map_ventry_t* entry = _map_ventry_put(map, i2v(key));
    if (!entry) {
        return -1;
    }
    _map_ventry_set_ixf(entry, key, value);
    return 0;
}

////

static int _map_add_fxp(lib_map_t* map, double key, void* value) {
    lib_map_ventry_t* entry = _map_ventry_put(map, f2v(key));
    if (!entry) {
        return -1;
    }
    _map_ventry_set_fxp(entry, key, value);
    return 0;
}

static int _map_add_fxi(lib_map_t* map, double key, int64_t value) {
    lib_map_ventry_t* entry = _map_ventry_put(map, f2v(key));
    if (!entry) {
        return -1;
    }
    _map_ventry_set_fxi(entry, key, value);
    return 0;
}

static int _map_add_fxf(lib_map_t* map, double key, double value) {
    lib_map_ventry_t* entry = _map_ventry_put(map, f2v(key));
    if (!entry) {
        return -1;
    }
    _map_ventry_set_fxf(entry, key, value);
    return 0;
}

//// get

static void* _map_get(lib_map_t* map, void* key) {
    lib_map_ventry_t* entry = _map_ventry_find(map, p2v(key));
    if (!entry) {
        return NULL;
    }
    return v2p(entry->value);
}

//// get: V-Entry

static void* _map_get_ixp(lib_map_t* map, int64_t key) {
    lib_map_ventry_t* entry = _map_ventry_find(map, i2v(key));
    if (!entry) {
        return NULL;
    }
    return v2p(entry->value);
}

static int64_t _map_get_ixi(lib_map_t* map, int64_t key) {
    lib_map_ventry_t* entry = _map_ventry_find(map, i2v(key));
    if (!entry) {
        return 0;
    }
    return v2i(entry->value);
}

static double _map_get_ixf(lib_map_t* map, int64_t key) {
    lib_map_ventry_t* entry = _map_ventry_find(map, i2v(key));
    if (!entry) {
        return 0;
    }
    return v2f(entry->value);
}

////

static void* _map_get_fxp(lib_map_t* map, double key) {
    lib_map_ventry_t* entry = _map_ventry_find(map, f2v(key));
    if (!entry) {
        return NULL;
    }
    return v2p(entry->value);
}

static int64_t _map_get_fxi(lib_map_t* map, double key) {
    lib_map_ventry_t* entry = _map_ventry_find(map, f2v(key));
    if (!entry) {
        return 0;
    }
    return v2i(entry->value);
}

static double _map_get_fxf(lib_map_t* map, double key) {
    lib_map_ventry_t* entry = _map_ventry_find(map, f2v(key));
    if (!entry) {
        return 0;
    }
    return v2f(entry->value);
}

/**
 * Find V-Entry by key
 */
static lib_map_ventry_t* _map_ventry_find(lib_map_t* map, lib_value_t key) {

    if (map->size == 0) {
        return NULL;
    }

    lib_map_ventry_t** entries = (lib_map_ventry_t**) map->entries;
    lib_map_ventry_t* entry    = NULL;

    for (size_t i = 0; i < map->size; i++) {
        entry = *entries;
        if (entry->key.i_value == key.i_value) {
            // Found
            return entry;
        }
        entries++;
    }

    return NULL;
}

/*
static lib_map_ventry_t* _map_ventry_print(lib_map_t* map, lib_value_t key) {

    if (map->size == 0) {
        return NULL;
    }

    lib_map_ventry_t** entries = map->entries;
    lib_map_ventry_t* entry    = NULL;
    size_t i = 0;
    fprintf(stderr, "KEY: %lld\n", key.i_value);

    while ((entry = *entries) != NULL) {
        fprintf(stderr, "ENTRY-%lu: %lld\n", i, entry->key.i_value);
        if (entry->key.i_value == key.i_value) {
        //    // Found
        //    return entry;
        }
        i++;
        entries++;
    }
    return NULL;
}
*/


/**
 * Return V-Entry by key or create new if not found
 */
static lib_map_ventry_t* _map_ventry_put(lib_map_t* map, lib_value_t key) {

    lib_map_ventry_t* entry = _map_ventry_find(map, key);
    if (entry) {
        // Found - Return
        return entry;
    }

    // Not found - Create new
    entry = _map_ventry_new();
    if (!entry) {
        return NULL;
    }

    if (!_map_try_realloc(map)) {
        free(entry);
        return NULL;
    }

    lib_map_ventry_t** entries = (lib_map_ventry_t**) map->entries;
    entries[map->size++] = entry;

    return entry;

}

//// ALT

//// get

int8_t map_get_int8(lib_value_t* value) {
    return !value ? 0 : ((int8_t) value->i_value);
}

int16_t map_get_int16(lib_value_t* value) {
    return !value ? 0 : ((int16_t) value->i_value);
}

int32_t map_get_int32(lib_value_t* value) {
    return !value ? 0 : ((int32_t) value->i_value);
}

int64_t map_get_int64(lib_value_t* value) {
    return !value ? 0 : (value->i_value);
}

////

int map_get_cint(lib_value_t* value) {
    return !value ? 0 : ((int) value->i_value);
}

short map_get_cshort(lib_value_t* value) {
    return !value ? 0 : ((short) value->i_value);
}

long map_get_clong(lib_value_t* value) {
    return !value ? 0 : ((long) value->i_value);
}

////

char map_get_char(lib_value_t* value) {
    return !value ? 0 : ((char) value->i_value);
}

int map_get_int(lib_value_t* value) {
    return !value ? 0 : ((int) value->i_value);
}

short map_get_short(lib_value_t* value) {
    return !value ? 0 : ((short) value->i_value);
}

long map_get_long(lib_value_t* value) {
    return !value ? 0 : ((long) value->i_value);
}

float map_get_float(lib_value_t* value) {
    return !value ? 0 : ((float) value->f_value);
}

double map_get_double(lib_value_t* value) {
    return !value ? 0 : value->f_value;
}

void* map_get_ptr(lib_value_t* value) {
    return !value ? 0 : ((void*) value->p_value);
}

//// TEST

lib_map_t* test_map() {
    lib_map_t* test_map = lib_map_new();

    lib_map_add_ixp(test_map, 1000, "Hello-1000-888");
    lib_map_add_ixp(test_map, 7777, "Hello-7777-888");
    lib_map_add_ixp(test_map, 2000, "Hello-2000-888");
    lib_map_add_ixp(test_map, 9000, (void*) 12345888);

    //double d1 = 3.146789888;
    double* p1 = (double*) malloc(sizeof(double));
    *p1 = 3.746789888;

    //fprintf(stdout, "d1: %f\n", d1);
    //fprintf(stdout, "p1: %p\n", &d1);
    //fprintf(stdout, "p1: %p\n", (void*) &d1);

    //map_add_ixp(map, 888, (void*) &d1);

    ////

    fprintf(stdout, "d1: %f\n", *p1);
    fprintf(stdout, "p1: %p\n", p1);
    lib_map_add_ixp(test_map, 8888, p1);


    return test_map;
}

int main() {
    fprintf(stdout, "Test Map...\n");
    fprintf(stdout, "=========================================\n");

    fprintf(stdout, "sizeof(void*)\t\t: %lu\n", sizeof(void*));
    fprintf(stdout, "sizeof(int*)\t\t: %lu\n", sizeof(int*));
    fprintf(stdout, "sizeof(long*)\t\t: %lu\n", sizeof(long*));

    fprintf(stdout, "sizeof(value_t*)\t: %lu\n", sizeof(lib_value_t*));
    fprintf(stdout, "sizeof(map_sentry_t*)\t: %lu\n", sizeof(lib_map_sentry_t*));
    fprintf(stdout, "sizeof(map_ventry_t*)\t: %lu\n", sizeof(lib_map_ventry_t*));
    
    fprintf(stdout, "sizeof(value_t)\t: %lu\n", sizeof(lib_value_t));
    fprintf(stdout, "sizeof(map_sentry_t)\t: %lu\n", sizeof(lib_map_sentry_t));
    fprintf(stdout, "sizeof(map_ventry_t)\t: %lu\n", sizeof(lib_map_ventry_t));

    int    ivalue = 10;
    double dvalue = 3.14;

    lib_map_sentry_t entry_1;
    lib_map_sentry_t entry_2;
    
    entry_1.key = (void*) 10;
    entry_1.value = (void*) (long) 3.14;

    //entry_2.key = (void*) ivalue;
    entry_2.value = (void*) &dvalue;

    double* p = (double*) entry_2.value;
    double x = *p;

    fprintf(stdout, "\n");
    fprintf(stdout, "entry_1.key\t: %p\n", entry_1.key);
    fprintf(stdout, "entry_1.value\t: %p\n", entry_1.value);

    fprintf(stdout, "entry_1.key\t: %d\n", (int) entry_1.key);
    //fprintf(stdout, "entry_1.key\t: %d\n", *((int*) entry_1.key));

    fprintf(stdout, "entry_2.value\t: %f\n", *((double*) entry_2.value));
    fprintf(stdout, "entry_2.value\t: %f\n", x);

    lib_value_t mvalue;
    fprintf(stdout, "\n");
    fprintf(stdout, "mvalue.i_value\t: %lld\n", mvalue.i_value);    
    fprintf(stdout, "mvalue.f_value\t: %f\n", mvalue.f_value);

    mvalue.i_value  = 0;//65;
    //mvalue.f_value = 3.74;
    //mvalue.f_value = 0;

    fprintf(stdout, "\n");
    fprintf(stdout, "v->i_value\t: %lld\n", mvalue.i_value);    
    fprintf(stdout, "v->f_value\t: %f\n", mvalue.f_value);
    //fprintf(stdout, "v->c_value\t: %d\n", (int) mvalue.c_value);

    fprintf(stdout, "\n");
    fprintf(stdout, "v->get_int\t: %d\n", map_get_int(&mvalue));
    fprintf(stdout, "v->get_int64\t: %lld\n", map_get_int64(&mvalue));
    fprintf(stdout, "v->get_float\t: %f\n", map_get_float(&mvalue));
    fprintf(stdout, "v->get_char\t: %c\n", map_get_char(&mvalue));
    fprintf(stdout, "v->get_ptr\t: %4p\n", map_get_ptr(&mvalue));

    lib_map_t* map = lib_map_new();

    lib_map_add_ixp(map, 100, "Hello-100");
    lib_map_add_ixp(map, 777, "Hello-777");
    lib_map_add_ixp(map, 200, "Hello-200");
    lib_map_add_ixp(map, 900, (void*) 12345);
    lib_map_add_ixf(map, 888, 555.777);


    lib_map_add(map, "ID-100", "Hello-100-10-X");
    lib_map_add(map, "ID-200", "Hello-200-10-X");
    lib_map_add(map, "ID-300", "Hello-300-11-X");
    lib_map_add(map, "ID-400", (void*) 12345);
    lib_map_add(map, "ID-300", "Hello-300-12-X");
    lib_map_add_ixp(map, 314, "Hello-314-11-X");
    lib_map_add_fxp(map, 3.14, "Hello-314-3.14");
    
    //map_add_int32_ptr(map, 900, (void*) 12345);

    fprintf(stdout, "\n");
    fprintf(stdout, "v->get(700)\t: %s\n", lib_map_get_ixp(map, 777));
    fprintf(stdout, "v->get(100)\t: %s\n", lib_map_get_ixp(map, 100));
    fprintf(stdout, "v->get(200)\t: %s\n", lib_map_get_ixp(map, 200));
    fprintf(stdout, "v->get(300)\t: %s\n", lib_map_get_ixp(map, 300));
    fprintf(stdout, "v->get(900)\t: %d\n", (int) lib_map_get_ixp(map, 900));
    fprintf(stdout, "v->get(888)\t: %f\n", lib_map_get_ixf(map, 888));

    fprintf(stdout, "\n");
    fprintf(stdout, "v->get(\"ID-100\")\t: %s\n", lib_map_get(map, "ID-100"));
    fprintf(stdout, "v->get(\"ID-200\")\t: %s\n", lib_map_get(map, "ID-200"));
    fprintf(stdout, "v->get(\"ID-300\")\t: %s\n", lib_map_get(map, "ID-300"));
    fprintf(stdout, "v->get(314)\t\t: %s\n", lib_map_get_ixp(map, 314));
    fprintf(stdout, "v->get(3.14)\t\t: %s\n", lib_map_get_fxp(map, 3.14));
    
    double d1 = 3.146789;
    lib_map_add_ixp(map, 900, (void*) &d1);
    double*  p2 = (double*) lib_map_get_ixp(map, 900);
    double d2 = (double) *p2;
    fprintf(stdout, "v->get(900)\t\t: %f\n", d2);

    fprintf(stdout, "map-1\t\t: %p %p\n", map, map->entries);

    lib_map_t* map2 = test_map();
    fprintf(stdout, "map-1\t\t: %p %p\n", map, map->entries);
    fprintf(stdout, "map-2\t\t: %p %p\n", map2, map2->entries);

    fprintf(stdout, "\n");
    fprintf(stdout, "Test Map\n");

    fprintf(stdout, "v->get(7000)\t: %s\n", lib_map_get_ixp(map2, 7777));
    fprintf(stdout, "v->get(1000)\t: %s\n", lib_map_get_ixp(map2, 1000));
    fprintf(stdout, "v->get(2000)\t: %s\n", lib_map_get_ixp(map2, 2000));
    fprintf(stdout, "v->get(3000)\t: %s\n", lib_map_get_ixp(map2, 3000));
    //fprintf(stdout, "v->get(9000)\t: %d\n", (int) lib_map_get_ixp(map2, 9000));

    //double* p3 = (double*) lib_map_get_ixp(map2, 8888);
    //double  d3 = (double) *p3;

    //fprintf(stdout, "p3: %p\n", p3);
    //fprintf(stdout, "d3: %f\n", d3);
 
    //fprintf(stdout, "v->get(8888)\t: %f\n", d3);

    fprintf(stdout, "map-1\t\t: %p %p\n", map, map->entries);
    fprintf(stdout, "map-2\t\t: %p %p\n", map2, map2->entries);

    lib_map_free(map);
    lib_map_free(map2);

}