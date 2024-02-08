#include <wchar.h>

/**
 * Converts ANSII Chars to Wide Chars with a lenght
 */
wchar_t* achar_wchar(const char* str, int len);

/**
 * Converts ANSII Chars to Wide Chars 
 */
wchar_t* achar_wchar(const char* str);

/**
 * Converts Wide Chars to ANSII Chars with a lenght
 */
char* wchar_achar(const wchar_t* wstr, int wlen);

/**
 * Converts Wide Chars to ANSII Chars
 */
char* wchar_achar(const wchar_t* wstr);

////

/**
 * Converts Multi byte Chars to Wide Chars with a lenght
 */
wchar_t* char_wchar(const char* str, int len);

/**
 * Converts Multi byte Chars to Wide Chars
 */
wchar_t* char_wchar(const char* str);

/**
 * Converts Wide Chars to Multi byte Chars with a lenght
 */
char* wchar_char(const wchar_t* wstr, int wlen);

/**
 * Converts Wide Chars to Multi byte Chars
 */
char* wchar_char(const wchar_t* wstr);
