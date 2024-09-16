#ifndef PLAZMA_LIB_IOLIB_H
#define PLAZMA_LIB_IOLIB_H

#include "fslib.h"

char* lib_io_read_bytes(const char* file_name, size_t& size);

void lib_io_write_bytes(const char* file_name, char* data, size_t& size);

char* lib_io_read_cat_bytes(const char** file_names, int file_count, size_t& size);

void lib_io_buf_init();

#endif // PLAZMA_LIB_IOLIB_H