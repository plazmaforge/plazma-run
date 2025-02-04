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
 * Create new string by size
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
 * Transform a string by char
 */
char* lib_strtrc(char* str, char from, char to);

// strntrc [ALT]

/**
 * Transform a string by char, up to a maximum length
 */
char* lib_strntrc(char* str, size_t num, char from, char to);

/**
 * Transform a string by chars
 */
char* lib_strtrs(char* str, const char* from, const char* to);

/**
 * Transform a string by chars, up to a maximum length
 */
char* lib_strntrs(char* str, size_t num, const char* from, const char* to);

/**
 * Return safe implemantation of a string
 */
const char* lib_strsafe(const char* str);

// strarrnew [ALT]

/**
 * Create new NULL-terminated string array
 */
char** lib_strarrnew(size_t size);

// stralen

/**
 * Return lenght of NULL-terminated string array
 */
size_t lib_strarrlen(/*const*/ char** arr);

/**
 * Free NULL-terminated string array
 */
void lib_strarrfree(char** arr);

////

// strstarts [ALT]

bool lib_strstarts(const char* str, const char* val);

#define lib_hasstrpref(str, pref) lib_strstarts(str, pref)

// strends [ALT]

bool lib_strends(const char* str, const char* val);

#define lib_hasstrsuff(str, suff) lib_strends(str, suff)

// stristarts [ALT]

bool lib_stristarts(const char* str, const char* val);

#define lib_hasstripref(str, pref) lib_stristarts(str, pref)

// striends [ALT]

bool lib_striends(const char* str, const char* val);

#define lib_hasstrisuff(str, suff) lib_striends(str, suff)

/**
 * Return left part of a string
 * 
 * [alocate]
 */
char* lib_strleft(const char* str, size_t len);

/**
 * Return right part of a string
 * 
 * [alocate]
 */
char* lib_strright(const char* str, size_t len);


/**
 * Return a substring of a string
 * 
 * [alocate]
 */
char* lib_strsub(const char* str, size_t start, size_t len);

/**
 * Create new string and convert a string to lowercase.
 * 
 * [alocate]
 */
char* lib_tostrlwr(const char* str);

/**
 * Create new string and convert a string to uppercase.
 * 
 * [alocate]
 */
char* lib_tostrupr(const char* str);


/**
 * Create new string and convert a string to case (lower/upper/title) by mode.
 * 
 * [alocate]
 */
char* lib_tostrcase(const char* str, int mode);

#endif // PLAZMA_LIB_STRLIB_H