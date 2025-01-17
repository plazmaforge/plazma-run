#ifndef PLAZMA_LIB_STRLIB_H
#define PLAZMA_LIB_STRLIB_H

#include <stdbool.h>
#include <string.h>


// strlen

size_t lib_strlen(const char* str);

size_t lib_strnlen(const char* str, size_t num);

// strnew

/**
 * [allocate]
 */
char* lib_strnew(size_t size);

// strdup

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
char* lib_strdupqt(const char* src);

/**
 * [allocate]
 * Quote char
 */
char* lib_strdupqtc(const char* src, char quote);

/**
 * [allocate]
 */
char* lib_strdupuq(const char* src);

/**
 * [allocate]
 * Quote char
 */
char* lib_strdupuqc(const char* src, char quote);

// is_strqt/qtc

bool lib_is_strqt(const char* str);

bool lib_is_strnqt(const char* str, size_t num);

bool lib_is_strqtc(const char* str, char quote);

bool lib_is_strnqtc(const char* str, size_t num, char quote);

// strcpy

char* lib_strcpy(char* dst, const char* src);

char* lib_strncpy(char* dst, const char* src, size_t num);

size_t lib_strlcpy(char* dst, const char* src, size_t num);

char* lib_strcpyv_(char* dst, const char* str, ...);

#define lib_strcpyv(dst, ...) lib_strcpyv_(dst, __VA_ARGS__, NULL)

// strcat

char* lib_strcat(char* dst, const char* src);

char* lib_strncat(char* dst, const char* src, size_t num);

size_t lib_strlcat(char* dst, const char* src, size_t num);

char* lib_strcatv_(char* dst, const char* str, ...);

#define lib_strcatv(dst, ...) lib_strcatv_(dst, __VA_ARGS__, NULL)

// strjoin

/**
 * [allocate]
 */
char* lib_strjoin_(const char* str, ...);

/**
 * [allocate]
 */
#define lib_strjoin(...) lib_strjoin_(__VA_ARGS__, NULL)

/**
 * [allocate]
 */
char* lib_strcjoin(size_t count, const char* str, ...);

// strchr

const char* lib_strchr(const char* str, int c);

const char* lib_strrchr(const char* str, int c);

char* lib_strchr_end(char* str, int c);

// strstr

const char* lib_strstr(const char* str, const char* find);

const char* lib_strnstr(const char* str, const char* find, size_t num);

// strcmp

/**
 * Return true if a str is empty 
 */
bool lib_stremp(const char* str);

int lib_strcmp(const char* str1, const char* str2);

int lib_strncmp(const char* str1, const char* str2, size_t num);

int lib_stricmp(const char* str1, const char* str2);

int lib_strnicmp(const char* str1, const char* str2, size_t num);

// strlwr/upr

char* lib_strlwr(char* str);

char* lib_strupr(char* str);

// strrev

char* lib_strrev(char* str);

// strstok/sep
char* lib_strsep(char** str, const char* delims);

char* lib_strtok(char* str, const char* delims);

// strtrc

char* lib_strtrc(char* str, char from, char to);

char* lib_strntrc(char* str, size_t len, char from, char to);

/**
 * Return safe implemantation of a str 
 */
const char* lib_strsaf(const char* str);

// stralen

int lib_stralen(/*const*/ char** array);

void lib_strafree(char** array);

#endif // PLAZMA_LIB_STRLIB_H