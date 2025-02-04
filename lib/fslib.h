#ifndef PLAZMA_LIB_FSLIB_H
#define PLAZMA_LIB_FSLIB_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "fsdirent.h"
#include "fspath.h"

#ifdef _WIN32

#include <windows.h>

#else

//#include <dirent.h>

#endif

typedef enum {
  LIB_FS_FILE_CHECK_IS_REG   = 1 << 0,
  LIB_FS_FILE_CHECK_IS_LNK   = 1 << 1,
  LIB_FS_FILE_CHECK_IS_DIR   = 1 << 2,
  LIB_FS_FILE_CHECK_IS_EXEC  = 1 << 3,
  LIB_FS_FILE_CHECK_EXISTS   = 1 << 4
} lib_fs_file_check_t;

#define LIB_FS_SCANDIR_FLAT       -1 // Scandir flat mode (only one level)
#define LIB_FS_SCANDIR_RECURSIVE   0 // Scandir recursive mode

typedef enum {
  LIB_FS_FILE_SORT_NONE           = 0,
  LIB_FS_FILE_SORT_BY_NAME        = 1,
  LIB_FS_FILE_SORT_BY_SIZE        = 2,
  LIB_FS_FILE_SORT_BY_TIME        = 3,
} lib_fs_file_sort_t;

#if (defined(__MINGW64_VERSION_MAJOR) || defined(_MSC_VER)) && !defined(_WIN64)
//typedef struct _stat32 lib_fs_stat_t;
//typedef struct _stat32 stat;
#elif defined(__MINGW64_VERSION_MAJOR) && defined(_WIN64)
//typedef struct _stat64 lib_fs_stat_t;
//typedef struct _stat64 stat;
#else
//typedef struct stat lib_fs_stat_t;
#endif

typedef struct stat lib_fs_stat_t;

#ifdef _WIN32

/* File descriptor    */
typedef HANDLE lib_fs_fd_t;

/*
typedef struct _BY_HANDLE_FILE_INFORMATION {
  DWORD    dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD    dwVolumeSerialNumber;
  DWORD    nFileSizeHigh;
  DWORD    nFileSizeLow;
  DWORD    nNumberOfLinks;
  DWORD    nFileIndexHigh;
  DWORD    nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION;
*/

/* File Info (stat)   */
typedef BY_HANDLE_FILE_INFORMATION lib_fs_file_info_t;

/*
typedef struct _WIN32_FIND_DATAW {
  DWORD    dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD    nFileSizeHigh;
  DWORD    nFileSizeLow;
  DWORD    dwReserved0;
  DWORD    dwReserved1;
  WCHAR    cFileName[MAX_PATH];
  WCHAR    cAlternateFileName[14];
  DWORD    dwFileType;    // Obsolete. Do not use.
  DWORD    dwCreatorType; // Obsolete. Do not use
  WORD     wFinderFlags;  // Obsolete. Do not use
} WIN32_FIND_DATAW, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;
*/

#else

/* File descriptor    */
typedef int lib_fs_fd_t;     

/*
typedef struct stat {
    ino_t       st_ino;         // File serial number.
    off_t       st_size;        // File size in bytes.
    dev_t       st_dev;         // ID of the device containing the file.
    dev_t       st_rdev;        // Device ID.
    uid_t       st_uid;         // User ID of file.
    gid_t       st_gid;         // Group ID of file.
    time_t      st_mtime;       // Time of last data modification.
    time_t      st_atime;       // Time when file data was last accessed.
    time_t      st_ctime;       // Time of last file status change.
    mode_t      st_mode;        // File types and permissions.
    nlink_t     st_nlink;       // Number of hard links to the file.
    blkcnt_t    st_blocks;      // Blocks allocated for a file.
    blksize_t   st_blksize;     // Preferred I/O block size for object.
}
*/

/* File Info          */
//#define lib_fs_file_info_t struct stat;
typedef struct stat lib_fs_file_info_t;

#endif

//////////////////////////////////////////////////////////////////////////////////
////
//// File Size Max (int64_t) = 9,223,372,036,854,775,807 bytes = 8 EB (exabytes)
//// C (int64_t)             = Java (long)
////
//////////////////////////////////////////////////////////////////////////////////

typedef uint32_t fid_t;
typedef uint64_t fino_t;

typedef int64_t  fsize_t;
typedef int32_t  bsize_t;

typedef struct lib_file_t {
    int                  type;        // File type
    char*                path;        // File path

    fino_t               ino;         // File serial number.
    fsize_t /*uint32_t*/ size;        // File size in bytes.
    int     /*int32_t*/  dev;         // ID of the device containing the file.
    int     /*int32_t*/  rdev;        // Device ID.
    fid_t   /*uint32_t*/ uid;         // User ID of file.
    fid_t   /*uint32_t*/ gid;         // Group ID of file.
    time_t               mtime;       // Time of last data modification.
    time_t               atime;       // Time when file data was last accessed.
    time_t               ctime;       // Time of last file status change.
    int     /*uint16_t*/ mode;        // File types and permissions.
    int     /*uint16_t*/ nlink;       // Number of hard links to the file.
    fsize_t /*int64_t*/  blocks;      // Blocks allocated for a file.
    bsize_t /*int32_t*/  blksize;     // Preferred I/O block size for object.
} lib_file_t;

typedef struct lib_fs_file_t {
  int type;
  char* name;
  lib_fs_stat_t* stat;
} lib_fs_file_t;


typedef struct lib_fs_file_data_t {
    char* data;
    size_t size;
} lib_fs_file_data_t;

#define LIB_FS_MODE_LEN    11
#define LIB_FS_ACCESS_LEN   9


/**
 * Return true if a file name is matched by a pattern
 */
int lib_fs_match_file(const char* name, const char* pattern);

//// fs-check

bool lib_fs_exists(const char* file_name);

bool lib_fs_is_reg(const char* file_name);

bool lib_fs_is_dir(const char* file_name);

bool lib_fs_is_exec(const char* file_name);

bool lib_fs_file_check(const char* file_name, lib_fs_file_check_t check);

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

int lib_fs_stat(const char* path, lib_fs_stat_t* buf);

//// fs-file

lib_fs_file_t* lib_fs_get_file(const char* file_name);

const char* lib_fs_file_get_name(lib_fs_file_t* file);

int lib_fs_file_get_type(lib_fs_file_t* file);

char lib_fs_file_get_type_char(lib_fs_file_t* file);

char* lib_fs_file_get_uname(lib_fs_file_t* file);

char* lib_fs_file_get_gname(lib_fs_file_t* file);

//

void lib_fs_init_mode(char* mode);

char* lib_fs_file_add_attr(lib_fs_file_t* file, char* mode);

char* lib_fs_file_add_mode(lib_fs_file_t* file, char* mode);

char lib_fs_file_get_mode_access(lib_fs_file_t* file);

char lib_fs_get_mode_access(const char* path);

//

uint64_t lib_fs_file_get_size(lib_fs_file_t* file);

int lib_fs_file_get_mode(lib_fs_file_t* file);

time_t lib_fs_file_get_atime(lib_fs_file_t* file);

time_t lib_fs_file_get_mtime(lib_fs_file_t* file);

time_t lib_fs_file_get_ctime(lib_fs_file_t* file);

bool lib_fs_file_is_dir(lib_fs_file_t* file);

int lib_fs_file_get_type_by_mode(int mode);

////

lib_fs_file_t* lib_fs_file_new();

void lib_fs_file_free(lib_fs_file_t* file);

void lib_fs_files_free(lib_fs_file_t** files);

int lib_fs_files_init(lib_fs_file_t*** files, size_t size);

int lib_fs_files_reinit(lib_fs_file_t*** files, size_t size);

//// fs-scan

int lib_fs_scandir(const char* dir_name, const char* pattern, lib_fs_file_t*** files, int max_depth, int file_only);

#endif // PLAZMA_LIB_FSLIB_H