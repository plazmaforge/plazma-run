#ifndef PLAZMA_LIB_BASE64_H
#define PLAZMA_LIB_BASE64_H

int lib_base64_encode(const char* data, size_t len, char** odata, size_t* olen);

int lib_base64_decode(const char* data, size_t len, char** odata, size_t* olen);

#endif // PLAZMA_LIB_BASE64_H
