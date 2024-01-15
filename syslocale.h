#ifndef PLAZMA_LIB_SYSLOCALE_H
#define PLAZMA_LIB_SYSLOCALE_H

/*
 * LC locale.
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
int is_empty_locale(char* locale);

/*
 * Returns true if LC locales are equal
 */
int equals_locale(char* locale1, char* locale2);

/*
 * Parse LC locale
 */
locale_t* parse_locale(char* locale);

/*
 * Parse LC locale and return encoding
 */
char* parse_encoding(char* locale);

/*
 * Load LC locale
 */
locale_t* load_locale(int cat);

/*
 * Print a locale
 */
void print_locale(locale_t* locale);

#endif // PLAZMA_LIB_SYSLOCALE_H