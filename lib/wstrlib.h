#include <wchar.h>

#define lib_wcslen(str) lib_wstrlen(str)

#define lib_wcsnlen(str, num) lib_wstrnlen(str, num)

#define lib_wcsnew(size) lib_wstrnew(size)

#define lib_wcsdup(src) lib_wstrdup(src)

#define lib_wcsndup(src, num) lib_wstrndup(src, num)

// wstrlen

size_t lib_wstrlen(const wchar_t* str);

size_t lib_wstrnlen(const wchar_t* str, size_t num);

// wstrnew

/**
 * Creates new wstring with a lenght
 * [allocate]
 */
wchar_t* lib_wstrnew(size_t size);

// wstrdup

/**
 * [allocate]
 */
wchar_t* lib_wstrdup(const wchar_t* src);

/**
 * [allocate]
 */
wchar_t* lib_wstrndup(const wchar_t* src, size_t num);

////

/**
 * Converts ANSII Chars to Wide Chars with a lenght
 * [allocate]
 */
wchar_t* lib_acs_to_wcs_n(const char* str, int len);

/**
 * Converts ANSII Chars to Wide Chars 
 * [allocate]
 */
wchar_t* lib_acs_to_wcs(const char* str);

/**
 * Converts Wide Chars to ANSII Chars with a lenght
 * [allocate]
 */
char* lib_wcs_to_acs_n(const wchar_t* wstr, int wlen);

/**
 * Converts Wide Chars to ANSII Chars
 * [allocate]
 */
char* lib_wcs_to_acs(const wchar_t* wstr);

////

/**
 * Converts Multi byte Chars to Wide Chars with a lenght
 * [allocate]
 */
wchar_t* lib_mbs_to_wcs_n(const char* str, int len);

/**
 * Converts Multi byte Chars to Wide Chars
 * [allocate]
 */
wchar_t* lib_mbs_to_wcs(const char* str);

/**
 * Converts Wide Chars to Multi byte Chars with a lenght
 * [allocate]
 */
char* lib_wcs_to_mbs_n(const wchar_t* wstr, int wlen);

/**
 * Converts Wide Chars to Multi byte Chars
 * [allocate]
 */
char* lib_wcs_to_mbs(const wchar_t* wstr);
