#ifndef PLAZMA_LIB_STRLIB_H
#define PLAZMA_LIB_STRLIB_H

#include <stdbool.h>
#include <string.h>

char* lib_strcat(char* dst, const char* src);

size_t lib_strlcat(char* dst, const char* src, size_t num);

char* lib_strncat(char* dst, const char* src, size_t num);

char* lib_strcpy(char* dst, const char* src);

size_t lib_strlcpy(char* dst, const char* src, size_t num);

char* lib_strncpy(char* dst, const char* src, size_t num);

const char* lib_strstr(const char* str, const char* find);

const char* lib_strnstr(const char* str, const char* find, size_t num);

/**
 * [allocate]
 */
char* lib_strdup(const char* src);

/**
 * [allocate]
 */
char* lib_strndup(const char* src, size_t num);

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

int lib_strncmp(const char* str1, const char* str2, size_t num);

int lib_stricmp(const char* str1, const char* str2);

int lib_strnicmp(const char* str1, const char* str2, size_t num);

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
char* lib_strcjoin(size_t count, const char* str, ...);

char* __lib_strcatv__(char* dst, const char* str, ...);

char* __lib_strcpyv__(char* dst, const char* str, ...);

/**
 * [allocate]
 */
char* __lib_strjoin__(const char* str, ...);

/**
 * [allocate]
 */
#define lib_strjoin(...) __lib_strjoin__(__VA_ARGS__, NULL)

#define lib_strcatv(dst, ...) __lib_strcatv__(dst, __VA_ARGS__, NULL)

#define lib_strcpyv(dst, ...) __lib_strcpyv__(dst, __VA_ARGS__, NULL)

const char* lib_strchr(const char* str, int c);

const char* lib_strrchr(const char* str, int c);

size_t lib_strlen(const char* str);

size_t lib_strnlen(const char* str, size_t num);

//

bool lib_is_str_qt(const char* str);

bool lib_is_strn_qt(const char* str, size_t num);

bool lib_is_str_qtc(const char* str, char quote);

bool lib_is_strn_qtc(const char* str, size_t num, char quote);

//

int lib_stralen(/*const*/ char** array);

void lib_strafree(char** array);

char* lib_strtr(char* str, char from, char to);

char* lib_strntr(char* str, size_t len, char from, char to);

char* lib_strchr_end(char* str, int c);

//

char* lib_strlwr(char* str);

char* lib_strupr(char* str);

char* lib_strrev(char* str);

char* lib_strsep(char** str, const char* delims);

char* lib_strtok(char* str, const char* delims);

#endif // PLAZMA_LIB_STRLIB_H