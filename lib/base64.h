#ifndef PLAZMA_LIB_BASE64_H
#define PLAZMA_LIB_BASE64_H

int lib_base64_encode(const char* idata, size_t isize, char** odata, size_t* osize);

int lib_base64_decode(const char* idata, size_t isize, char** odata, size_t* osize);

#endif // PLAZMA_LIB_BASE64_H
