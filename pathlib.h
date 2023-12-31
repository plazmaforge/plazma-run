#ifndef PLAZMA_LIB_PATHLIB_H
#define PLAZMA_LIB_PATHLIB_H

/*
 Returns true if a char is cross-platform path separator: '/', '\'
*/
bool isPatchChar(char ch);

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
*/
char* getLevelPath(const char* path, int level);

/*
 Returns true if a char is a wildcard char: '*', '?' , '[', ']'
*/
bool isWildcardChar(char ch);

/*
 Returns true if a pattern is a wildcard
*/
int isWildcard(const char* pattern);

/*
 Returns start position of a wildcard char
*/
int getWildcardIndex(const char* pattern);

// ../a/name*.text - > 4 -> '/'
int getWildcardPathIndex(const char* pattern, const char* fileName);

int getWildcardPathIndex(int wildcardIndex, const char* fileName);

#endif // PLAZMA_LIB_PATHLIB_H