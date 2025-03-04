#ifndef PLAZMA_LIB_BASE16_H
#define PLAZMA_LIB_BASE16_H

int lib_base16_encode(const char* idata, size_t isize, char** odata, size_t* osize);

int lib_base16_decode(const char* idata, size_t isize, char** odata, size_t* osize);

#endif // PLAZMA_LIB_BASE16_H
