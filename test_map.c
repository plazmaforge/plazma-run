#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

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
    lib_map_ventry_t** entries;
    size_t size;
    size_t allocated;
} lib_map_t;

static int _map_init(lib_map_t* map);

static void _map_free(lib_map_t* map);

//// add

static int _map_add(lib_map_t* map, void* key, void* value);

//// add: V-Entry

static int _map_add_ixp(lib_map_t* map, int64_t key, void* value);

static int _map_add_ixi(lib_map_t* map, int64_t key, int64_t value);

static int _map_add_ixf(lib_map_t* map, int64_t key, double value);

//// get

static void* _map_get(lib_map_t* map, void* key);

//// get: V-Entry

static void* _map_get_ixp(lib_map_t* map, int64_t key);

static int64_t _map_get_ixi(lib_map_t* map, int64_t key);

static double _map_get_ixf(lib_map_t* map, int64_t key);

static lib_map_ventry_t* _map_ventry_find(lib_map_t* map, lib_value_t key);

//// new

lib_map_t* map_new() {
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

void map_free(lib_map_t* map) {
    _map_free(map);
}

//// add

int map_add(lib_map_t* map, void* key, void* value) {
    if (!map) {
        return -1;
    }
    return _map_add(map, key, value);
}

//// add: V-Entry

int map_add_ixp(lib_map_t* map, int64_t key, void* value) {
    if (!map) {
        return -1;
    }
    return _map_add_ixp(map, key, value);
}

int map_add_ixf(lib_map_t* map, int64_t key, float value) {
    if (!map) {
        return -1;
    }
    return _map_add_ixf(map, key, value);
}

////

// int map_add_sxs(lib_map_t* map, char* key, char* value) {
//     if (!map) {
//         return -1;
//     }
//     return _map_add(map, key, value);
// }

//// get

void* map_get(lib_map_t* map, void* key) {
    if (!map) {
        return NULL;
    }
    return _map_get(map, key);
}

//// get: V-Entry

void* map_get_ixp(lib_map_t* map, int64_t key) {
    if (!map) {
        return NULL;
    }
    return _map_get_ixp(map, key);
}

float map_get_ixf(lib_map_t* map, int64_t key) {
    if (!map) {
        return 0;
    }
    return _map_get_ixf(map, key);
}

//// V-Entry

static lib_map_ventry_t* _map_ventry_new() {
    lib_map_ventry_t* entry = (lib_map_ventry_t*) malloc(sizeof(lib_map_ventry_t));
    if (!entry) {
        return NULL;
    }
    entry->key.i_value   = 0;
    entry->value.i_value = 0;
    return entry;
}

static lib_map_ventry_t** _map_ventries_new(size_t size) {
    return (lib_map_ventry_t**) malloc(sizeof(lib_map_ventry_t*) * size + 1); // +1 NULL
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

static void* v2p(lib_value_t v) {
    return v.p_value;
}

static int64_t v2i(lib_value_t v) {
    return v.i_value;
}

static double v2f(lib_value_t v) {
    return v.f_value;
}

////

static int _map_init(lib_map_t* map) {
    if (!map) {
        return -1;
    }
    map->size = 0;
    map->allocated = 0;
    map->entries = NULL;

    size_t capacity = 10; 
    lib_map_ventry_t** entries = _map_ventries_new(capacity);
    if (!entries) {
        return -1;
    }
    map->entries = entries;
    map->allocated = capacity;
    return 0;
}

static void _map_free(lib_map_t* map) {
    if (!map) {
        return;
    }
    if (map->entries) {
        //TODO
    }
    return;
}

static lib_map_ventry_t* _map_entry_new() {
    return _map_ventry_new();
}

/**
 * Return V-Entry by key or create new if not found
 */
static lib_map_ventry_t* _map_ventry_put(lib_map_t* map, lib_value_t key) {

    lib_map_ventry_t* entry = NULL;

    if (map->size > 0) {
        // Try find entry by key
        entry = _map_ventry_find(map, key);
    }

    if (entry) {
        // Found - Return
        return entry;
    }

    // Not found - Create new
    entry = _map_entry_new();
    if (!entry) {
        return NULL;
    }

    if (map->size >= map->allocated) {
        // TODO: Use realloc
        free(entry);
        return NULL;
    }

    map->entries[map->size] = entry;
    map->size++;
    return entry;

}

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

/**
 * Find V-Entry by key
 */
static lib_map_ventry_t* _map_ventry_find(lib_map_t* map, lib_value_t key) {

    if (map->size == 0) {
        return NULL;
    }

    lib_map_ventry_t** entries = map->entries;
    lib_map_ventry_t* entry    = NULL;

    while ((entry = *entries) != NULL) {
        if (entry->key.i_value == key.i_value) {
            // Found
            return entry;
        }
        entries++;
    }
    return NULL;
}

static void* _map_get(lib_map_t* map, void* key) {
    lib_map_ventry_t* entry = _map_ventry_find(map, p2v(key));
    if (!entry) {
        return NULL;
    }
    //return entry->value.p_value;
    return v2p(entry->value);
}

static void* _map_get_ixp(lib_map_t* map, int64_t key) {
    lib_map_ventry_t* entry = _map_ventry_find(map, i2v(key));
    if (!entry) {
        return NULL;
    }
    //return entry->value.p_value;
    return v2p(entry->value);
}

static int64_t _map_get_ixi(lib_map_t* map, int64_t key) {
    lib_map_ventry_t* entry = _map_ventry_find(map, i2v(key));
    if (!entry) {
        return 0;
    }
    //return entry->value.i_value;
    return v2i(entry->value);
}

static double _map_get_ixf(lib_map_t* map, int64_t key) {
    lib_map_ventry_t* entry = _map_ventry_find(map, i2v(key));
    if (!entry) {
        return 0;
    }
    //return entry->value.f_value;
    return v2f(entry->value);
}

static int _map_add(lib_map_t* map, void* key, void* value) {
    lib_map_ventry_t* entry = _map_ventry_put(map, p2v(key));
    if (!entry) {
        return -1;
    }
    _map_ventry_set(entry, key, value);
    return 0;
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
    lib_map_t* map = map_new();

    map_add_ixp(map, 100, "Hello-100-888");
    map_add_ixp(map, 777, "Hello-777-888");
    map_add_ixp(map, 200, "Hello-200-888");
    map_add_ixp(map, 900, (void*) 12345888);

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
    map_add_ixp(map, 888, p1);


    return map;
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

    entry_2.key = (void*) ivalue;
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

    lib_map_t* map = map_new();

    map_add_ixp(map, 100, "Hello-100");
    map_add_ixp(map, 777, "Hello-777");
    map_add_ixp(map, 200, "Hello-200");
    map_add_ixp(map, 900, (void*) 12345);
    map_add_ixf(map, 888, 555.777);


    map_add(map, "ID-100", "Hello-100-10-X");
    map_add(map, "ID-200", "Hello-200-10-X");
    map_add(map, "ID-300", "Hello-300-11-X");
    map_add(map, "ID-400", (void*) 12345);
    map_add(map, "ID-300", "Hello-300-12-X");
    
    //map_add_int32_ptr(map, 900, (void*) 12345);

    fprintf(stdout, "\n");
    fprintf(stdout, "v->get(700)\t: %s\n", map_get_ixp(map, 777));
    fprintf(stdout, "v->get(100)\t: %s\n", map_get_ixp(map, 100));
    fprintf(stdout, "v->get(200)\t: %s\n", map_get_ixp(map, 200));
    fprintf(stdout, "v->get(300)\t: %s\n", map_get_ixp(map, 300));
    fprintf(stdout, "v->get(900)\t: %d\n", (int) map_get_ixp(map, 900));
    fprintf(stdout, "v->get(888)\t: %f\n", map_get_ixf(map, 888));

    fprintf(stdout, "\n");
    fprintf(stdout, "v->get(\"ID-100\")\t: %s\n", map_get(map, "ID-100"));
    fprintf(stdout, "v->get(\"ID-200\")\t: %s\n", map_get(map, "ID-200"));
    fprintf(stdout, "v->get(\"ID-300\")\t: %s\n", map_get(map, "ID-300"));
    
    double d1 = 3.146789;
    map_add_ixp(map, 900, (void*) &d1);
    double*  p2 = (double*) map_get_ixp(map, 900);
    double d2 = (double) *p2;
    fprintf(stdout, "v->get(900)\t: %f\n", d2);

    lib_map_t* map2 = test_map();

    fprintf(stdout, "\n");
    fprintf(stdout, "Test Map\n");

    fprintf(stdout, "v->get(700)\t: %s\n", map_get_ixp(map2, 777));
    fprintf(stdout, "v->get(100)\t: %s\n", map_get_ixp(map2, 100));
    fprintf(stdout, "v->get(200)\t: %s\n", map_get_ixp(map2, 200));
    fprintf(stdout, "v->get(300)\t: %s\n", map_get_ixp(map2, 300));
    fprintf(stdout, "v->get(900)\t: %d\n", (int) map_get_ixp(map2, 900));

    double* p3 = (double*) map_get_ixp(map2, 888);
    double  d3 = (double) *p3;

    fprintf(stdout, "p3: %p\n", p3);
    fprintf(stdout, "d3: %f\n", d3);
 
    fprintf(stdout, "v->get(888)\t: %f\n", d3);


}