#ifndef PLAZMA_LIB_STRLIB_H
#define PLAZMA_LIB_STRLIB_H

#include <stdbool.h>
#include <string.h>


int lib_strcmp(const char* str1, const char* str2);

int lib_strncmp(const char* str1, const char* str2, size_t size);

int lib_stricmp(const char* str1, const char* str2);

int lib_strnicmp(const char* str1, const char* str2, size_t size);

/**
 * Return true if a str is empty 
 */
bool lib_stremp(const char* str);

/**
 * Return safe implemantation of a str 
 */
const char* lib_strsaf(const char* str);

/**
 * [allocate]
 */
char* lib_strnew(size_t size);

/**
 * [allocate]
 */
char* lib_stradd(const char* str1, const char* str2);

// /**
//  * [allocate]
//  */
// char* lib_straddv_(const char* str1, const char* str2, const char* str3);

char* lib_strapp(const char* str1, ...);

char* lib_strappn(int n, const char* str, ...);

//char* __lib_straddv__(const char* str1, ...);

//char* __lib_straddn__(int n, const char* str1, ...);

//#define lib_straddv(...) __lib_straddv__(NULL, __VA_ARGS__, NULL)

//#define lib_straddn(n, ...) __lib_straddn__(n, NULL, __VA_ARGS__, NULL)

#define lib_strappv(...) lib_strapp(__VA_ARGS__, NULL)

char* lib_strcat(char* dst, const char* src);

char* lib_strcpy(char* dst, const char* src);

size_t lib_strlen(const char* src);

/**
 * [allocate]
 */
char* lib_strdup(const char* src);

/**
 * [allocate]
 */
char* lib_strndup(const char* src, size_t size);

/**
 * [allocate]
 */
char* lib_strdup_qt_opts(const char* src, char quote);

/**
 * [allocate]
 */
char* lib_strdup_qt(const char* src);

/**
 * [allocate]
 */
char* lib_strdup_uq(const char* src);

bool lib_is_strn_qt_opts(const char* src, size_t size, char quote);

bool lib_is_str_qt_opts(const char* src, char quote);

bool lib_is_strn_qt(const char* src, size_t size);

bool lib_is_str_qt(const char* src);

int lib_stralen(/*const*/ char** array);

void lib_strafree(char** array);

void lib_replace_n(char* path, size_t len, char from, char to);

void lib_replace(char* path, char from, char to);

char* lib_strchr_end(char* str, int c);

#endif // PLAZMA_LIB_STRLIB_H