#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef union map_value_t {
    int64_t i_value;
    double  f_value;
    void*   p_value;
} map_value_t;

typedef struct map_typed_value_t {
    int8_t type;
    map_value_t value;
} map_typed_value_t;

typedef struct map_entry_t {
    void* key;
    void* value;
} map_entry_t;

typedef struct map_union_entry_t {
    map_value_t key;
    map_value_t value;
} map_union_entry_t;

typedef struct map_t {
    map_union_entry_t** entries;
    size_t size;
    size_t allocated;
} map_t;

static int _map_init(map_t* map);

static void _map_free(map_t* map);

//// add

static int _map_add(map_t* map, void* key, void* value);

static int _map_add_ixp(map_t* map, int64_t key, void* value);

static int _map_add_ixi(map_t* map, int64_t key, int64_t value);

static int _map_add_ixf(map_t* map, int64_t key, double value);

//// get

static void* _map_get(map_t* map, void* key);

static void* _map_get_ixp(map_t* map, int64_t key);

static int64_t _map_get_ixi(map_t* map, int64_t key);

static double _map_get_ixf(map_t* map, int64_t key);

//// new

map_t* map_new() {
    map_t* map = (map_t*) malloc(sizeof(map_t));
    if (!map) {
        return NULL;
    }
    if (_map_init(map) != 0) {
        _map_free(map);
        return NULL;
    }
    return map;
}

void map_free(map_t* map) {
    _map_free(map);
}

//// add

int map_add(map_t* map, void* key, void* value) {
    if (!map) {
        return -1;
    }
    return _map_add(map, key, value);
}

////

int map_add_ixp(map_t* map, int64_t key, void* value) {
    if (!map) {
        return -1;
    }
    return _map_add_ixp(map, key, value);
}

int map_add_ixf(map_t* map, int64_t key, float value) {
    if (!map) {
        return -1;
    }
    return _map_add_ixf(map, key, value);
}

////

int map_add_sxs(map_t* map, char* key, char* value) {
    if (!map) {
        return -1;
    }
    return _map_add(map, key, value);
}

//// get

void* map_get(map_t* map, void* key) {
    if (!map) {
        return NULL;
    }
    return _map_get(map, key);
}

void* map_get_ixp(map_t* map, int64_t key) {
    if (!map) {
        return NULL;
    }
    return _map_get_ixp(map, key);
}

float map_get_ixf(map_t* map, int64_t key) {
    if (!map) {
        return 0;
    }
    return _map_get_ixf(map, key);
}

////

static int _map_init(map_t* map) {
    if (!map) {
        return -1;
    }
    map->size = 0;
    map->allocated = 0;
    map->entries = NULL;

    size_t capacity = 10; 
    map_union_entry_t** entries = (map_union_entry_t**) malloc(sizeof(map_union_entry_t*) * capacity + 1); // +1 NULL
    if (!entries) {
        return -1;
    }
    map->entries = entries;
    map->allocated = capacity;
    return 0;
}

static void _map_free(map_t* map) {
    if (!map) {
        return;
    }
    if (map->entries) {
        //TODO
    }
    return;
}

static map_union_entry_t* _map_entry_new() {
    map_union_entry_t* entry = (map_union_entry_t*) malloc(sizeof(map_union_entry_t));
    if (!entry) {
        return NULL;
    }
    entry->key.i_value   = 0;
    entry->value.i_value = 0;
    return entry;
}

static void _map_entry_set_ixp(map_union_entry_t* entry, int64_t key, void* value) {
    entry->key.i_value = key;
    entry->value.p_value = value;
}

static void _map_entry_set_ixi(map_union_entry_t* entry, int64_t key, int64_t value) {
    entry->key.i_value = key;
    entry->value.i_value = value;
}

static void _map_entry_set_ixf(map_union_entry_t* entry, int64_t key, double value) {
    entry->key.i_value = key;
    entry->value.f_value = value;
}

/**
 * Add entry by universal key  
 */
static map_union_entry_t* _map_entry_add(map_t* map, int64_t key) {

    map_union_entry_t** entries = NULL;
    map_union_entry_t* entry    = NULL;

    if (map->size == 0) {
        entry = _map_entry_new();
        if (!entry) {
            return NULL;
        }
        map->entries[0] = entry;
        //_map_entry_set_ixp(entry, key, value);
        map->size++;
        return entry;
    }

    if (map->size >= map->allocated) {
        // TODO: Use realloc
        return NULL;
    }

    entries = map->entries;
    entry   = NULL;

    while ((entry = *entries) != NULL) {
         if (entry->key.i_value == key) {
            // Found and replaced
            //entry->value.p_value = value;
            return entry;
         }
        entries++;
    }

    entry = _map_entry_new();
    if (!entry) {
        return NULL;
    }

    map->entries[map->size] = entry;
    //_map_entry_set_ixp(entry, key, value);
    map->size++;
    return entry;

}

static int _map_add_ixp(map_t* map, int64_t key, void* value) {
    map_union_entry_t* entry = _map_entry_add(map, key);
    if (!entry) {
        return -1;
    }
    _map_entry_set_ixp(entry, key, value);
    return 0;
}

static int _map_add_ixi(map_t* map, int64_t key, int64_t value) {
    if (!map) {
        return -1;
    }
    map_union_entry_t* entry = _map_entry_add(map, key);
    if (!entry) {
        return -1;
    }
    _map_entry_set_ixi(entry, key, value);
    return 0;
}

static int _map_add_ixf(map_t* map, int64_t key, double value) {
    if (!map) {
        return -1;
    }
    map_union_entry_t* entry = _map_entry_add(map, key);
    if (!entry) {
        return -1;
    }
    _map_entry_set_ixf(entry, key, value);
    return 0;
}

/**
 * Add entry by universal key  
 */
static map_union_entry_t* _map_entry_get(map_t* map, map_value_t key) {
    //if (!map) {
    //    return NULL;
    //}

    map_union_entry_t** entries = NULL;
    map_union_entry_t* entry    = NULL;

    if (map->size == 0) {
        return NULL;
    }

    entries = map->entries;
    entry   = NULL;

    while ((entry = *entries) != NULL) {
         //if (entry->key.i_value == key) {
         if (entry->key.i_value == key.i_value) {
            // Found
            //return entry->value.p_value;
            return entry;
         }
        entries++;
    }
    return NULL;
}

static void* _map_get(map_t* map, void* key) {
    map_value_t vkey;
    vkey.p_value = key;

    map_union_entry_t* entry = _map_entry_get(map, vkey);
    if (!entry) {
        return NULL;
    }
    return entry->value.p_value;
}

static void* _map_get_ixp(map_t* map, int64_t key) {
    map_value_t vkey;
    vkey.i_value = key;

    map_union_entry_t* entry = _map_entry_get(map, vkey);
    if (!entry) {
        return NULL;
    }
    return entry->value.p_value;
}

static int64_t _map_get_ixi(map_t* map, int64_t key) {
    map_value_t vkey;
    vkey.i_value = key;

    map_union_entry_t* entry = _map_entry_get(map, vkey);
    if (!entry) {
        return 0;
    }
    return entry->value.i_value;
}

static double _map_get_ixf(map_t* map, int64_t key) {
    map_value_t vkey;
    vkey.i_value = key;

    map_union_entry_t* entry = _map_entry_get(map, vkey);
    if (!entry) {
        return 0;
    }
    return entry->value.f_value;
}

static int64_t p2i(void* value) {
    //return !value ? 0 : (int64_t) value;
    map_value_t v;
    v.p_value = value;
    return v.i_value;
}

static int64_t f2i(float value) {
    map_value_t v;
    v.f_value = value;
    return v.i_value;
}

static int _map_add(map_t* map, void* key, void* value) {
    int64_t ikey = p2i(key);
    return _map_add_ixp(map, ikey, value);
}

//// get

int8_t map_get_int8(map_value_t* value) {
    return !value ? 0 : ((int8_t) value->i_value);
}

int16_t map_get_int16(map_value_t* value) {
    return !value ? 0 : ((int16_t) value->i_value);
}

int32_t map_get_int32(map_value_t* value) {
    return !value ? 0 : ((int32_t) value->i_value);
}

int64_t map_get_int64(map_value_t* value) {
    return !value ? 0 : (value->i_value);
}

////

int map_get_cint(map_value_t* value) {
    return !value ? 0 : ((int) value->i_value);
}

short map_get_cshort(map_value_t* value) {
    return !value ? 0 : ((short) value->i_value);
}

long map_get_clong(map_value_t* value) {
    return !value ? 0 : ((long) value->i_value);
}

////

char map_get_char(map_value_t* value) {
    return !value ? 0 : ((char) value->i_value);
}

int map_get_int(map_value_t* value) {
    return !value ? 0 : ((int) value->i_value);
}

short map_get_short(map_value_t* value) {
    return !value ? 0 : ((short) value->i_value);
}

long map_get_long(map_value_t* value) {
    return !value ? 0 : ((long) value->i_value);
}

float map_get_float(map_value_t* value) {
    return !value ? 0 : ((float) value->f_value);
}

double map_get_double(map_value_t* value) {
    return !value ? 0 : value->f_value;
}

void* map_get_ptr(map_value_t* value) {
    return !value ? 0 : ((void*) value->p_value);
}

map_t* test_map() {
    map_t* map = map_new();

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
    fprintf(stdout, "Test Union...\n");
    fprintf(stdout, "=========================================\n");

    fprintf(stdout, "sizeof(void*)\t\t: %lu\n", sizeof(void*));
    fprintf(stdout, "sizeof(int*)\t\t: %lu\n", sizeof(int*));
    fprintf(stdout, "sizeof(long*)\t\t: %lu\n", sizeof(long*));

    fprintf(stdout, "sizeof(map_entry_t*)\t: %lu\n", sizeof(map_entry_t*));
    fprintf(stdout, "sizeof(map_entry_t)\t: %lu\n", sizeof(map_entry_t));
    fprintf(stdout, "sizeof(map_value_t)\t: %lu\n", sizeof(map_value_t));
    fprintf(stdout, "sizeof(map_value_t)\t: %lu\n", sizeof(map_value_t));
    fprintf(stdout, "sizeof(map_union_entry_t)\t: %lu\n", sizeof(map_union_entry_t));

    int    ivalue = 10;
    double dvalue = 3.14;

    map_entry_t entry_1;
    map_entry_t entry_2;
    
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

    map_value_t mvalue;
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

    map_t* map = map_new();

    map_add_ixp(map, 100, "Hello-100");
    map_add_ixp(map, 777, "Hello-777");
    map_add_ixp(map, 200, "Hello-200");
    map_add_ixp(map, 900, (void*) 12345);
    map_add_ixf(map, 888, 888.777);


    map_add(map, "ID-100", "Hello-100-0");
    map_add(map, "ID-200", "Hello-200-0");
    map_add(map, "ID-300", "Hello-300-1");
    map_add(map, "ID-400", (void*) 12345);
    map_add(map, "ID-300", "Hello-300-2");
    
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

    map_t* map2 = test_map();

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