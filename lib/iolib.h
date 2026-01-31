#ifndef PLAZMA_LIB_IOLIB_H
#define PLAZMA_LIB_IOLIB_H

#include "fslib.h"
#include "iodef.h"

int lib_io_read_all_bytes(const char* file_name, char** data, size_t* size);

int lib_io_read_bytes(const char* file_name, char** data, size_t size, size_t* out_size);

int lib_io_write_all_bytes(const char* file_name, char* data, size_t size, size_t* out_size);

int lib_io_write_bytes(const char* file_name, char* data, size_t size, size_t* out_size);

int lib_io_read_cat_bytes(const char** file_names, size_t file_count, char** data, size_t* size);

FILE* lib_io_fopen(const char* file_name, const char* mode);

int lib_io_fclose(FILE* file);

#endif // PLAZMA_LIB_IOLIB_H