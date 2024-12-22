#ifndef PLAZMA_LIB_MEMLIB_H
#define PLAZMA_LIB_MEMLIB_H

void lib_memset0(void* data, size_t len);

void lib_free0(void* data, size_t len);

void lib_memreset(void* data, size_t len);

void lib_freereset(void* data, size_t len);

#endif // PLAZMA_LIB_MEMLIB_H