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

/*
 * File types
 */
// #define LIB_FS_UNKNOWN       0
// #define LIB_FS_FIFO          1
// #define LIB_FS_CHR           2
// #define LIB_FS_DIR           4
// #define LIB_FS_BLK           6
// #define LIB_FS_REG           8
// #define LIB_FS_LNK          10
// #define LIB_FS_SOCK         12
// #define LIB_FS_WHT          14

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
typedef struct _stat32 lib_fs_stat_t;
#elif defined(__MINGW64_VERSION_MAJOR) && defined(_WIN64)
typedef struct _stat64 lib_fs_stat_t;
#else
typedef struct stat lib_fs_stat_t;
#endif

#ifdef _WIN32

/* File descriptor    */
typedef HANDLE lib_fs_fd_t;

/* Directory pointer  */
//typedef HANDLE lib_fs_dir_t;

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

/* Directory entry    */
// typedef struct lib_fs_dirent_t {
//     int type; // OS Indepentent
//     char* name;
//     WIN32_FIND_DATAW fd;
// } lib_fs_dirent_t;

// typedef struct lib_fs_dir_t {
//     void* ptr;
//     lib_fs_dirent_t* dirent;
// } lib_fs_dir_t;

#else

/* File descriptor    */
typedef int lib_fs_fd_t;     

/* Directory pointer  */
//typedef DIR lib_fs_dir_t;
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

/*
struct dirent {
    ino_t          d_ino;       // Inode number
    off_t          d_off;       // Offset to the next dirent
    unsigned short d_reclen;    // Length of this record
    unsigned char  d_type;      // Type of file; not supported by all file system types
    char           d_name[NAME_MAX]; // filename
};
*/

// /* Directory entry    */
// typedef struct lib_fs_dirent_t {
//     int type; // OS Indepentent
//     char* name;
//     struct dirent* fd;
// } lib_fs_dirent_t;

// typedef struct lib_fs_dir_t {
//     DIR* ptr;
//     lib_fs_dirent_t* dirent;
// } lib_fs_dir_t;

#endif

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

long lib_fs_file_get_atime(lib_fs_file_t* file);

long lib_fs_file_get_mtime(lib_fs_file_t* file);

long lib_fs_file_get_ctime(lib_fs_file_t* file);

int lib_fs_file_is_dir(lib_fs_file_t* file);

int lib_fs_file_get_type_by_mode(int mode);

////

lib_fs_file_t* lib_fs_file_new();

void lib_fs_file_free(lib_fs_file_t* file);

void lib_fs_files_free(lib_fs_file_t** files);

int lib_fs_files_init(lib_fs_file_t*** files, size_t size);

int lib_fs_files_reinit(lib_fs_file_t*** files, size_t size);

//// fs-dirent

// int lib_fs_is_dirent_dir(lib_fs_dirent_t* dirent);

// int lib_fs_get_dirent_type(lib_fs_dirent_t* dirent);

// // [allocate]
// lib_fs_dir_t* lib_fs_open_dir(const char* dir_name);

// lib_fs_dirent_t* lib_fs_read_dir(lib_fs_dir_t* dir);

// int lib_fs_close_dir(lib_fs_dir_t* dir);

//// fs-scan

int lib_fs_scandir(const char* dir_name, const char* pattern, lib_fs_file_t*** files, int max_depth, int file_only);

#endif // PLAZMA_LIB_FSLIB_H