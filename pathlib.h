#ifndef PLAZMA_LIB_PATHLIB_H
#define PLAZMA_LIB_PATHLIB_H

/*
 Returns true if a char is cross-platform path separator: '/', '\'
*/
bool path_is_path_separator(char ch);

/*
 Returns count of path level:
 'abc'         -> 1
 '/abc'        -> 1
 '/abc/def     -> 2
 '/abc/def/xyz -> 3
*/
int path_count_path_level(const char* path);

/*
 Returns path of level:
 '/abc/def/xyz', 0 -> 'abc'
 '/abc/def/xyz', 1 -> 'def'
 '/abc/def/xyz', 2 -> 'xyz'
 The level starts with '0'
*/
char* path_get_level_path(const char* path, int level);

char** path_split_path(const char* path);

#endif // PLAZMA_LIB_PATHLIB_H