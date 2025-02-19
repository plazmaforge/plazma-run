#ifndef PLAZMA_LIB_BASE32_H
#define PLAZMA_LIB_BASE32_H

#define LIB_BASE32    0
#define LIB_BASE32U   1
#define LIB_BASE32L   2
#define LIB_ZBASE32   3
#define LIB_BASE32HEX 4

char* lib_base32_encode_type(int type, const char *src, size_t len, size_t* out_len);

char* lib_base32_encode(const char* src, size_t len, size_t* out_len);

char* lib_base32_decode_type(int type, const char *src, size_t len, size_t* out_len);

char* lib_base32_decode(const char *src, size_t len, size_t* out_len);

#endif // PLAZMA_LIB_BASE32_H