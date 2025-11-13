#ifndef PLAZMA_LIB_UNIDEF_H
#define PLAZMA_LIB_UNIDEF_H

/* The maximum possible value of a Unicode code point. See
   http://www.cl.cam.ac.uk/~mgk25/unicode.html#ucs. */

#define LI__UNI_MAX             0x10ffff
#define LIB_UNI_MAX             0x10FFFF

/* The maximum possible value which will fit into four bytes of
   UTF-8. This is larger than UNICODE_MAXIMUM. */

#define LI__UTF8_4              0x1fffff
#define LIB_UTF8_4              0x1FFFFF

/* Surrogate pair zone. */
#define LIB_UNI_SUR_HIGH_START  0xD800
#define LIB_UNI_SUR_HIGH_END    0xDBFF
#define LIB_UNI_SUR_LOW_START   0xDC00
#define LIB_UNI_SUR_LOW_END     0xDFFF

/* Start of the "not character" range. */
#define LIB_UNI_NOT_CHAR_MIN    0xFDD0

/* End of the "not character" range.  */
#define LIB_UNI_NOT_CHAR_MAX    0xFDEF

/* Error codes                        */
#define LIB_UNI_SUR_PAIR        -2
#define LIB_UNI_TOO_SMALL       -6
#define LIB_UNI_TOO_BIG         -7
#define LIB_UNI_NOT_CHAR        -8

//// uni: check

static int lib_uni_check_range(int cp) {
    if (cp < 0) {
        return LIB_UNI_TOO_SMALL; // For [int] only
    }
    if (cp > LIB_UNI_MAX) {
        return LIB_UNI_TOO_BIG;
    }
    return 0;
}

static int lib_uni_check_ffff(int cp) {
    if ((cp & 0xFFFF) >= 0xFFFE) {
        return LIB_UNI_NOT_CHAR;
    }
    return 0;
}

static int lib_uni_check_not_char(int cp) {
    if (cp >= LIB_UNI_NOT_CHAR_MIN && cp <= LIB_UNI_NOT_CHAR_MAX) {
	  return LIB_UNI_NOT_CHAR;
    }
    return 0;
}

static int lib_uni_check_sur(int cp) {
    if (cp >= LIB_UNI_SUR_HIGH_START && cp <= LIB_UNI_SUR_LOW_END) {
        return LIB_UNI_SUR_PAIR;
    }
    return 0;
}

//// uni: is

static bool lib_uni_is_not_char(int cp) {
    return lib_uni_check_not_char(cp) != 0;
}

static bool lib_uni_is_char(int cp) {
    return lib_uni_check_not_char(cp) == 0;
}

static bool lib_uni_is_sur(int cp) {
    return lib_uni_check_sur(cp) != 0;
}

static int lib_uni_is_not_ffff(int cp) {
    return lib_uni_check_ffff(cp) == 0;
}

#endif // PLAZMA_LIB_UNIDEF_H
