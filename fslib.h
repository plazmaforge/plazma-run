#ifndef PLAZMA_LIB_FSLIB_H
#define PLAZMA_LIB_FSLIB_H

#include <string>
#include <vector>

#include "pathlib.h"


const int FS_SCANDIR_FLAT      = -1; // Scandir flat mode (only one level)
const int FS_SCANDIR_RECURSIVE = 0;  // Scandir recursive mode

#ifdef _WIN32
#include <windows.h>

/* File descriptor    */
typedef HANDLE fs_fd_t;

/* Directory pointer  */
//typedef HANDLE fs_dir_t;

/* File Info          */
typedef BY_HANDLE_FILE_INFORMATION fs_file_info_t;

/* Directory entry    */
typedef struct fs_dirent_t {
    //fs_file_info_t info;
    int type; // OS Indepentent
    char* name;
    WIN32_FIND_DATAW fd;
} fs_dirent_t;

typedef struct fs_dir_t {
    void* ptr;
    fs_dirent_t* dirent;
} fs_dir_t;

#else

#include <dirent.h>
#include <sys/stat.h>

/* File descriptor    */
typedef int fs_fd_t;     

/* Directory pointer  */
//typedef DIR fs_dir_t;

/* File Info          */
//#define fs_file_info_t struct stat;
typedef struct stat fs_file_info_t;

/* Directory entry    */
typedef struct fs_dirent_t {
    //fs_file_info_t info;
    int type; // OS Indepentent
    char* name;
    struct dirent* fd;
} fs_dirent_t;

typedef struct fs_dir_t {
    DIR* ptr;
    fs_dirent_t* dirent;
} fs_dir_t;

#endif

typedef struct file_t {
  const char* name;
  int type;
} file_t;

/* C Style */

/**
 * Build full file path
 */
// [allocate]
char* get_file_path(const char* dir_name, const char* file_name);

/**
 * Return normalize full file path
 */
// [allocate]
char* get_normalize_path(const char* dir_name, const char* file_name);

/**
 * Return real file path
 */
// [allocate]
char* get_real_path(const char* path);

/**
 * Return current find path
 */
const char* get_current_find_path();


/**
 * Return true if a path is current find path
 */
int is_current_find_path(const char* path);

/**
 * Return true if a file name is matched by a pattern
 */
int match_file(const char* name, const char* pattern);


////

int fs_access(const char* file_name, int mode);

int fs_chmod(const char* file_name, int mode);

int fs_mkdir(const char* file_name, int mode);

int fs_chdir(const char* file_name);

int fs_rename(const char* old_file_name, const char* new_file_name);

int fs_remove(const char* file_name);

int fs_rmdir(const char* file_name);

////

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

int scandir2(const char* dir_name, const char* pattern, file_t*** files, int max_depth);

////

file_t* file_new();

void file_free(file_t* file);

void files_free(file_t** files);

int files_init(file_t*** files, size_t size);

int files_reinit(file_t*** files, size_t size);

////

int is_dir(fs_dirent_t* dirent);

fs_dir_t* open_dir(const char* dir_name);

fs_dirent_t* read_dir(fs_dir_t* dir);

int close_dir(fs_dir_t* dir);

#endif // PLAZMA_LIB_FSLIB_H