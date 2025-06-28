#include <string.h>
#include "memlib.h"

/**
 * Malloc 
 */
void* lib_malloc(size_t size) {
    return malloc(size);
}

/**
 * Malloc and reset memory
 */
void* lib_mallocz(size_t size) {
    void* data = lib_malloc(size);
    if (!data) {
        return NULL;
    }
    lib_memsetz(data, size);
    return data;
}

/**
 * Calloc
 */
void* lib_calloc(size_t count, size_t size) {
    return calloc(count, size);
}

/**
 * Calloc and reset memory
 */
void* lib_callocz(size_t count, size_t size) {
    void* data = calloc(count, size);
    if (!data) {
        return NULL;
    }
    lib_memsetz(data, size);
    return data;
}

/**
 * Free 
 */
void lib_free(void* data) {
    if (!data) {
        return;
    }
    free(data);
}

/**
 * Reset memory and free
 */
void lib_freez(void* data, size_t size) {
    if (!data) {
        return;
    }
    lib_memsetz(data, size);
    lib_free(data);
}

/**
 * Reset memory
 */
void lib_memsetz(void* data, size_t size) {
    if (!data || size == 0) {
        return;
    }
    memset(data, 0, size);
}

////

void lib_memreset(void* data, size_t len) {
    lib_memsetz(data, len);
}

void lib_freereset(void* data, size_t len) {
    lib_freez(data, len);
}

