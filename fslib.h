#ifndef PLAZMA_LIB_FSLIB_H
#define PLAZMA_LIB_FSLIB_H

#include <string>
#include <vector>
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

#define FS_DIR_SEPARATOR '\\'
#define FS_DIR_SEPARATOR_STR "\\"
#define FS_IS_DIR_SEPARATOR(c) ((c) == FS_DIR_SEPARATOR || (c) == '/')

#else

#include <dirent.h>

#define FS_DIR_SEPARATOR '/'
#define FS_DIR_SEPARATOR_STR "/"
#define FS_IS_DIR_SEPARATOR(c) ((c) == FS_DIR_SEPARATOR)

#endif

/*
 * File types
 */
#define FS_UNKNOWN       0
#define FS_FIFO          1
#define FS_CHR           2
#define FS_DIR           4
#define FS_BLK           6
#define FS_REG           8
#define FS_LNK          10
#define FS_SOCK         12
#define FS_WHT          14

typedef enum {
  FS_FILE_CHECK_IS_REGULAR    = 1 << 0,
  FS_FILE_CHECK_IS_SYMLINK    = 1 << 1,
  FS_FILE_CHECK_IS_DIR        = 1 << 2,
  FS_FILE_CHECK_IS_EXECUTABLE = 1 << 3,
  FS_FILE_CHECK_EXISTS        = 1 << 4
} fs_file_check_t;

const int FS_SCANDIR_FLAT      = -1; // Scandir flat mode (only one level)
const int FS_SCANDIR_RECURSIVE = 0;  // Scandir recursive mode

#if (defined(__MINGW64_VERSION_MAJOR) || defined(_MSC_VER)) && !defined(_WIN64)
typedef struct _stat32 fs_stat_t;
#elif defined(__MINGW64_VERSION_MAJOR) && defined(_WIN64)
typedef struct _stat64 fs_stat_t;
#else
typedef struct stat fs_stat_t;
#endif

#ifdef _WIN32

/* File descriptor    */
typedef HANDLE fs_fd_t;

/* Directory pointer  */
//typedef HANDLE fs_dir_t;

/* File Info          */
typedef BY_HANDLE_FILE_INFORMATION fs_file_info_t;

/* Directory entry    */
typedef struct fs_dirent_t {
    int type; // OS Indepentent
    char* name;
    WIN32_FIND_DATAW fd;
} fs_dirent_t;

typedef struct fs_dir_t {
    void* ptr;
    fs_dirent_t* dirent;
} fs_dir_t;

#else

/* File descriptor    */
typedef int fs_fd_t;     

/* Directory pointer  */
//typedef DIR fs_dir_t;

/* File Info          */
//#define fs_file_info_t struct stat;
typedef struct stat fs_file_info_t;

/* Directory entry    */
typedef struct fs_dirent_t {
    int type; // OS Indepentent
    char* name;
    struct dirent* fd;
} fs_dirent_t;

typedef struct fs_dir_t {
    DIR* ptr;
    fs_dirent_t* dirent;
} fs_dir_t;

#endif

typedef struct fs_file_t {
  int type;
  char* name;
  fs_stat_t* stat;
} fs_file_t;

/* C Style */

/**
 * Build full file path
 * 
 * [allocate]
 */
char* fs_get_file_path(const char* dir_name, const char* file_name);

/**
 * Return normalize full file path
 * 
 * [allocate]
 */
char* fs_get_normalize_path(const char* dir_name, const char* file_name);

/**
 * Return normalize slash file path
 * 
 * [allocate]
 */
char* fs_get_normalize_slash(char* path);

/**
 * Normalize slash file path
 */
void fs_normalize_slash(char* path);

/**
 * Return real file path
 * 
 * [allocate]
 */
char* fs_get_real_path(const char* path);

/**
 * Return current directory
 * 
 * [allocate]
 */
char* fs_get_current_dir();

/**
 * Return current find path
 * - Unix    : '.'
 * - Windows : './ *'
 */
const char* fs_get_current_find_path();


/**
 * Return true if a path is current find path
 * - Unix    : '.'
 * - Windows : './ *'
 */
int fs_is_current_find_path(const char* path);

/**
 * Return true if a file name is matched by a pattern
 */
int fs_match_file(const char* name, const char* pattern);

/* General Functions          */

#ifdef _WIN32
int fs_is_drive_path(const char* path);
#endif

int fs_is_absolute_path(const char* path);

int fs_exists(const char* file_name);

int fs_is_regular(const char* file_name);

int fs_is_dir(const char* file_name);

int fs_is_executable(const char* file_name);

int fs_file_check(const char* file_name, fs_file_check_t check);

/**
 * Return base file name
 * 
 * [allocate]
 */
char* fs_get_base_name(const char* file_name);

/**
 * Return directory name
 * 
 * [allocate]
 */
char* fs_get_dir_name (const char* file_name);

/**
 * Return base file name (alias fs_get_base_name)
 * 
 * [allocate]
 */
char* fs_get_file_name(const char* file_name);

/**
 * Return file extension
 * 
 * [allocate]
 */
char* fs_get_file_ext(const char* file_name);

/**
 * Find file extension (without allocation)
 * 
 */
const char* fs_find_file_ext(const char* file_name);

/* POSIX Style                */

int fs_access(const char* path, int mode);

int fs_chmod(const char* path, int mode);

int fs_mkdir(const char* path, int mode);

int fs_mkdir_all(const char* path, int mode);

int fs_chdir(const char* path);

int fs_rename(const char* old_path, const char* new_path);

int fs_remove(const char* path);

int fs_rmdir(const char* path);

/* Human Style                */

int fs_create_dir(const char* path);

int fs_create_dir_all(const char* path);

int fs_remove_file(const char* path);

int fs_remove_dir(const char* path);

////

int fs_stat(const char* path, fs_stat_t* buf);

////

fs_file_t* fs_get_file(const char* file_name);

const char* fs_file_get_file_name(fs_file_t* file);

int fs_file_get_file_type(fs_file_t* file);

uint64_t fs_file_get_file_size(fs_file_t* file);

int fs_file_get_file_mode(fs_file_t* file);

int fs_file_get_file_type_by_mode(int mode);

////

fs_file_t* fs_file_new();

void fs_file_free(fs_file_t* file);

void fs_files_free(fs_file_t** files);

int fs_files_init(fs_file_t*** files, size_t size);

int fs_files_reinit(fs_file_t*** files, size_t size);

////

int fs_is_dirent_dir(fs_dirent_t* dirent);

int fs_get_dirent_type(fs_dirent_t* dirent);

fs_dir_t* fs_open_dir(const char* dir_name);

fs_dirent_t* fs_read_dir(fs_dir_t* dir);

int fs_close_dir(fs_dir_t* dir);

////

int fs_scandir(const char* dir_name, const char* pattern, fs_file_t*** files, int max_depth);


/* C++ Style - Migration Task */

/**
 * Build full file path 
 */
// [allocate]
char* getFilePath(const char* dirName, const char* fileName);

/**
 * Return real file path
 */
// [allocate]
char* getRealPath(const char* path);

std::string getCurrentFindPath();

bool isCurrentFindPath(const char* path);

////

std::vector<std::string> getFiles(const char* dirName);

std::vector<std::string> getFiles(const char* dirName, const char* pattern);

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files);

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files, int level);

#endif // PLAZMA_LIB_FSLIB_H