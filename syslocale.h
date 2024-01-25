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

struct locale_t {
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
char* set_default_locale();

/*
 * Gets LC locale by category (LC_CTYPE, LC_MESSAGES)
 */
char* get_locale(int cat);

/*
 * Gets current LC locale by category
 */
char* get_locale();

/*
 * Returns true if LC locale is empty
 */
int is_empty_locale(const char* locale);

/*
 * Returns true if LC locales are equal
 */
int equals_locale(const char* locale1, const char* locale2);

locale_t* new_locale();

void free_locale(locale_t* locale);

/*
 * Parse LC locale
 */
locale_t* parse_locale(const char* locale);

/*
 * Parse LC locale and return encoding
 */
char* parse_encoding(const char* locale);

/*
 * Load LC locale
 */
locale_t* load_locale(int cat);

/*
 * Returns a locale name by language, country, encoding in format: <language>_<COUNTRY>.<encoding>
 */
char* get_locale_name(const char* language, const char* country, const char* encoding);

/*
 * Print a locale
 */
void print_locale(locale_t* locale);

#endif // PLAZMA_LIB_SYSLOCALE_H