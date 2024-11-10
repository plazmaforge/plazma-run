#ifndef PLAZMA_LIB_UTF8LIB_H
#define PLAZMA_LIB_UTF8LIB_H

/*
 * Return lenght of UTF-8 char by codepoint or error (0).
 */
int lib_utf8_get_codepoint_sequence_len(int cp);

// by array (strong)
int lib_utf8_get_byte_sequence_len_array(char first);

// by range (strong)
int lib_utf8_get_byte_sequence_len_strong(char first);

// by range
int lib_utf8_get_byte_sequence_len_range(char first);

/*
 * Return lenght of UTF-8 char by first byte or error (0).
 */
int lib_utf8_get_byte_sequence_len(char first);

/*
 * Return codepoint by char and sequence lenght.
 */
int lib_utf8_get_codepoint_by_len(const char* ch, int len);

/*
 * Return codepoint by char.
 */
int lib_utf8_get_codepoint(const char* ch);

/*
 * Convert codepoint to UTF-8 char.
 * Return lenght of UTF-8 char or error (-1).
 */
int lib_utf8_to_utf8(char* ch, int cp);

/*
 * Convert UTF-8 char to codepoint.
 * Return lenght of utf8 char or error (-1).
 */
int lib_utf8_to_codepoint(const char* ch, int* cp);

/*
 * Encode utf8 char from codepoint.
 * Return lenght of utf8 char or error (-1).
 */
int lib_utf8_encode(char* ch, int cp);

/*
 * Decode UTF-8 char to codepoint.
 * Return lenght of UTF-8 char or error (-1).
 */
int lib_utf8_decode(const char* ch, int* cp);

//// str

/*
 * Return lenght of UTF-8 string.
 */
int lib_utf8_strlen(const char* str);

/*
 * Return lenght of UTF-8 string.
 * First numbers only.
 */
int lib_utf8_strlen_n(const char* str, int num);

/*
 * Copy UTF-8 char from src to dst
 */
void lib_utf8_chrcpy(char* dst, char* src, int len);

/**
 * Find UTF-8 char by the index.
 * Store this char to the buffer.
 * The buffer must be array with size: 4 + 1
 * Return codepoint of this char or error (-1).
 */
int lib_utf8_get_char(const char* str, char* buf, int index);

/**
 * Find UTF-8 char by the index.
 * First numbers only.
 * Store this char to the buffer.
 * The buffer must be array with size: 4 + 1
 * Return codepoint of this char or error (-1).
 */
int lib_utf8_get_char_n(const char* str, int num, char* buf, int index);

/*
 * Calculate count of codepoints.
 */
int lib_utf8_get_codepoint_count(const char* str);

/*
 * Calculate count of codepoints.
 * First numbers only.
 */
int lib_utf8_get_codepoint_count_n(const char* str, int num);

/*
 * Convert all chars of a string to lower/upper case
 * depends on mode: 
 *  1 - lower
 *  2 - upper
 * 
 * Return count of converted chars or error (-1).
*/
int lib_utf8_to_case(int mode, const char* str);

/*
 * Convert all chars of a string to lower case.
 * Return count of converted chars or error (-1).
 */
int lib_utf8_to_lower(const char* str);

/*
 * Convert a codepoint to lower case.
 * Return new codepoint.
 */
int lib_utf8_to_lower_codepoint(int cp);

/*
 * Convert all chars of a string to upper case.
 * Return count of converted chars or error (-1).
 */
int lib_utf8_to_upper(const char* str);

/*
 * Convert the codepoint to upper case.
 * Return new codepoint.
 */
int lib_utf8_to_upper_codepoint(int cp);

////

/**
 * Return true if UTF-8 string is valid.
 */
bool lib_utf8_is_utf8_valid(const char* str);

/**
 * Return true if UTF-8 string is valid.
 * First numbers only.
 */
bool lib_utf8_is_utf8_valid_n(const char* str, int num);

/**
 * Return true if UTF (UTF-8, UTF-16, UTF-32) string is valid.
 * First numbers only.
 */
bool lib_utf8_is_utf_valid_n(const char* str, int num);

/**
 * Return true if a string is ASCII
 */
bool lib_utf8_is_ascii(const char* str);

/**
 * Return true if a string is ASCII.
 * First numbers only.
 */
bool lib_utf8_is_ascii_n(const char* str, int num);

/**
 * Return true if a string is UTF-8.
 */
bool lib_utf8_is_utf8(const char* str);

/**
 * Return true if a string is UTF-8.
 * First numbers only.
 */
bool lib_utf8_is_utf8_n(const char* str, int num);

/*
 * Return byte order mark (BOM) of a string.
 * First numbers only.
 */
int lib_utf8_get_bom_n(const char* str, int num);

/*
 * Convert BOM type to a string representation.
 */
const char* lib_utf8_to_bom_str(int bom);

#endif // PLAZMA_LIB_UTF8LIB_H
