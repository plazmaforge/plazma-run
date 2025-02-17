#ifndef PLAZMA_LIB_BASE64_H
#define PLAZMA_LIB_BASE64_H

char* lib_base64_encode(const char* src, size_t len, size_t* out_len);

char* lib_base64_decode(const char *src, size_t len, size_t* out_len);

#endif // PLAZMA_LIB_BASE64_H
