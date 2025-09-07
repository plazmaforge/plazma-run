#ifndef PLAZMA_LIB_UTF8LIB_H
#define PLAZMA_LIB_UTF8LIB_H

#include <stdbool.h>

/* utfdef.h */

#define LIB_UTF8_ID       65001
#define LIB_UTF8_BOM_ID 1065001

#define LIB_UTF16_ID       1200
#define LIB_UTF16BE_ID     1201
#define LIB_UTF16LE_ID     1202

#define LIB_UTF32_ID      12000
#define LIB_UTF32BE_ID    12001
#define LIB_UTF32LE_ID    12002

/*
 * Return lenght of UTF-8 char by codepoint or error (0).
 */
size_t lib_utf8_code_seq_len(int cp);

// by array (strong)
size_t lib_utf8_byte_seq_len_array(char first);

// by range (strong)
size_t lib_utf8_byte_seq_len_strong(char first);

// by range
size_t lib_utf8_byte_seq_len_range(char first);

/*
 * Return lenght of UTF-8 char by first byte or error (0).
 */
size_t lib_utf8_byte_seq_len(char first);

/*
 * Return codepoint by UTF-8 char or error (-1).
 */
int lib_utf8_get_code(const char* str);

/*
 * Convert codepoint to UTF-8 char and store it to the buffer
 * Return lenght of UTF-8 char or error (-1).
 */
int lib_utf8_to_char(char* buf, int cp);

/*
 * Convert UTF-8 char to codepoint.
 * Return lenght of utf8 char or error (-1).
 */
int lib_utf8_to_code(const char* str, int* cp);

/**
 * Get current UTF-8 char from a string and move to next UTF-8 char.
 * Store this char to the buffer.
 * The buffer must be array with min size: 4 + 1
 * Return pointer of next char or NULL if it is end of the string
 */
const char* lib_utf8_iterate(const char* str, char* buf, int* cp, int* len);

/**
 * Move to next char in a string
 * Output codepoint and lenght of next char 
 */
const char* lib_utf8_next(const char* str, int* cp, int* len);

/*
 * Encode utf8 char from codepoint and store it to the buffer
 * Return lenght of utf8 char or error (-1).
 */
int lib_utf8_encode(char* buf, int cp);

/*
 * Decode UTF-8 char to codepoint.
 * Return lenght of UTF-8 char or error (-1).
 */
int lib_utf8_decode(const char* str, int* cp);

const char* lib_utf8_strlast(const char* str);

const char* lib_utf8_strnext(const char* str);

const char* lib_utf8_strprev(const char* str);

//// str: std

/*
 * Copy UTF-8 char from src to dst
 */
void lib_utf8_chrcpy(char* dst, const char* src, int len);

/**
 * Find UTF-8 char by the index.
 * Store this char to the buffer.
 * The buffer must be array with min size: 4 + 1
 * Return codepoint of this char or error (-1).
 */
int lib_utf8_get_char(const char* str, char* buf, int index);

/**
 * Find UTF-8 char by the index.
 * First numbers only.
 * Store this char to the buffer.
 * The buffer must be array with min size: 4 + 1
 * Return codepoint of this char or error (-1).
 */
int lib_utf8_get_char_n(const char* str, size_t num, char* buf, int index);

/**
 * Get current UTF-8 char from a string and move to next UTF-8 char.
 * Store this char to the buffer.
 * The buffer must be array with min size: 4 + 1
 * Return pointer of next char or NULL if it is end of the string
 */
const char* lib_utf8_get_char_next(const char* str, char* buf);

/*
 * Return count of codepoints or error (-1).
 */
int lib_utf8_get_code_count(const char* str);

/*
 * Return count of codepoints or error (-1).
 * First numbers only.
 */
int lib_utf8_get_code_count_n(const char* str, size_t num);

int lib_utf8_get_first_byte_count(const char* str, size_t char_num);

int lib_utf8_get_first_byte_count_n(const char* str, size_t num, size_t char_num);

/**
 * Return count of bytes
 */
int lib_utf8_get_byte_count(const char* str);

/**
 * Return count of UTF-8 chars or error (-1).
 */
int lib_utf8_get_char_count(const char* str);

/*
 * Convert all chars of a string to lower/upper case
 * depends on mode: 
 *  1 - lower
 *  2 - upper
 * 
 * Return count of converted bytes or error (-1).
*/
int lib_utf8_to_case(int mode, char* str);

/*
 * Convert current char of a string to lower/upper case
 * depends on mode: 
 *  1 - lower
 *  2 - upper
 * 
 * Return count of converted bytes or error (-1).
*/
int lib_utf8_to_case_char(int mode, const char* str, char* buf);

int lib_utf8_to_case_code(int mode, int cp);

/*
 * Convert all chars of a string to lower case.
 * Return count of converted bytes or error (-1).
 */
int lib_utf8_to_lower(char* str);

/*
 * Convert current char of a string to lower case.
 * Return count of converted bytes or error (-1).
 */
int lib_utf8_to_lower_char(const char* str, char* buf);
/*
 * Convert a codepoint to lower case.
 * Return new codepoint.
 */
int lib_utf8_to_lower_code(int cp);

/*
 * Convert all chars of a string to upper case.
 * Return count of converted bytes or error (-1).
 */
int lib_utf8_to_upper(char* str);

/*
 * Convert current char of a string to upper case.
 * Return count of converted bytes or error (-1).
 */
int lib_utf8_to_upper_char(const char* str, char* buf);

/*
 * Convert the codepoint to upper case.
 * Return new codepoint.
 */
int lib_utf8_to_upper_code(int cp);

////

/**
 * Return true if UTF-8 string is valid.
 */
bool lib_utf8_is_utf8_valid(const char* str);

/**
 * Return true if UTF-8 string is valid.
 * First numbers only.
 */
bool lib_utf8_is_utf8_valid_n(const char* str, size_t num);

/**
 * Return true if UTF (UTF-8, UTF-16, UTF-32) string is valid.
 * First numbers only.
 */
bool lib_utf8_is_utf_valid_n(const char* str, size_t num);

/**
 * Return true if a string is ASCII
 */
bool lib_utf8_is_ascii(const char* str);

/**
 * Return true if a string is ASCII.
 * First numbers only.
 */
bool lib_utf8_is_ascii_n(const char* str, size_t num);

/**
 * Return true if a string is UTF-8.
 */
bool lib_utf8_is_utf8(const char* str);

/**
 * Return true if a string is UTF-8.
 * First numbers only.
 */
bool lib_utf8_is_utf8_n(const char* str, size_t num);

/*
 * Return byte order mark (BOM) of a string.
 * First numbers only.
 */
int lib_utf8_get_bom_n(const char* str, size_t num);

/*
 * Convert BOM type to a string representation.
 */
const char* lib_utf8_to_bom_str(int bom);

//// std: begin

// strlen

/*
 * Return lenght of UTF-8 string.
 */
size_t lib_utf8_strlen(const char* str);

size_t lib_utf8_strnlen(const char* str, size_t num);

// strcpy

char* lib_utf8_strcpy(char* dst, const char* src);

char* lib_utf8_strncpy(char* dst, const char* src, size_t num);

// strcat

char* lib_utf8_strcat(char* dst, const char* src);

char* lib_utf8_strncat(char* dst, const char* src, size_t num);

// strcmp

int lib_utf8_strcmp(const char* str1, const char* str2);

int lib_utf8_strncmp(const char* str1, const char* str2, size_t num);

// strchr

char* lib_utf8_strchr(const char* str, int ch);

char* lib_utf8_strrchr(const char* str, int ch);

// strstr

const char* lib_utf8_strstr(const char* str1, const char* str2);

// strlwr/upr
char* lib_utf8_strlwr(char* str);

char* lib_utf8_strupr(char* str);

// strrev

char* lib_utf8_strrev(char* str);

// strtok

// char* lib_utf8_strtok(char* str, const int* delims)

char* lib_utf8_strtok(char* str, const char* delims);

//// std: end

//// alt: begin

bool lib_utf8_stremp(const char* str);

// equals by byte
bool lib_utf8_streq(const char* str1, const char* str2);

// equals by codepoint
bool lib_utf8_strceq(const char* str1, const char* str2);

// equals by ignorte case codepoint
bool lib_utf8_strieq(const char* str1, const char* str2);

//// alt: end

//// UTF-16

int lib_utf16_char_seq_len(const char* str);

int lib_utf16be_char_seq_len(const char* str);

int lib_utf16le_char_seq_len(const char* str);

int lib_utf16_to_code(const char* str);

int lib_utf16be_to_code(const char* str);

int lib_utf16le_to_code(const char* str);

size_t lib_utf16_code_seq_len(int cp);

int lib_utf16_to_char(char* buf, int cp);

int lib_utf16be_to_char(char* buf, int cp);

int lib_utf16le_to_char(char* buf, int cp);


//// UTF-32

int lib_utf32_char_seq_len(const char* str);

int lib_utf32_to_code(const char* str);

int lib_utf32be_to_code(const char* str);

int lib_utf32le_to_code(const char* str);

size_t lib_utf32_code_seq_len(int cp);

int lib_utf32_to_char(char* buf, int cp);

int lib_utf32be_to_char(char* buf, int cp);

int lib_utf32le_to_char(char* buf, int cp);

//// UTF

size_t lib_utf_char_seq_len(int utf_id, const char* str);

size_t lib_utf_code_seq_len(int utf_id, int cp);

int lib_utf_to_char(int utf_id, char* buf, int cp);

int lib_utf_to_code(int utf_id, const char* str, int* cp);

#endif // PLAZMA_LIB_UTF8LIB_H
