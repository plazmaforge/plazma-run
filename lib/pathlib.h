#ifndef PLAZMA_LIB_PATHLIB_H
#define PLAZMA_LIB_PATHLIB_H

#include <stdbool.h>

/**
 * Return true if a char is cross-platform path separator: '/', '\' 
 */
bool lib_path_is_separator(char ch);

/**
 * Returns count of path level:
 * 'abc'         -> 1
 * '/abc'        -> 1
 * '/abc/def     -> 2
 * '/abc/def/xyz -> 3
 */
int lib_path_count_level(const char* path);

/**
 * Returns path of level:
 * '/abc/def/xyz', 0 -> 'abc'
 * '/abc/def/xyz', 1 -> 'def'
 * '/abc/def/xyz', 2 -> 'xyz'
 * The level starts with '0'
 * [allocate]
 */
char* lib_path_get_level_path(const char* path, int level);

/**
 * Split path by path separators and return NULL-terminated string array
 * [allocate]
 */
char** lib_path_split(const char* path);

/**
 * Return base name of path 
 */
char* lib_path_base(char* path);

/**
 * Return parent name of path 
 */
char* lib_path_parent(char* path);

#endif // PLAZMA_LIB_PATHLIB_H