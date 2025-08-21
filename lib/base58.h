#ifndef PLAZMA_LIB_BASE58_H
#define PLAZMA_LIB_BASE58_H

#define LIB_BASE58 580

int lib_base58_encode(const char* data, size_t len, char** odata, size_t* olen);

int lib_base58_decode(const char* data, size_t len, char** odata, size_t* olen);

#endif // PLAZMA_LIB_BASE58_H
