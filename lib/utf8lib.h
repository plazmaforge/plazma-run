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
 * Return codepoint by UTF-8 char.
 */
int lib_utf8_get_codepoint(const char* str);

/*
 * Convert codepoint to UTF-8 char.
 * Return lenght of UTF-8 char or error (-1).
 */
int lib_utf8_to_utf8(char* buf, int cp);

/*
 * Convert UTF-8 char to codepoint.
 * Return lenght of utf8 char or error (-1).
 */
int lib_utf8_to_codepoint(const char* str, int* cp);

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
 * Encode utf8 char from codepoint.
 * Return lenght of utf8 char or error (-1).
 */
int lib_utf8_encode(char* buf, int cp);

/*
 * Decode UTF-8 char to codepoint.
 * Return lenght of UTF-8 char or error (-1).
 */
int lib_utf8_decode(const char* str, int* cp);

//// str: std

char* lib_utf8_strcat(char* dst, const char* src);

char* lib_utf8_strncat(char* dst, const char* src, size_t num);

int lib_utf8_strcmp(const char* str1, const char* str2);

int lib_utf8_strncmp(const char* str1, const char* str2, size_t num);

char* lib_utf8_strcpy(char* dst, const char* src);

char* lib_utf8_strncpy(char* dst, const char* src, size_t num);

char* lib_utf8_strchr(const char* str, int ch);

char* lib_utf8_strrchr(const char* str, int ch);

const char* lib_utf8_strstr(const char* str1, const char* str2);

// char* lib_utf8_strtok(char* str, const int* delims)

char* lib_utf8_strtok(char* str, const char* delims);

//// str: alt

const char* lib_utf8_strlast(const char* str);

const char* lib_utf8_strnext(const char* str);

const char* lib_utf8_strprev(const char* str);

//// str: std

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
int lib_utf8_get_char_n(const char* str, int num, char* buf, int index);

/**
 * Get current UTF-8 char from a string and move to next UTF-8 char.
 * Store this char to the buffer.
 * The buffer must be array with min size: 4 + 1
 * Return pointer of next char or NULL if it is end of the string
 */
const char* lib_utf8_get_char_next(const char* str, char* buf);

/*
 * Calculate count of codepoints.
 */
int lib_utf8_get_codepoint_count(const char* str);

/*
 * Calculate count of codepoints.
 * First numbers only.
 */
int lib_utf8_get_codepoint_count_n(const char* str, int num);


int lib_utf8_get_first_byte_count_n(const char* str, int num, int char_num);

int lib_utf8_get_first_byte_count(const char* str, int char_num);

/**
 * Return count of bytes
 */
int lib_utf8_get_byte_count(const char* str);

/**
 * Return count of UTF-8 chars
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
int lib_utf8_to_case(int mode, const char* str);

/*
 * Convert current char of a string to lower/upper case
 * depends on mode: 
 *  1 - lower
 *  2 - upper
 * 
 * Return count of converted bytes or error (-1).
*/
int lib_utf8_to_case_char(int mode, const char* str, char* buf);

int lib_utf8_to_case_codepoint(int mode, int cp);

/*
 * Convert all chars of a string to lower case.
 * Return count of converted bytes or error (-1).
 */
int lib_utf8_to_lower(const char* str);

/*
 * Convert current char of a string to lower case.
 * Return count of converted bytes or error (-1).
 */
int lib_utf8_to_lower_char(const char* str, char* buf);
/*
 * Convert a codepoint to lower case.
 * Return new codepoint.
 */
int lib_utf8_to_lower_codepoint(int cp);

/*
 * Convert all chars of a string to upper case.
 * Return count of converted bytes or error (-1).
 */
int lib_utf8_to_upper(const char* str);

/*
 * Convert current char of a string to upper case.
 * Return count of converted bytes or error (-1).
 */
int lib_utf8_to_upper_char(const char* str, char* buf);

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
