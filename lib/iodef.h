#ifndef PLAZMA_LIB_IODEF_H
#define PLAZMA_LIB_IODEF_H

#include <stdbool.h>

/**
 * Initialize I/O buffer
 */
int lib_io_buf_init();

/**
 * Set I/O buffer size
 */
int lib_io_set_buf_size(FILE* file, int mode, size_t size);

/**
 * Set stdin buffer size
 */
int lib_io_stdin_set_buf_size(int mode, size_t size);

/**
 * Return stdin buffer size
 */
size_t lib_io_get_stdin_buf_size();

/**
 * Set stdout buffer size
 */
int lib_io_stdout_set_buf_size(int mode, size_t size);

/**
 * Return stdout buffer size
 */
size_t lib_io_stdout_get_buf_size();

bool lib_io_stdout_use_buf_size();

/**
 * Set stderr buffer size
 */
int lib_io_stderr_set_buf_size(int mode, size_t size);

/**
 * Return stderr buffer size
 */
size_t lib_io_stderr_get_buf_size();

bool lib_io_stderr_use_buf_size();

bool lib_io_std_use_buf_size();

/**
 * Return I/O buffer page by buffer size
 */
size_t lib_io_get_buf_page(size_t size);

#endif // PLAZMA_LIB_IODEF_H