#ifndef PLAZMA_LIB_MEMLIB_H
#define PLAZMA_LIB_MEMLIB_H

#include <stdlib.h>

/**
 * Malloc 
 */
void* lib_malloc(size_t size);

/**
 * Malloc and reset memory
 */
void* lib_mallocz(size_t size);

/**
 * Calloc
 */
void* lib_calloc(size_t count, size_t size);

/**
 * Calloc and reset memory
 */
void* lib_callocz(size_t count, size_t size);

/**
 * Free 
 */
void lib_free(void* data);

/**
 * Reset memory and free
 */
void lib_freez(void* data, size_t size);

/**
 * Reset memory
 */
void lib_memsetz(void* data, size_t size);

#endif // PLAZMA_LIB_MEMLIB_H