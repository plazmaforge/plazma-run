#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LIB_DOC_UNIT      "px"
#define LIB_DOC_MAARGIN   "5px"
#define LIB_DOC_FONT_NAME "Helvetica"
#define LIB_DOC_FONT_WEIGHT NULL
#define LIB_DOC_FONT_SIZE "12px"

static bool _isdigit(char c) {
    return c == '0' 
    || c == '1'
    || c == '2'
    || c == '3'
    || c == '4'
    || c == '5'
    || c == '6'
    || c == '7'
    || c == '8'
    || c == '9';
}

static bool lib_is_digit(const char* str) {
    if (!str) {
        return false;
    }
    
    if (*str == '\0') {
        // End string
        return false;
    }

    const char* ptr = str;
    while (*ptr != '\0') {
        if (!_isdigit(*ptr)) {
            return false;
        }
        ptr++;
    }
    return true;
}

static bool lib_is_normal_digit(const char* str) {
    if (!str) {
        return false;
    }

    if (*str == '\0') {
        // End string
        return false;
    }

    if (*str == '0') {
        // Start with '0'
        // '0'                - correct
        // '01', '001', '000' - incorrect
        return *(str + 1) == '\0';
    }

    return lib_is_digit(str);
}
