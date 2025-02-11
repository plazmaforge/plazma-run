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

#define LIB_FS_MODE_LEN    11
#define LIB_FS_ACCESS_LEN   9

typedef struct lib_fs_scan_config_t {

    int max_depth;
    bool use_stat;           // Use stat                                     : true
    bool use_dir;            // Use directory in output list                 : true
    bool use_global_pattern; // Use global pattern (only one pattern: *, *.*): true
    bool use_all;            // Use all files (., .., .git, .svn, )          : true

    lib_file_sort_t sort;

    int (*filter)(const void* v);
    int (*compare) (const void* v1, const void* v2);

} lib_fs_scan_config_t;

typedef struct lib_fs_scan_context_t {

    // config-1
    int max_depth;
    bool use_stat;           // Use stat                                     : true
    bool use_dir;            // Use directory in output list                 : true
    bool use_global_pattern; // Use global pattern (only one pattern: *, *.*): true
    bool use_all;            // Use all files (., .., .git, .svn, )          : true

    // config-2
    char** patterns;
    int pattern_count;

    // context
    //const char* dir_name;
    //int level;

    int (*filter)(const void* v);
    int (*compare) (const void* v1, const void* v2);

    lib_file_t*** files;
    int file_count;

} lib_fs_scan_context_t;

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

//// fs-scan

int lib_fs_scan_config_init(lib_fs_scan_config_t* cfg);

int lib_fs_scandir(const char* dir_name, const char* pattern, lib_file_t*** files, int max_depth, int file_only);

int lib_fs_scandir_cfg(lib_fs_scan_config_t* cfg, const char* dir_name, const char* pattern, lib_file_t*** files);

#endif // PLAZMA_LIB_FSLIB_H