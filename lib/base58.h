#ifndef PLAZMA_LIB_BASE58_H
#define PLAZMA_LIB_BASE58_H

char* lib_base58_encode(const char* src, size_t len, size_t* out_len);

char* lib_base58_decode(const char *src, size_t len, size_t* out_len);

//bool b58enc(char *b58, size_t *b58sz, const void *data, size_t binsz);

//bool b58tobin(void *bin, size_t *binszp, const char *b58, size_t b58sz);

#endif // PLAZMA_LIB_BASE58_H
