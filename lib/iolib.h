#ifndef PLAZMA_LIB_IOLIB_H
#define PLAZMA_LIB_IOLIB_H

#include "fslib.h"
#include "iodef.h"

int lib_io_read_all_bytes(const char* file_name, char** data);

int lib_io_read_bytes2(const char* file_name, char** data, size_t size);

////

char* lib_io_read_bytes(const char* file_name, size_t& size);

void lib_io_write_bytes(const char* file_name, char* data, size_t& size);

char* lib_io_read_cat_bytes(const char** file_names, int file_count, size_t& size);

#endif // PLAZMA_LIB_IOLIB_H