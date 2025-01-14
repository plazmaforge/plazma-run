#ifndef PLAZMA_LIB_STRLIB_H
#define PLAZMA_LIB_STRLIB_H

#include <stdbool.h>
#include <string.h>

char* lib_strcat(char* dst, const char* src);

char* lib_strcpy(char* dst, const char* src);

char* lib_strncpy(char* dst, const char* src, size_t len);

const char* lib_strstr(const char* str, const char* find);

const char* lib_strnstr(const char* str, const char* find, size_t len);

//

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
char* lib_strdup_qt(const char* src);

/**
 * [allocate]
 * Quote char
 */
char* lib_strdup_qtc(const char* src, char quote);

/**
 * [allocate]
 */
char* lib_strdup_uq(const char* src);

/**
 * [allocate]
 * Quote char
 */
char* lib_strdup_uqc(const char* src, char quote);

//

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
//char* lib_stradd(const char* str1, const char* str2);

//char* lib_strapp(const char* str1, ...);

/**
 * [allocate]
 */
char* lib_strcjoin(size_t count, const char* str, ...);

/**
 * [allocate]
 */
char* __lib_strjoin__(const char* str, ...);

/**
 * [allocate]
 */
#define lib_strjoin(...) __lib_strjoin__(__VA_ARGS__, NULL)

size_t lib_strlen(const char* src);

//

bool lib_is_str_qt(const char* src);

bool lib_is_strn_qt(const char* src, size_t size);

bool lib_is_str_qtc(const char* src, char quote);

bool lib_is_strn_qtc(const char* src, size_t size, char quote);

//

int lib_stralen(/*const*/ char** array);

void lib_strafree(char** array);

void lib_strtr(char* str, char from, char to);

void lib_strntr(char* str, size_t len, char from, char to);

char* lib_strchr_end(char* str, int c);

#endif // PLAZMA_LIB_STRLIB_H