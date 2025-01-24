#ifndef PLAZMA_LIB_FSPATH_H
#define PLAZMA_LIB_FSPATH_H

#include <stdbool.h>

#include "fsdef.h"

/**
 * Build full file path
 * 
 * [allocate]
 */
char* lib_fs_get_file_path(const char* dir_name, const char* file_name);

/**
 * Return normalize full file path
 * 
 * [allocate]
 */
char* lib_fs_get_normalize_path(const char* dir_name, const char* file_name);

/**
 * Return normalize slash file path
 * 
 * [allocate]
 */
char* lib_fs_get_normalize_slash(char* path);

/**
 * Normalize slash file path
 */
void lib_fs_normalize_slash(char* path);

////

/**
 * Return real file path
 * 
 * [allocate]
 */
char* lib_fs_get_real_path(const char* path);

/**
 * Return current directory
 * 
 * [allocate]
 */
char* lib_fs_get_current_dir();

/**
 * Return current find path
 * - Unix    : '.'
 * - Windows : './ *'
 */
const char* lib_fs_get_current_find_path();


/**
 * Return true if a path is current find path
 * - Unix    : '.'
 * - Windows : './ *'
 */
bool lib_fs_is_current_find_path(const char* path);

#endif // PLAZMA_LIB_FSPATH_H