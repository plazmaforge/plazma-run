#ifndef PLAZMA_LIB_UTF8LIB_H
#define PLAZMA_LIB_UTF8LIB_H

/*
 * Return lenght of utf8 char or error
 */
int lib_utf8_get_sequence_len(char c);

/*
 * Return codepoint by char and sequence lenght
 */
int lib_utf8_get_sequence_codepoint(const char* str, int len);

/*
 * Return codepoint by char
 */
int lib_utf8_get_codepoint(const char* str);

/*
 * Convert codepoint to utf8 char.
 * Return lenght of utf8 char or error (-1)
 */
int lib_utf8_to_utf8(char* dst, int codepoint);

/*
 * Convert utf8 char to codepoint.
 * Return lenght of utf8 char or error (-1)
 */
int lib_utf8_to_codepoint(const char* str, int* codepoint);

int lib_utf8_encode(char* dst, int codepoint);

int lib_utf8_decode(const char* str, int* codepoint);

int lib_utf8_get_codepoint_len(const char* str);

int lib_utf8_to_lower(const char* str);

int lib_utf8_to_upper(const char* str);

#endif // PLAZMA_LIB_UTF8LIB_H
