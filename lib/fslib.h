#ifndef PLAZMA_LIB_FSLIB_H
#define PLAZMA_LIB_FSLIB_H

#include <stdint.h>
#include <stdbool.h>
//#include <string.h>
#include <sys/stat.h>

#include "pathlib.h"

#ifdef _WIN32

#include <windows.h>

/* Stuff missing in std vc6 api */
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES -1
#endif
#ifndef FILE_ATTRIBUTE_DEVICE
#define FILE_ATTRIBUTE_DEVICE 64
#endif

#define LIB_FS_DIR_SEPARATOR '\\'
#define LIB_FS_DIR_SEPARATOR_STR "\\"
#define LIB_FS_IS_DIR_SEPARATOR(c) ((c) == LIB_FS_DIR_SEPARATOR || (c) == '/')

#else

#include <dirent.h>

#define LIB_FS_DIR_SEPARATOR '/'
#define LIB_FS_DIR_SEPARATOR_STR "/"
#define LIB_FS_IS_DIR_SEPARATOR(c) ((c) == LIB_FS_DIR_SEPARATOR)

#endif

/*
 * File types
 */
#define LIB_FS_UNKNOWN       0
#define LIB_FS_FIFO          1
#define LIB_FS_CHR           2
#define LIB_FS_DIR           4
#define LIB_FS_BLK           6
#define LIB_FS_REG           8
#define LIB_FS_LNK          10
#define LIB_FS_SOCK         12
#define LIB_FS_WHT          14

typedef enum {
  LIB_FS_FILE_CHECK_IS_REGULAR    = 1 << 0,
  LIB_FS_FILE_CHECK_IS_SYMLINK    = 1 << 1,
  LIB_FS_FILE_CHECK_IS_DIR        = 1 << 2,
  LIB_FS_FILE_CHECK_IS_EXECUTABLE = 1 << 3,
  LIB_FS_FILE_CHECK_EXISTS        = 1 << 4
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
typedef struct lib_fs_dirent_t {
    int type; // OS Indepentent
    char* name;
    WIN32_FIND_DATAW fd;
} lib_fs_dirent_t;

typedef struct lib_fs_dir_t {
    void* ptr;
    lib_fs_dirent_t* dirent;
} lib_fs_dir_t;

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
/* Directory entry    */
typedef struct lib_fs_dirent_t {
    int type; // OS Indepentent
    char* name;
    struct dirent* fd;
} lib_fs_dirent_t;

typedef struct lib_fs_dir_t {
    DIR* ptr;
    lib_fs_dirent_t* dirent;
} lib_fs_dir_t;

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

/* C Style */

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
int lib_fs_is_current_find_path(const char* path);

/**
 * Return true if a file name is matched by a pattern
 */
int lib_fs_match_file(const char* name, const char* pattern);

/* General Functions          */

#ifdef _WIN32
int lib_fs_is_drive_path(const char* path);
#endif

int lib_fs_is_absolute_path(const char* path);

int lib_fs_exists(const char* file_name);

int lib_fs_is_regular(const char* file_name);

int lib_fs_is_dir(const char* file_name);

int lib_fs_is_executable(const char* file_name);

int lib_fs_file_check(const char* file_name, lib_fs_file_check_t check);

/**
 * Return base file name
 * 
 * [allocate]
 */
char* lib_fs_get_base_name(const char* file_name);

/**
 * Return directory name
 * 
 * [allocate]
 */
char* lib_fs_get_dir_name (const char* file_name);

/**
 * Return base file name (alias lib_fs_get_base_name)
 * 
 * [allocate]
 */
char* lib_fs_get_file_name(const char* file_name);

/**
 * Return file extension
 * 
 * [allocate]
 */
char* lib_fs_get_file_ext(const char* file_name);

/**
 * Find file extension (without allocation)
 * 
 */
const char* lib_fs_find_file_ext(const char* file_name);

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

////

int lib_fs_stat(const char* path, lib_fs_stat_t* buf);

////

lib_fs_file_t* lib_fs_get_file(const char* file_name);

const char* lib_fs_file_get_file_name(lib_fs_file_t* file);

int lib_fs_file_get_file_type(lib_fs_file_t* file);

char lib_fs_file_get_file_type_char(lib_fs_file_t* file);

char* lib_fs_file_get_uname(lib_fs_file_t* file);

char* lib_fs_file_get_gname(lib_fs_file_t* file);

void lib_fs_init_mode(char* mode);

char* lib_fs_file_add_attr(lib_fs_file_t* file, char* mode);

char* lib_fs_file_add_mode(lib_fs_file_t* file, char* mode);

char lib_fs_file_get_mode_access(lib_fs_file_t* file);

char lib_fs_get_mode_access(const char* path);

uint64_t lib_fs_file_get_file_size(lib_fs_file_t* file);

int lib_fs_file_get_file_mode(lib_fs_file_t* file);

long lib_fs_file_get_file_atime(lib_fs_file_t* file);

long lib_fs_file_get_file_mtime(lib_fs_file_t* file);

long lib_fs_file_get_file_ctime(lib_fs_file_t* file);

int lib_fs_file_is_dir(lib_fs_file_t* file);

int lib_fs_file_get_file_type_by_mode(int mode);

////

lib_fs_file_t* lib_fs_file_new();

void lib_fs_file_free(lib_fs_file_t* file);

void lib_fs_files_free(lib_fs_file_t** files);

int lib_fs_files_init(lib_fs_file_t*** files, size_t size);

int lib_fs_files_reinit(lib_fs_file_t*** files, size_t size);

////

int lib_fs_is_dirent_dir(lib_fs_dirent_t* dirent);

int lib_fs_get_dirent_type(lib_fs_dirent_t* dirent);

// [allocate]
lib_fs_dir_t* lib_fs_open_dir(const char* dir_name);

lib_fs_dirent_t* lib_fs_read_dir(lib_fs_dir_t* dir);

int lib_fs_close_dir(lib_fs_dir_t* dir);

////

int lib_fs_scandir(const char* dir_name, const char* pattern, lib_fs_file_t*** files, int max_depth, int file_only);


/* C++ Style - Migration Task */

// /**
//  * Build full file path 
//  */
// // [allocate]
// char* getFilePath(const char* dirName, const char* fileName);

// /**
//  * Return real file path
//  */
// // [allocate]
// char* getRealPath(const char* path);

// std::string getCurrentFindPath();

// bool isCurrentFindPath(const char* path);

// ////

// std::vector<std::string> getFiles(const char* dirName);

// std::vector<std::string> getFiles(const char* dirName, const char* pattern);

// void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files);

// void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files, int level);

#endif // PLAZMA_LIB_FSLIB_H