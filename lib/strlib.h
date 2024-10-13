#ifndef PLAZMA_LIB_STRLIB_H
#define PLAZMA_LIB_STRLIB_H

#include <string.h>

int lib_stremp(const char* str);

const char* lib_strsaf(const char* str);

/**
 * [allocate]
 */
char* lib_strnew(size_t size);

/**
 * [allocate]
 */
char* lib_strapp(const char* str1, const char* str2);

/**
 * [allocate]
 */
char* lib_strappv(const char* str1, const char* str2, const char* str3);

char* lib_strcat(char* str1, const char* str2);

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
char* lib_strdup_qt_opt(const char* src, char quote);

/**
 * [allocate]
 */
char* lib_strdup_qt(const char* src);

/**
 * [allocate]
 */
char* lib_strdup_uq(const char* src);

int lib_is_strn_qt_opt(const char* src, size_t size, char quote);

int lib_is_str_qt(const char* src, char quote);

int lib_is_strn_qt(const char* src, size_t size);

int lib_is_str_qt(const char* src);

int lib_stralen(/*const*/ char** array);

void lib_strafree(char** array);

void lib_replace_n(char* path, size_t len, char from, char to);

void lib_replace(char* path, char from, char to);

char* lib_strchr_end(char* str, int c);

#endif // PLAZMA_LIB_STRLIB_H