#ifndef PLAZMA_LIB_DOCIB_H
#define PLAZMA_LIB_DOCIB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LIB_DOC_ENCODING        "utf-8"
#define LIB_DOC_CHARSET         LIB_DOC_ENCODING
#define LIB_DOC_TITLE           NULL
#define LIB_DOC_UNIT            "px"
#define LIB_DOC_MARGIN          "5px"
#define LIB_DOC_FONT_NAME       "Helvetica"
#define LIB_DOC_FONT_STYLE      NULL
#define LIB_DOC_FONT_WEIGHT     NULL
#define LIB_DOC_FONT_SIZE       "12px"

#define LIB_OPT_ENCODING        "encoding"
#define LIB_OPT_CHARSET         "charset"
#define LIB_OPT_TITLE           "title"
#define LIB_OPT_MARGIN          "margin"
#define LIB_OPT_FONT_NAME       "font-name"
#define LIB_OPT_FONT_STYLE      "font-style"
#define LIB_OPT_FONT_WEIGHT     "font-weight"
#define LIB_OPT_FONT_SIZE       "font-size"
#define LIB_OPT_WIDTH           "width"
#define LIB_OPT_HEIGHT          "height"

#define LIB_OPT_ENCODING_ID     1000
#define LIB_OPT_CHARSET_ID      1001
#define LIB_OPT_TITLE_ID        2001
#define LIB_OPT_MARGIN_ID       3001
#define LIB_OPT_FONT_NAME_ID    4001
#define LIB_OPT_FONT_STYLE_ID   4002
#define LIB_OPT_FONT_WEIGHT_ID  4003
#define LIB_OPT_FONT_SIZE_ID    4004
#define LIB_OPT_WIDTH_ID        5001
#define LIB_OPT_HEIGHT_ID       5002

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

inline static const char* lib_ifs(bool flag, const char* s1, const char* s2) {
    return flag ? s1 : s2;
}

inline static char lib_ifc(bool flag, char c1, char c2) {
    return flag ? c1 : c2;
}

inline static int lib_ifi(bool flag, int i1, int i2) {
    return flag ? i1 : i2;
}

static long lib_ifl(bool flag, long l1, long l2) {
    return flag ? l1 : l2;
}

inline static const char* lib_defs(const char* s1, const char* s2) {
    return s1 ? s1 : s2;
}

////

static const char* lib_doc_unitdef(const char* value) {
    return lib_is_digit(value) ? LIB_DOC_UNIT : "";
}

#endif // PLAZMA_LIB_DOCLIB_H