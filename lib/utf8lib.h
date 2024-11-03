#ifndef PLAZMA_LIB_UTF8LIB_H
#define PLAZMA_LIB_UTF8LIB_H

#define LIB_BOM_UNKNOWN    -1
#define LIB_BOM_NONE       0

#define LIB_BOM_UTF8       1
#define LIB_BOM_UTF16_BE   2
#define LIB_BOM_UTF16_LE   3
#define LIB_BOM_UTF32_BE   4
#define LIB_BOM_UTF32_LE   5

#define LIB_BOM_UTF7       6
#define LIB_BOM_UTF1       7
#define LIB_BOM_UTF_EBCDIC 8

#define LIB_BOM_SCSU       9
#define LIB_BOM_BOCU1      10
#define LIB_BOM_GB_18030   11

/*
 * Return lenght of utf8 char by codepoint or error (0)
 */
int lib_utf8_get_codepoint_sequence_len(int cp);

// by array (strong)
int lib_utf8_get_byte_sequence_len_array(char first);

// by range (strong)
int lib_utf8_get_byte_sequence_len_strong(char first);

// by range
int lib_utf8_get_byte_sequence_len_range(char first);

/*
 * Return lenght of utf8 char by first byte or error (0)
 */
int lib_utf8_get_byte_sequence_len(char first);

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

int lib_utf8_get_str_len(const char* str);

int lib_utf8_get_str_len_n(const char* str, int len);

int lib_utf8_get_codepoint_count(const char* str);

int lib_utf8_get_codepoint_count_n(const char* str, int len);

int lib_utf8_to_lower(const char* str);

int lib_utf8_to_upper(const char* str);

////

/**
 * Return true if string is UTF-8 valid
 */
bool lib_utf8_is_utf8_valid(const char* str);

/**
 * Return true if string is UTF-8 valid
 */
bool lib_utf8_is_utf8_valid_n(const char* str, int len);

/**
 * Return true if string is utf (UTF-8, UTF-16, UTF-32) valid
 */
bool lib_utf8_is_utf_valid_n(const char* str, int len);

/**
 * Return true if string is ASCII
 */
bool lib_utf8_is_ascii(const char* str);

/**
 * Return true if string is ASCII
 */
bool lib_utf8_is_ascii_n(const char* str, int len);

/**
 * Return true if string is UTF8
 */
bool lib_utf8_is_utf8(const char* str);

/**
 * Return true if string is UTF8
 */
bool lib_utf8_is_utf8_n(const char* str, int len);

/*
 * Return byte order mark (BOM) by string
 */
int lib_utf8_get_bom_n(const char* str, int len);

const char* lib_utf8_to_bom_str(int bom);

#endif // PLAZMA_LIB_UTF8LIB_H
