#ifndef PLAZMA_LIB_BASE64_H
#define PLAZMA_LIB_BASE64_H

#define LIB_BASE64 640

int lib_base64_encode(const char* idata, size_t isize, char** odata, size_t* osize);

int lib_base64_decode(const char* idata, size_t isize, char** odata, size_t* osize);

#endif // PLAZMA_LIB_BASE64_H
