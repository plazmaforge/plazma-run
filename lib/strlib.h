#ifndef PLAZMA_LIB_STRLIB_H
#define PLAZMA_LIB_STRLIB_H

#include <stdbool.h>
#include <string.h>

// strlen [ANSI, POSIX]

size_t lib_strlen(const char* str);

// strnlen [POSIX]

size_t lib_strnlen(const char* str, size_t num);

// strnew [ALT]

/**
 * Create a string
 * 
 * [allocate]
 */
char* lib_strnew(size_t size);

// strdup [POSIX]

/**
 * Create a duplicate of a string
 * 
 * [allocate]
 */
char* lib_strdup(const char* src);

// strndup [POSIX]

/**
 * Create a duplicate of a string, up to a maximum length
 * 
 * [allocate]
 */
char* lib_strndup(const char* src, size_t num);

// strdupqt [ALT]

/**
 * Create a duplicate of a string with quotes
 * 
 * [allocate]
 */
char* lib_strdupqt(const char* src);

// strdupqtc [ALT]

/**
 * Create a duplicate of a string with quotes (char)
 * 
 * [allocate]
 */
char* lib_strdupqtc(const char* src, char quote);

/**
 * Create a duplicate of a string without quotes
 * 
 * [allocate]
 */
char* lib_strdupuq(const char* src);

/**
 * Create a duplicate of a string without quotes (char)
 * 
 * [allocate]
 */
char* lib_strdupuqc(const char* src, char quote);

// is_strqt/qtc

bool lib_is_strqt(const char* str);

bool lib_is_strnqt(const char* str, size_t num);

bool lib_is_strqtc(const char* str, char quote);

bool lib_is_strnqtc(const char* str, size_t num, char quote);

// strcpy [ANSI, POSIX]

/**
 * Copy a string
 */
char* lib_strcpy(char* dst, const char* src);

// strncpy [ANSI, POSIX]

/**
 * Copy a string, to a maximum length 
 */
char* lib_strncpy(char* dst, const char* src, size_t num);

// strlcpy [~Unix]

/**
 * Size-bounded string copying
 */
size_t lib_strlcpy(char* dst, const char* src, size_t dsize);

// strcpyv_ [ALT]

/**
 * Copy strings with NULL marker at last (variadic arguments)
 */
char* lib_strcpyv_(char* dst, const char* str, ...);

/**
 * Copy strings (variadic arguments)
 */
#define lib_strcpyv(dst, ...) lib_strcpyv_(dst, __VA_ARGS__, NULL)

// strcat [ANSI, POSIX]

/**
 * Concatenate two strings 
 */
char* lib_strcat(char* dst, const char* src);

// strncat [ANSI, POSIX]

/**
 * Concatenate two strings, up to a maximum length
 */
char* lib_strncat(char* dst, const char* src, size_t num);

// strlcat [~Unix]

/**
 * Size-bounded string concatenation
 */
size_t lib_strlcat(char* dst, const char* src, size_t dsize);

// strcatv_ [ALT]

/**
 * Concatenate strings with NULL marker at last (variadic arguments)
 */
char* lib_strcatv_(char* dst, const char* str, ...);

// strcatv [ALT]

/**
 * Concatenate strings (variadic arguments)
 */
#define lib_strcatv(dst, ...) lib_strcatv_(dst, __VA_ARGS__, NULL)

// strjoin_ [ALT]

/**
 * Join strings with NULL marker at last (variadic arguments)
 * 
 * [allocate]
 */
char* lib_strjoin_(const char* str, ...);

// strjoin [ALT]

/**
 * Join strings (variadic arguments)
 * 
 * [allocate]
 */
#define lib_strjoin(...) lib_strjoin_(__VA_ARGS__, NULL)

// strcjoin [ALT]

/**
 * Join strings
 * 
 * [allocate]
 */
char* lib_strcjoin(size_t count, const char* str, ...);

// strchr [ANSI, POSIX]

/**
 * Find the first occurrence of a character in a string 
 */
const char* lib_strchr(const char* str, int c);

// strrchr [ANSI, POSIX]

/**
 * Find the last occurrence of a character in a string 
 */
const char* lib_strrchr(const char* str, int c);

char* lib_strchr_end(char* str, int c);

// strstr [ANSI, POSIX]

/**
 * Find one string inside another 
 */
const char* lib_strstr(const char* str, const char* find);

// strnstr [~Unix]

/**
 * Find one string inside another, up to a maximum length
 */
const char* lib_strnstr(const char* str, const char* find, size_t num);

// stristr    [ALT]
// strcasestr [~Unix]

/**
 * Find one string inside another, ignoring case
 */
const char* lib_stristr(const char* str, const char* find);

/**
 * Find one string inside another, ignoring case
 */
#define lib_strcasestr(str, find) lib_stristr(str, find)

// strnistr    [ALT]
// strncasestr [~Unix]

/**
 * Find one string inside another, ignoring case, up to a maximum length
 */
const char* lib_strnistr(const char* str, const char* find, size_t num);

#define lib_strncasestr(str, find, num) lib_strnistr(str, find, num)

/**
 * Return true if a string is empty 
 */
bool lib_stremp(const char* str);

// strcmp [ANSI, POSIX]

/**
 * Compare two strings 
 */
int lib_strcmp(const char* str1, const char* str2);

// strncmp [ANSI, POSIX]

/**
 * Compare two strings, up to a given length 
 */
int lib_strncmp(const char* str1, const char* str2, size_t num);

// stricmp    [ALT]
// strcasecmp [POSIX]

/**
 * Compare two strings, ignoring case
 */
int lib_stricmp(const char* str1, const char* str2);

// strcasecmp [POSIX]

/**
 * Compare two strings, ignoring case
 */
#define lib_strcasecmp(str1, str2) lib_stricmp(str1, str2)

// strnicmp    [ALT]
// strncasecmp [POSIX]

/**
 * Compare two strings, ignoring case, up to a given length 
 */
int lib_strnicmp(const char* str1, const char* str2, size_t num);

// strncasecmp [POSIX]

/**
 * Compare two strings, ignoring case, up to a given length 
 */
#define lib_strncasecmp(str1, str2) lib_strnicmp(str1, str2)

// strlwr [~Unix]

/**
 * Convert a string to lowercase 
 */
char* lib_strlwr(char* str);

// strupr [~Unix]

/**
 * Convert a string to uppercase 
 */
char* lib_strupr(char* str);

// strrev [~ALT]

/**
 * Reverse a string 
 */
char* lib_strrev(char* str);

// strstok [~Unix]

/**
 * Separate a string into pieces marked by given delimiters 
 */
char* lib_strsep(char** str, const char* delim);

// strtok [ANSI, POSIX]
/**
 * Break a string into tokens 
 */
char* lib_strtok(char* str, const char* delim);

// strtrc [ALT]

/**
 * Transform a string 
 */
char* lib_strtrc(char* str, char from, char to);

// strntrc [ALT]

/**
 * Transform a string, up to a maximum length
 */
char* lib_strntrc(char* str, size_t num, char from, char to);

/**
 * Return safe implemantation of a string
 */
const char* lib_strsaf(const char* str);

// stralen

int lib_stralen(/*const*/ char** array);

void lib_strafree(char** array);

#endif // PLAZMA_LIB_STRLIB_H