#ifndef PLAZMA_LIB_WCLIB_H
#define PLAZMA_LIB_WCLIB_H

/*
 Returns true if a char is a wildcard char: '*', '?' , '[', ']'
*/
bool lib_wc_is_wildcard_char(char ch);

/*
 Returns true if a pattern is a wildcard
*/
int lib_wc_is_wildcard_pattern(const char* pattern);

/*
 Returns start position of a wildcard char
*/
int lib_wc_get_wildcard_index(const char* pattern);

// ../a/name*.text - > 4 -> '/'
int lib_wc_get_wildcard_path_index_by_pattern(const char* pattern, const char* file_name);

int lib_wc_get_wildcard_path_index(int wildcard_index, const char* file_name);

int lib_wc_match_file(const char* name, const char* pattern);

#endif // PLAZMA_LIB_WCLIB_H
