#ifndef PLAZMA_LIB_BASE16_H
#define PLAZMA_LIB_BASE16_H

char* lib_base16_encode(const char* src, size_t len, size_t* out_len);

char* lib_base16_decode(const char *src, size_t len, size_t* out_len);

#endif // PLAZMA_LIB_BASE16_H
