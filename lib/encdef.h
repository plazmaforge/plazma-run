#ifndef PLAZMA_LIB_ENCDEF_H
#define PLAZMA_LIB_ENCDEF_H

#include <stdlib.h>
#include <stdio.h>

typedef struct lib_encoding_t {
    int id;                  /* 437         , 1251               */
    const char* name;        /* 'CP437'     , 'WINDOWS-1251'     */
    const char* description; /* 'Latin (US)', 'Windows Cyrillic' */
    const char* alias;       /* alias or aliases                 */
} lib_encoding_t;

static const lib_encoding_t lib_encodings[] = {

    /*
      ISO-8859
    */

    {28591,   "ISO-8859-1", "Latin-1",                "ISO-IR-100 ISO8859-1  ISO_8859-1  ISO_8859-1:1987  L1 LATIN1 CSISOLATIN1 CP819 IBM819"},
    {28592,   "ISO-8859-2", "Latin-2",                "ISO-IR-101 ISO8859-2  ISO_8859-2  ISO_8859-2:1987  L2  LATIN2 CSISOLATIN2"},
    {28593,   "ISO-8859-3", "Latin-3/South European", "ISO-IR-109 ISO8859-3  ISO_8859-3  ISO_8859-3:1988  L3  LATIN3 CSISOLATIN3"},
    {28594,   "ISO-8859-4", "Latin-4/North European", "ISO-IR-110 ISO8859-4  ISO_8859-4  ISO_8859-4:1988  L4  LATIN4 CSISOLATIN4"},

    {28595,   "ISO-8859-5", "Latin/Cyrillic",         "ISO-IR-144 ISO8859-5  ISO_8859-5  ISO_8859-5:1988  CYRILLIC   CSISOLATINCYRILLIC"},
    {28596,   "ISO-8859-6", "Latin/Arabic",           "ISO-IR-127 ISO8859-6  ISO_8859-6  ISO_8859-6:1987  ARABIC     CSISOLATINARABIC ECMA-114 ASMO-708"}, 
    {28597,   "ISO-8859-7", "Latin/Greek",            "ISO-IR-126 ISO8859-7  ISO_8859-7  ISO_8859-7:1987  GREEK      CSISOLATINGREEK ECMA-118 ELOT_928 GREEK8 ISO_8859-7:2003"},
    {28598,   "ISO-8859-8", "Latin/Hebrew",           "ISO-IR-138 ISO8859-8  ISO_8859-8  ISO_8859-8:1988  HEBREW     CSISOLATINHEBREW"},
    {28599,   "ISO-8859-9", "Latin-5/Turkish",        "ISO-IR-148 ISO8859-9  ISO_8859-9  ISO_8859-9:1989  L5  LATIN5 CSISOLATIN5"},

    {28600,   "ISO-8859-10", "Latin-6/Nordic",        "ISO-IR-157 ISO8859-10 ISO_8859-10 ISO_8859-10:1992 L6  LATIN6 CSISOLATIN6"},
    {28601,   "ISO-8859-11", "Latin/Thai",            "ISO8859-11 ISO_8859-11"},

    {28603,   "ISO-8859-13", "Latin-7/Baltic Rim",    "ISO-IR-179 ISO8859-13 ISO_8859-13                  L7  LATIN7"},
    {28604,   "ISO-8859-14", "Latin-8/Celtic",        "ISO-IR-199 ISO8859-14 ISO_8859-14 ISO_8859-14:1998 L8  LATIN8 ISO-CELTIC"},
    {28605,   "ISO-8859-15", "Latin-9",               "ISO-IR-203 ISO8859-15 ISO_8859-15 ISO_8859-15:1998 L9  LATIN9"},
    {28606,   "ISO-8859-16", "Latin-10/SE European",  "ISO-IR-226 ISO8859-16 ISO_8859-16 ISO_8859-16:2001 L10 LATIN10"},


    /*
      MS-DOS

      >> CP853 853
      >> CP858 858
      >> CP874 WINDOWS-874
    */

    {437,     "CP437",     "Latin (US)",              "437 IBM437 CSPC8CODEPAGE437"},
    {737,     "CP737",     "MS-DOS Greek 1",          "737 IBM737"},
    {775,     "CP775",     "MS-DOS Baltic Rim",       "775 IBM775 CSPC775BALTIC"},
    {850,     "CP850",     "MS-DOS Latin 1",          "850 IBM850 CSPC850MULTILINGUAL"},
    {852,     "CP852",     "MS-DOS Latin 2",          "852 IBM852 CSPCP852"},
    {855,     "CP855",     "MS-DOS Cyrillic",         "855 IBM855 CSIBM855"},
    {857,     "CP857",     "MS-DOS Turkish",          "857 IBM857 CSIBM857"},
    {860,     "CP860",     "MS-DOS Portuguese",       "860 IBM860 CSIBM860"},
    {861,     "CP861",     "MS-DOS Icelandic",        "861 IBM861 CSIBM861 CP-IS"},
    {862,     "CP862",     "MS-DOS Hebrew",           "862 IBM862 CSPC862LATINHEBREW"},
    {863,     "CP863",     "MS-DOS French Canada",    "863 IBM863 CSIBM863"},
    {864,     "CP864",     "Arabic",                  "864 IBM864 CSIBM864"},
    {865,     "CP865",     "MS-DOS Nordic",           "865 IBM865 CSIBM865"},
    {866,     "CP866",     "MS-DOS Cyrillic Russian", "866 IBM866 CSIBM866"},
    {869,     "CP869",     "MS-DOS Greek 2",          "869 IBM869 CSIBM869 CP-GR"},


    /*
      Unicode
    */

    {65000,   "UTF-7",     "UTF-7",                   "UTF7     CSUNICODE11UTF7 UNICODE-1-1-UTF-7"},
    {65001,   "UTF-8",     "UTF-8",                   "UTF8"},
    {1065001, "UTF-8-MAC", "UTF-8-MAC",               "UTF8MAC  UTF8-MAC"},

    {1200,    "UTF-16",    "UTF-16",                  "UTF16"},
    {1201,    "UTF-16BE",  "UTF-16BE",                "UTF16BE"},
/**/{1202,    "UTF-16LE",  "UTF-16LE",                "UTF16LE"}, /* codepage = 1200  */

    {12000,   "UTF-32",    "UTF-32",                  "UTF32"},
    {12001,   "UTF-32BE",  "UTF-32BE",                "UTF32BE"},
/**/{12002,   "UTF-32LE",  "UTF-32LE",                "UTF32LE"}, /* codepage = 12000 */

    {1001200, "UCS-2",     "UCS-2",                   "UCS2     CSUNICODE       ISO-10646-UCS-2"},
    {1001201, "UCS-2BE",   "UCS-2BE",                 "UCS2BE   CSUNICODE11     UNICODEBIG UNICODE-1-1"},
    {1001202, "UCS-2LE",   "UCS-2LE",                 "UCS2LE                   UNICODELITTLE"},

    {1012000, "UCS-4",     "UCS-4",                   "UCS4     CSUCS4          ISO-10646-UCS-4"},
    {1012001, "UCS-4BE",   "UCS-4BE",                 "UCS4BE"},
    {1012002, "UCS-4LE",   "UCS-4LE",                 "UCS4LE"}

};

void lib_enc_print_encodings() {
    int size = sizeof(lib_encodings) / sizeof(lib_encoding_t);
    for (int i = 0; i < size; i++) {
        lib_encoding_t e = lib_encodings[i];
        printf("%s   \t%s\n", e.name, e.description);
    }
}

#endif // PLAZMA_LIB_ENCDEF_H
