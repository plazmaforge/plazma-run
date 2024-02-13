#include <stdlib.h>
#include <string.h>

#include "wclib.h"

static bool is_path_separator(char ch) {
    return ch  == '\\' || ch == '/';
}

bool is_wildcard_char(char ch) {
    return (ch == '*' || ch == '?' || ch == '[' || ch == ']');
}

int is_wildcard_pattern(const char* pattern) {
    return get_wildcard_index(pattern) != -1;
}

int get_wildcard_index(const char* pattern) {
    if (pattern == NULL) {
        return -1;
    }
    int len = strlen(pattern);
    if (len == 0) {
        return -1;
    }
    for (int i = 0; i < len; i++) {
        if (is_wildcard_char(pattern[i])) {
            return i;
        }
    }
    return -1;
}

// ../a/name*.text - > 4 -> '/'
int get_wildcard_path_index(const char* pattern, const char* fileName) {
    if (pattern == NULL || fileName == NULL) {
        return -1;
    }
    int wildcardIndex = get_wildcard_index(pattern);
    if (wildcardIndex < 0) {
        return -1;
    }
    return get_wildcard_path_index(wildcardIndex, fileName);
}

int get_wildcard_path_index(int wildcardIndex, const char* fileName) {
    if (fileName == NULL) {
        return -1;
    }
    int i = wildcardIndex;
    // go from end to start
    bool found = false;
    while (i >= 0) {
        if (is_path_separator(fileName[i])) {
            found = true;
            break;
        }
        i--;
    }
    return found ? i : -1;
}
