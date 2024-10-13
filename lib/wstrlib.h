#include <wchar.h>

/**
 * Creates new string with a lenght  
 * [allocate]
 */
wchar_t* lib_wstrnew(size_t size);

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
