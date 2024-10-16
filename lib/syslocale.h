#ifndef PLAZMA_LIB_SYSLOCALE_H
#define PLAZMA_LIB_SYSLOCALE_H

/*
 * LC locale definition.
 * 
 * Structures: 
 * - locale_t
 *
 */

//#ifndef LC_MESSAGES
//#define LC_MESSAGES 7
//#endif

struct lib_locale_t {
    char* name;

    char* language;
    char* script;
    char* country;
    char* variant;
    char* encoding;
};

/*
 * Sets default LC locale
 */
char* lib_locale_set_default_locale();

/*
 * Gets LC locale by category (LC_CTYPE, LC_MESSAGES ...)
 */
char* lib_locale_get_locale(int cat);

/*
 * Gets current LC locale by LC_CTYPE category
 */
char* lib_locale_get_current_locale();

/*
 * Returns true if LC locale is empty
 */
int lib_locale_is_empty(const char* locale);

/*
 * Returns true if LC locales are equal
 */
int lib_locale_equals(const char* locale1, const char* locale2);

lib_locale_t* lib_locale_new();

void lib_locale_free(lib_locale_t* locale);

/*
 * Parse LC locale
 */
lib_locale_t* lib_locale_parse_locale(const char* locale);

/*
 * Parse LC locale and return encoding
 * [allocate]
 */
char* lib_locale_parse_encoding(const char* locale);

/*
 * Load LC locale
 */
lib_locale_t* lib_locale_load_locale(int cat);

/*
 * Returns a locale name by language, country, encoding in format: <language>_<COUNTRY>.<encoding>
 * [allocate]
 */
char* lib_locale_get_locale_name(const char* language, const char* country, const char* encoding);

/*
 * Print a locale
 */
void lib_locale_print_locale(lib_locale_t* locale);

#endif // PLAZMA_LIB_SYSLOCALE_H