#ifndef PLAZMA_LIB_PATHLIB_H
#define PLAZMA_LIB_PATHLIB_H

/*
 Returns true if a char is cross-platform path separator: '/', '\'
*/
bool isPathSeparator(char ch);

/*
 Returns count of path level:
 'abc'         -> 1
 '/abc'        -> 1
 '/abc/def     -> 2
 '/abc/def/xyz -> 3
*/
int countPathLevel(const char* path);

/*
 Returns path of level:
 '/abc/def/xyz', 0 -> 'abc'
 '/abc/def/xyz', 1 -> 'def'
 '/abc/def/xyz', 2 -> 'xyz'
 The level starts with '0'
*/
char* getLevelPath(const char* path, int level);

char** split_path(const char* path);

#endif // PLAZMA_LIB_PATHLIB_H