#ifndef PLAZMA_LIB_IOLIB_H
#define PLAZMA_LIB_IOLIB_H

char* lib_io_read_bytes(const char* file_name);

char* lib_io_read_bytes_size(const char* file_name, size_t& size);

void lib_io_write_bytes_size(const char* file_name, char* data, size_t& size);

#endif // PLAZMA_LIB_IOLIB_H