#ifndef PLAZMA_LIB_FSLIB_H
#define PLAZMA_LIB_FSLIB_H

#include <string>
#include <vector>

#include "pathlib.h"


const int FS_SCANDIR_FLAT      = -1; // Scandir flat mode (only one level)
const int FS_SCANDIR_RECURSIVE = 0;  // Scandir recursive mode

struct file_t {
  const char* name;
  int type;
};

/* C Style */

/**
 * Build full file name
 */
// [allocate]
char* get_file_path(const char* dir_name, const char* file_name);

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

int files_malloc(file_t*** files, size_t size);

int files_realloc(file_t*** files, size_t size);

#endif // PLAZMA_LIB_FSLIB_H