#ifndef PLAZMA_LIB_WCLIB_H
#define PLAZMA_LIB_WCLIB_H

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

#endif // PLAZMA_LIB_WCLIB_H
