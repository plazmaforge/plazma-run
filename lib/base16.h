#ifndef PLAZMA_LIB_BASE16_H
#define PLAZMA_LIB_BASE16_H

int lib_base16_encode(const char* src, size_t len, char** odata, size_t* out_len);

int lib_base16_decode(const char *src, size_t len, char** odata, size_t* out_len);

#endif // PLAZMA_LIB_BASE16_H
