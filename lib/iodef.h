#ifndef PLAZMA_LIB_IODEF_H
#define PLAZMA_LIB_IODEF_H

int lib_io_buf_init();

int lib_io_set_buf_size(FILE* file, int mode, size_t size);

int lib_io_stdin_set_buf_size(int mode, size_t size);

size_t lib_io_get_stdin_buf_size();

int lib_io_stdout_set_buf_size(int mode, size_t size);

size_t lib_io_stdout_get_buf_size();

bool lib_io_stdout_use_buf_size();

int lib_io_stderr_set_buf_size(int mode, size_t size);

size_t lib_io_stderr_get_buf_size();

bool lib_io_stderr_use_buf_size();

bool lib_io_std_use_buf_size();

size_t lib_io_get_buf_page(size_t size);

#endif // PLAZMA_LIB_IODEF_H