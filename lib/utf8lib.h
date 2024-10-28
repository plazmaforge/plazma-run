#ifndef PLAZMA_LIB_UTF8LIB_H
#define PLAZMA_LIB_UTF8LIB_H

/*
 * Return lenght of utf8 char by first byte or error (-1)
 */
int lib_utf8_get_char_len(char c);

/*
 * Return codepoint by char and sequence lenght
 */
int lib_utf8_get_codepoint_by_len(const char* ch, int len);

/*
 * Return codepoint by char
 */
int lib_utf8_get_codepoint(const char* ch);

/*
 * Convert codepoint to utf8 char.
 * Return lenght of utf8 char or error (-1)
 */
int lib_utf8_to_utf8(char* ch, int cp);

/*
 * Convert utf8 char to codepoint.
 * Return lenght of utf8 char or error (-1)
 */
int lib_utf8_to_codepoint(const char* ch, int* cp);

/*
 * Encode utf8 char from codepoint.
 * Return lenght of utf8 char or error (-1)
 */
int lib_utf8_encode(char* ch, int cp);

/*
 * Dencode utf8 char to codepoint.
 * Return lenght of utf8 char or error (-1)
 */
int lib_utf8_decode(const char* ch, int* cp);

//// str

int lib_utf8_get_codepoint_count(const char* str);

int lib_utf8_to_lower(const char* str);

int lib_utf8_to_upper(const char* str);

#endif // PLAZMA_LIB_UTF8LIB_H
