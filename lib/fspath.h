#ifndef PLAZMA_LIB_FSPATH_H
#define PLAZMA_LIB_FSPATH_H

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

////

/* General Functions          */

#ifdef _WIN32
int lib_fs_is_drive_path(const char* path);
#endif

int lib_fs_is_absolute_path(const char* path);

/**
 * Return base file name
 * 
 * [allocate]
 */
char* lib_fs_get_base_name(const char* path);

/**
 * Return directory name
 * 
 * [allocate]
 */
char* lib_fs_get_dir_name (const char* path);

/**
 * Return base file name (alias lib_fs_get_base_name)
 * 
 * [allocate]
 */
char* lib_fs_get_file_name(const char* path);

/**
 * Return file extension
 * 
 * [allocate]
 */
char* lib_fs_get_file_ext(const char* path);

/**
 * Find file extension (without allocation)
 * 
 */
const char* lib_fs_find_file_ext(const char* path);

const char* lib_fs_skip_dir_separators(const char* path);

const char* lib_fs_skip_nondir_separators(const char* path);

const char* lib_fs_skip_root(const char* path);


#ifdef _WIN32

const wchar_t* lib_fs_wfind_file_ext(const wchar_t* wpath);

bool lib_fs_is_wexec_ext(const wchar_t* wpath);

bool lib_fs_is_exec_ext(const char* path);

#endif

#endif // PLAZMA_LIB_FSPATH_H