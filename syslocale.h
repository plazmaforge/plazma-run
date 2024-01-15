#ifndef PLAZMA_LIB_SYSLOCALE_H
#define PLAZMA_LIB_SYSLOCALE_H

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
 * Sets default C style locale
 */
char* set_default_locale();

/*
 * Gets C style locale by category (LC_CTYPE, LC_MESSAGES)
 */
char* get_locale(int cat);

/*
 * Gets current C style locale by category
 */
char* get_locale();

/*
 * Returns true if C style locale is empty
 */
int is_empty_locale(char* locale);

/*
 * Returns true if C style locales are equal
 */
int equals_locale(char* locale1, char* locale2);

/*
 * Parse C style locale
 */
locale_t* parse_locale(char* locale);

/*
 * Parse C style locale and return encoding
 */
char* parse_encoding(char* locale);

/*
 * Load C style locale
 */
locale_t* load_locale(int cat);

/*
 * Print a locale
 */
void print_locale(locale_t* locale);

#endif // PLAZMA_LIB_SYSLOCALE_H