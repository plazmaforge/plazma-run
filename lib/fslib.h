#ifndef PLAZMA_LIB_FSLIB_H
#define PLAZMA_LIB_FSLIB_H

#include <sys/stat.h>

#include "fspath.h"
#include "fsdirent.h"
#include "fsfile.h"

typedef enum {
  LIB_FILE_CHECK_REG   = 1 << 0,
  LIB_FILE_CHECK_LNK   = 1 << 1,
  LIB_FILE_CHECK_DIR   = 1 << 2,
  LIB_FILE_CHECK_EXEC  = 1 << 3,
  LIB_FILE_CHECK_EXISTS   = 1 << 4
} lib_file_check_t;

#define LIB_SCANDIR_FLAT       -1 // Scandir flat mode (only one level)
#define LIB_SCANDIR_RECURSIVE   0 // Scandir recursive mode

typedef enum {
  LIB_FILE_SORT_NONE           = 0,
  LIB_FILE_SORT_BY_NAME        = 1,
  LIB_FILE_SORT_BY_SIZE        = 2,
  LIB_FILE_SORT_BY_TIME        = 3,
} lib_file_sort_t;

#define LIB_FS_MODE_LEN    11
#define LIB_FS_ACCESS_LEN   9

/**
 * Return true if a file name is matched by a pattern
 */
int lib_fs_match_file(const char* name, const char* pattern);

//// fs-check

bool lib_fs_exists(const char* path);

bool lib_fs_is_reg(const char* path);

bool lib_fs_is_dir(const char* path);

bool lib_fs_is_exec(const char* path);

bool lib_fs_file_check(const char* path, lib_file_check_t check);

//// fs-cmd

/* POSIX Style                */

int lib_fs_access(const char* path, int mode);

int lib_fs_chmod(const char* path, int mode);

int lib_fs_mkdir(const char* path, int mode);

int lib_fs_mkdir_all(const char* path, int mode);

int lib_fs_chdir(const char* path);

int lib_fs_rename(const char* old_path, const char* new_path);

int lib_fs_move(const char* old_path, const char* new_path);

int lib_fs_remove(const char* path);

int lib_fs_copy(const char* src_file_name, const char* dst_file_name);

int lib_fs_copy_opts(const char* src_file_name, const char* dst_file_name, int buf_size);

int lib_fs_rmdir(const char* path);

/* Human Style                */

int lib_fs_create_dir(const char* path);

int lib_fs_create_dir_all(const char* path);

int lib_fs_remove_file(const char* path);

int lib_fs_remove_dir(const char* path);

//// fs-stat

//int lib_fs_stat(const char* path, lib_stat_t* buf);

//// fs-file

// lib_file_t* lib_fs_get_file(const char* file_name);

// const char* lib_fs_file_get_name(lib_file_t* file);

// int lib_fs_file_get_type(lib_file_t* file);

// char lib_fs_file_get_type_char(lib_file_t* file);

// char* lib_fs_file_get_uname(lib_file_t* file);

// char* lib_fs_file_get_gname(lib_file_t* file);

//

// void lib_fs_init_mode(char* mode);

// char* lib_fs_file_add_attr(lib_file_t* file, char* mode);

// char* lib_fs_file_add_mode(lib_file_t* file, char* mode);

// char lib_fs_file_get_mode_access(lib_file_t* file);

// char lib_fs_get_mode_access(const char* path);

//

// uint64_t lib_fs_file_get_size(lib_file_t* file);

// int lib_fs_file_get_mode(lib_file_t* file);

// time_t lib_fs_file_get_atime(lib_file_t* file);

// time_t lib_fs_file_get_mtime(lib_file_t* file);

// time_t lib_fs_file_get_ctime(lib_file_t* file);

// bool lib_fs_file_is_dir(lib_file_t* file);

// int lib_fs_file_get_type_by_mode(int mode);

//// fs-scan

int lib_fs_scandir(const char* dir_name, const char* pattern, lib_file_t*** files, int max_depth, int file_only);

#endif // PLAZMA_LIB_FSLIB_H