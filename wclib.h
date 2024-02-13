#ifndef PLAZMA_LIB_WCLIB_H
#define PLAZMA_LIB_WCLIB_H

/*
 Returns true if a char is a wildcard char: '*', '?' , '[', ']'
*/
bool wc_is_wildcard_char(char ch);

/*
 Returns true if a pattern is a wildcard
*/
int wc_is_wildcard_pattern(const char* pattern);

/*
 Returns start position of a wildcard char
*/
int wc_get_wildcard_index(const char* pattern);

// ../a/name*.text - > 4 -> '/'
int wc_get_wildcard_path_index(const char* pattern, const char* fileName);

int wc_get_wildcard_path_index(int wildcardIndex, const char* fileName);

int wc_match_file(const char* name, const char* pattern);

#endif // PLAZMA_LIB_WCLIB_H
