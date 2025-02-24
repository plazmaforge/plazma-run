#ifndef PLAZMA_LIB_BASE58_H
#define PLAZMA_LIB_BASE58_H

char* lib_base58_encode(const char* src, size_t len, size_t* out_len);

char* lib_base58_decode(const char* src, size_t len, size_t* out_len);

#endif // PLAZMA_LIB_BASE58_H
