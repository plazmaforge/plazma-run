#ifndef PLAZMA_LIB_BASE32_H
#define PLAZMA_LIB_BASE32_H

char* lib_base32_encode(const char* src, size_t len, size_t* out_len);

char* lib_base32_decode(const char *src, size_t len, size_t* out_len);

#endif // PLAZMA_LIB_BASE32_H