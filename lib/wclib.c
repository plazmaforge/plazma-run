//#include <stdlib.h>
#include <string.h>

#include "wclib.h"

static bool lib_wc_is_path_separator(char ch) {
    return ch  == '\\' || ch == '/';
}

bool lib_wc_is_wildcard_char(char ch) {
    return (ch == '*' || ch == '?' || ch == '[' || ch == ']');
}

int lib_wc_is_wildcard_pattern(const char* pattern) {
    return lib_wc_get_wildcard_index(pattern) != -1;
}

int lib_wc_get_wildcard_index(const char* pattern) {
    if (pattern == NULL) {
        return -1;
    }
    int len = strlen(pattern);
    if (len == 0) {
        return -1;
    }
    for (int i = 0; i < len; i++) {
        if (lib_wc_is_wildcard_char(pattern[i])) {
            return i;
        }
    }
    return -1;
}

// ../a/name*.text - > 4 -> '/'
int lib_wc_get_wildcard_path_index_by_pattern(const char* pattern, const char* file_name) {
    if (pattern == NULL || file_name == NULL) {
        return -1;
    }
    int wildcard_index = lib_wc_get_wildcard_index(pattern);
    if (wildcard_index < 0) {
        return -1;
    }
    return lib_wc_get_wildcard_path_index(wildcard_index, file_name);
}

int lib_wc_get_wildcard_path_index(int wildcard_index, const char* file_name) {
    if (file_name == NULL) {
        return -1;
    }
    int i = wildcard_index;
    // go from end to start
    bool found = false;
    while (i >= 0) {
        if (lib_wc_is_path_separator(file_name[i])) {
            found = true;
            break;
        }
        i--;
    }
    return found ? i : -1;
}

int lib_wc_match_file(const char* name, const char* pattern) {
    if (name == NULL || pattern == NULL) {
        return 0;
    }

    do {
        switch (*pattern) {
        case '\0':
            /* Only end of string matches NUL */
            return *name == '\0';

        case '/':
        case '\\':
        case ':':
            /* Invalid pattern */
            return 0;

        case '?':
            /* Any character except NUL matches question mark */
            if (*name == '\0')
                return 0;

            /* Consume character and continue scanning */
            name++;
            pattern++;
            break;

        case '*':
            /* Any sequence of characters match asterisk */
            switch (pattern[1]) {
            case '\0':
                /* Trailing asterisk matches anything */
                return 1;

            case '*':
            case '?':
            case '/':
            case '\\':
            case ':':
                /* Invalid pattern */
                return 0;

            default:
                /* Find the next matching character */
                while (*name != pattern[1]) {
                    if (*name == '\0')
                        return 0;
                    name++;
                }

                /* Terminate sequence on trailing match */
                if (lib_wc_match_file(name, pattern + 1))
                    return 1;

                /* No match, continue from next character */
                name++;
            }
            break;

        default:
            /* Only character itself matches */
            if (*pattern != *name)
                return 0;

            /* Character passes */
            name++;
            pattern++;
        }
    } while (1);

    return 0;
}

