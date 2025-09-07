#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "encdef.h"

/*
  Character encoding mappings and related files
  https://www.unicode.org/L2/L1999/99325-E.htm
  http://www.unicode.org/Public/MAPPINGS/

  Code Pages
  https://en.wikipedia.org/wiki/Code_page
  https://en.wikipedia.org/wiki/ISO/IEC_8859#The_parts_of_ISO/IEC_8859

  Java Encodings
  https://docs.oracle.com/javase/8/docs/technotes/guides/intl/encoding.doc.html

  Mozilla Encodings
  https://developer.mozilla.org/en-US/docs/Web/API/Encoding_API/Encodings

  W3C Encodings
  https://www.w3.org/International/docs/encoding/

  File.encoding values and IBM i CCSID
  https://www.ibm.com/docs/en/i/7.4.0?topic=encodings-fileencoding-values-i-ccsid

  CCSID values defined on IBM i
  https://www.ibm.com/docs/en/i/7.4.0?topic=information-ccsid-values-defined-i

  INA Character Sets
  https://www.iana.org/assignments/character-sets/character-sets.xhtml

  Microsoft Code Pages
  https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
*/

static const lib_encoding_t lib_encodings[] = {

    /*
      ISO-8859

      CE European: Central or Eastern European
      SE European: South or Eastern European
    */

    {28591,   "ISO-8859-1", "Latin-1//Western European",        "ISO-IR-100 ISO8859-1   ISO_8859-1  ISO_8859-1:1987  L1  LATIN1  LATIN-1  CSISOLATIN1  CSISO88591  ISO_8859-1:1998 CP819 IBM819"},
    {28592,   "ISO-8859-2", "Latin-2/Central-Eastern European", "ISO-IR-101 ISO8859-2   ISO_8859-2  ISO_8859-2:1987  L2  LATIN2  LATIN-2  CSISOLATIN2  CSISO88592  ISO_8859-2:1999"},
    {28593,   "ISO-8859-3", "Latin-3/South European",           "ISO-IR-109 ISO8859-3   ISO_8859-3  ISO_8859-3:1988  L3  LATIN3  LATIN-3  CSISOLATIN3  CSISO88593  ISO_8859-3:1999"},
    {28594,   "ISO-8859-4", "Latin-4/North European",           "ISO-IR-110 ISO8859-4   ISO_8859-4  ISO_8859-4:1988  L4  LATIN4  LATIN-4  CSISOLATIN4  CSISO88594  ISO_8859-4:1998"},

    {28595,   "ISO-8859-5", "Latin/Cyrillic",                   "ISO-IR-144 ISO8859-5   ISO_8859-5  ISO_8859-5:1988  CYRILLIC    CSISOLATINCYRILLIC    CSISO88595  ISO_8859-5:1999"},
    {28596,   "ISO-8859-6", "Latin/Arabic",                     "ISO-IR-127 ISO8859-6   ISO_8859-6  ISO_8859-6:1987  ARABIC      CSISOLATINARABIC      CSISO88596  ISO_8859-6:1999 ECMA-114 ASMO-708"}, 
    {28597,   "ISO-8859-7", "Latin/Greek",                      "ISO-IR-126 ISO8859-7   ISO_8859-7  ISO_8859-7:1987  GREEK       CSISOLATINGREEK       CSISO88597  ISO_8859-7:2003 ECMA-118 ELOT_928 GREEK8"},
    {28598,   "ISO-8859-8", "Latin/Hebrew",                     "ISO-IR-138 ISO8859-8   ISO_8859-8  ISO_8859-8:1988  HEBREW      CSISOLATINHEBREW      CSISO88598  ISO_8859-8:1999"},
    {28599,   "ISO-8859-9", "Latin-5/Turkish",                  "ISO-IR-148 ISO8859-9   ISO_8859-9  ISO_8859-9:1989  L5  LATIN5  LATIN-5  CSISOLATIN5  CSISO88599  ISO_8859-9:1999"},

    {28600,   "ISO-8859-10", "Latin-6/Nordic",                  "ISO-IR-157 ISO8859-10  ISO_8859-10 ISO_8859-10:1992 L6  LATIN6  LATIN-6  CSISOLATIN6  CSISO885910 ISO_8859-10:1998"},
    {28601,   "ISO-8859-11", "Latin/Thai",                      "           ISO8859-11  ISO_8859-11 ISO_8859-11:2001                                   CSISO885911"},

    {28603,   "ISO-8859-13", "Latin-7/Baltic Rim",              "ISO-IR-179 ISO8859-13  ISO_8859-13 ISO_8859-13:1998 L7  LATIN7  LATIN-7  CSISOLATIN7  CSISO885913"}, // WINDOWS-1257
    {28604,   "ISO-8859-14", "Latin-8/Celtic",                  "ISO-IR-199 ISO8859-14  ISO_8859-14 ISO_8859-14:1998 L8  LATIN8  LATIN-8  CSISOLATIN8  CSISO885914 ISO-CELTIC"},
    {28605,   "ISO-8859-15", "Latin-9/Western European",        "ISO-IR-203 ISO8859-15  ISO_8859-15 ISO_8859-15:1999 L9  LATIN9  LATIN-9  CSISOLATIN9  CSISO885915 LATIN0 LATIN-0"},
    {28606,   "ISO-8859-16", "Latin-10/South-Eastern European", "ISO-IR-226 ISO8859-16  ISO_8859-16 ISO_8859-16:2001 L10 LATIN10 LATIN-10 CSISOLATIN10 CSISO885916"},


    /*
      DOS

      >> CP853 853
      >> CP858 858
      >> CP874 WINDOWS-874
    */

    {437,     "CP437",       "DOS Latin US",                             "437  IBM437 CSPC8CODEPAGE437"},
    {737,     "CP737",       "DOS Greek 1",                              "737  IBM737"},
    {775,     "CP775",       "DOS Baltic Rim",                           "775  IBM775 CSPC775BALTIC"},
    {850,     "CP850",       "DOS Latin 1",                              "850  IBM850 CSPC850MULTILINGUAL"},
    {852,     "CP852",       "DOS Latin 2",                              "852  IBM852 CSPCP852"},
    {855,     "CP855",       "DOS Cyrillic",                             "855  IBM855 CSIBM855"},
    {857,     "CP857",       "DOS Turkish",                              "857  IBM857 CSIBM857"},
    {860,     "CP860",       "DOS Portuguese",                           "860  IBM860 CSIBM860"},
    {861,     "CP861",       "DOS Icelandic",                            "861  IBM861 CSIBM861 CP-IS"},
    {862,     "CP862",       "DOS Hebrew",                               "862  IBM862 CSPC862LATINHEBREW"},
    {863,     "CP863",       "DOS French Canada",                        "863  IBM863 CSIBM863"},
    {864,     "CP864",       "DOS Arabic",                               "864  IBM864 CSIBM864"},
    {865,     "CP865",       "DOS Nordic",                               "865  IBM865 CSIBM865"},
    {866,     "CP866",       "DOS Cyrillic",                             "866  IBM866 CSIBM866"},
    {869,     "CP869",       "DOS Greek 2",                              "869  IBM869 CSIBM869 CP-GR"},

    /*
      WINDOWS
    */

    {1250,    "CP1250",      "Windows Latin 2/Central-Eastern European", "1250 WINDOWS-1250 MS-EE"},
    {1251,    "CP1251",      "Windows Cyrillic",                         "1251 WINDOWS-1251 MS-CYRL"},
    {1252,    "CP1252",      "Windows Latin 1/Western European",         "1252 WINDOWS-1252 MS-ANSI"},
    {1253,    "CP1253",      "Windows Greek",                            "1253 WINDOWS-1253 MS-GREEK"},
    {1254,    "CP1254",      "Windows Turkish",                          "1254 WINDOWS-1254 MS-TURK"},
    {1255,    "CP1255",      "Windows Hebrew",                           "1255 WINDOWS-1255 MS-HEBR"},
    {1256,    "CP1256",      "Windows Arabic",                           "1256 WINDOWS-1256 MS-ARAB"},
    {1257,    "CP1257",      "Windows Baltic",                           "1257 WINDOWS-1257 WINBALTRIM"},
    {1258,    "CP1258",      "Windows Vietnamese",                       "1258 WINDOWS-1258"},

    {874,     "CP874",      "Windows Thai",                              "874  WINDOWS-874"},

    /*
      Unicode
    */

    {65000,   "UTF-7",       "UTF-7",                               "UTF7        CSUNICODE11UTF7 UNICODE-1-1-UTF-7"},
    {65001,   "UTF-8",       "UTF-8",                               "UTF8"},
/**/{1065001, "UTF-8-BOM",   "UTF-8 with BOM",                      "UTF8BOM     UTF8-BOM"},

    {1200,    "UTF-16",      "UTF-16 Big Endian with optional BOM", "UTF16       UTF_16"},
    {1201,    "UTF-16BE",    "UTF-16 Big Endian",                   "UTF16BE     UTF-16-BE      UTF16-BE     UTF_16_BE"},
/**/{1202,    "UTF-16LE",    "UTF-16 Little Endian",                "UTF16LE     UTF-16-LE      UTF16-LE     UTF_16_LE"},                 /* codepage = 1200  */
/**/{1203,    "UTF-16BE-BOM","UTF-16 Big Endian with BOM",          "UTF16BEBOM  UTF-16-BE-BOM  UTF16-BE-BOM UTF_16_BE_BOM UTF16BE-BOM"}, /* codepage = 1200  */
/**/{1204,    "UTF-16LE-BOM","UTF-16 Little Endian with BOM",       "UTF16LEBOM  UTF-16-LE-BOM  UTF16-LE-BOM UTF_16_LE_BOM UTF16LE-BOM"}, /* codepage = 1200  */

    {12000,   "UTF-32",      "UTF-32 Big Endian with optional BOM", "UTF32       UTF_32"},
    {12001,   "UTF-32BE",    "UTF-32 Big Endian",                   "UTF32BE     UTF-32-BE      UTF32-BE     UTF_32_BE"},
/**/{12002,   "UTF-32LE",    "UTF-32 Little Endian",                "UTF32LE     UTF-32-LE      UTF32-LE     UTF_32_LE"},                 /* codepage = 12000 */
/**/{12003,   "UTF-32BE-BOM","UTF-32 Big Endian with BOM",          "UTF32BEBOM  UTF-32-BE-BOM  UTF32-BE-BOM UTF_32_BE_BOM UTF32BE-BOM"}, /* codepage = 12000 */
/**/{12004,   "UTF-32LE-BOM","UTF-32 Little Endian with BOM",       "UTF32LE     UTF-32-LE-BOM  UTF32-LE-BOM UTF_32_LE_BOM UTF32LE-BOM"}, /* codepage = 12000 */

/**/{1001200, "UCS-2",       "UCS-2",                               "UCS2        CSUNICODE      ISO-10646-UCS-2"},
/**/{1001201, "UCS-2BE",     "UCS-2BE",                             "UCS2BE      CSUNICODE11    UNICODEBIG UNICODE-1-1"},
/**/{1001202, "UCS-2LE",     "UCS-2LE",                             "UCS2LE                     UNICODELITTLE"},

/**/{1012000, "UCS-4",       "UCS-4",                               "UCS4        CSUCS4         ISO-10646-UCS-4"},
/**/{1012001, "UCS-4BE",     "UCS-4BE",                             "UCS4BE"},
/**/{1012002, "UCS-4LE",     "UCS-4LE",                             "UCS4LE"}

};

/**
 * Returns true if the encoding id is ISO type 
 */
bool lib_enc_is_iso_encoding(int id) {
  return (   id == 28591
          || id == 28592
          || id == 28593
          || id == 28594
          || id == 28595
          || id == 28596
          || id == 28597
          || id == 28598
          || id == 28599
          || id == 28600
          || id == 28601

          || id == 28603
          || id == 28604
          || id == 28605
          || id == 28606);
}

/**
 * Returns true if the encoding id is DOS type 
 */
bool lib_enc_is_dos_encoding(int id) {
  return (   id == 437
          || id == 737
          || id == 775
          || id == 850
          || id == 852
          || id == 855
          || id == 857

          || id == 860
          || id == 861
          || id == 862
          || id == 863
          || id == 864
          || id == 865
          || id == 866

          || id == 869);
}

/**
 * Returns true if the encoding id is WIN type 
 */
bool lib_enc_is_win_encoding(int id) {
  return (   id == 1250
          || id == 1251
          || id == 1252
          || id == 1253
          || id == 1254
          || id == 1255
          || id == 1256
          || id == 1257
          || id == 1258

          /* CP874 */ /* WINDOWS-874 ?*/
          || id == 874
          
          );
}

/**
 * Returns true if the encoding id is UTF type 
 */
bool lib_enc_is_utf_encoding(int id) {
  return (   id == LIB_ENC_UTF7_ID
          || id == LIB_ENC_UTF8_ID
    /**/  || id == LIB_ENC_UTF8_BOM_ID

          || id == LIB_ENC_UTF16_ID
          || id == LIB_ENC_UTF16BE_ID
    /**/  || id == LIB_ENC_UTF16LE_ID      /* codepage = 1200  */
    /**/  || id == LIB_ENC_UTF16BE_BOM_ID  /* codepage = 1200  */
    /**/  || id == LIB_ENC_UTF16LE_BOM_ID  /* codepage = 1200  */

          || id == LIB_ENC_UTF32_ID
          || id == LIB_ENC_UTF32BE_ID      /* codepage = 12000 */
    /**/  || id == LIB_ENC_UTF32BE_BOM_ID  /* codepage = 12000 */
    /**/  || id == LIB_ENC_UTF32LE_BOM_ID  /* codepage = 12000 */

        );
}

bool lib_enc_is_utf16or32_encoding(int id) {
  return (   id == LIB_ENC_UTF16_ID
          || id == LIB_ENC_UTF16BE_ID
    /**/  || id == LIB_ENC_UTF16LE_ID     /* codepage = 1200  */
    /**/  || id == LIB_ENC_UTF16BE_BOM_ID /* codepage = 1200  */
    /**/  || id == LIB_ENC_UTF16LE_ID     /* codepage = 1200  */

          || id == LIB_ENC_UTF32_ID
          || id == LIB_ENC_UTF32BE_ID
    /**/  || id == LIB_ENC_UTF32LE_ID     /* codepage = 12000  */
    /**/  || id == LIB_ENC_UTF32BE_BOM_ID /* codepage = 12000  */
    /**/  || id == LIB_ENC_UTF32LE_BOM_ID /* codepage = 12000  */

    );
}

/**
 * Returns true if the encoding id is UCS type 
 */
bool lib_enc_is_ucs_encoding(int id) {
  return (   id == 1001200
    /**/  || id == 1001201
    /**/  || id == 1001202

    /**/  || id == 1012000
    /**/  || id == 1012001
    /**/  || id == 1012002);
}

/**
 * Converts DOS encoding id to WIN encoding id
 */
int lib_enc_dos_to_win(int id) {
    /*
     437 -> 1252 Latin 1, Western European
     737 -> 1253 Greek
     775 -> 1257 Baltic
     850 -> 1252 Latin 1, Western European
     852 -> 1250 Latin 2, Central European
     855 -> 1251 Cyrilik
     857 -> 1254 Turkish
     860 -> ---- Portuguese
     861 -> ---- Icelandic
     862 -> 1255 Hebrew
     863 -> ---- French Canada
     864 -> 1256 Arabic
     865 -> ---- Nordic
     866 -> 1251 Cyrilik
     869 -> 1253 Greek
    */
    switch (id) {
        case 437:
            return 1252; // Latin 1, Western European
        case 737:
            return 1253; // Greek
        case 775:
            return 1257; // Baltic
        case 850:
            return 1252; // Latin 1, Western European
        case 852:
            return 1250; // Latin 2, Central European
        case 855:
            return 1251; // Cyrilik
        case 857:
            return 1254; // Turkish
        case 860:
            return 1252; // Latin 1, Western European (?)
        case 861:
            return 1252; // Latin 1, Western European (?)
        case 862:
            return 1255; // Hebrew
        case 863:
            return 1252; // Latin 1, Western European (?)
        case 864:
            return 1256; // Arabic
        case 865:
            return 1252; // Latin 1, Western European (?)
        case 866:
            return 1251; // Cyrilik
        case 869:
            return 1253; // Greek
    }
    return 0;  
}

/**
 * Returns encoding type by encoding id
 */
int lib_enc_get_encoding_type(int id) {

  if (lib_enc_is_iso_encoding(id)) {
    return LIB_ENC_ISO_TYPE;
  } else if (lib_enc_is_dos_encoding(id)) {
    return LIB_ENC_DOS_TYPE;
  } else if (lib_enc_is_win_encoding(id)) {
    return LIB_ENC_WIN_TYPE;
  } else if (lib_enc_is_utf_encoding(id)) {
    return LIB_ENC_UTF_TYPE;
  } else if (lib_enc_is_ucs_encoding(id)) {
    return LIB_ENC_UCS_TYPE;
  }

  return 0;
}

/**
 * Returns name of encoding type by encoding id
 */
const char* lib_enc_get_encoding_type_name(int id) {
  
  // Get encoding type by id
  int type = lib_enc_get_encoding_type(id);

  if (type == LIB_ENC_ISO_TYPE) {
    return "ISO-8859";
  } else if (type == LIB_ENC_DOS_TYPE) {
    return "DOS";
  } else if (type == LIB_ENC_WIN_TYPE) {
    return "Windows";
  } else if (type == LIB_ENC_UTF_TYPE) {
    return "UTF";
  } else if (type == LIB_ENC_UCS_TYPE) {
    return "UCS";
  }

  return "";
}

static char* _to_case(int mode, char* str) {
    if (!str) {
        return str;
    }
    size_t len = strlen(str);
    if (len == 0) {
        return str;
    }
    char* chr = str;
    for (int i = 0; i < len; i++) {
        *chr = mode ? toupper(*chr) : tolower(*chr);
        chr++;
    }
    return str;
}

bool _lib_enc_equals(const char* name, lib_encoding_t* encoding) {
  if (!name || !encoding) {
    return false;
  }
  size_t len = strlen(name);
  if (len == 0) {
    return false;
  }

  // Convert encoding name to upper case
  char uname[len + 1];
  strcpy(uname, name);
  uname[len] = '\0';
  _to_case(1, uname);


  // Search by name
  if (encoding->name && strcmp(uname, encoding->name) == 0) {
    return true;
  }

  // Search by alias
  if (encoding->alias) {
    const char* alias = encoding->alias;
    const char* s = strstr(alias, uname);
    if (!s) {
      return false;
    }

    int start = (int) (s - alias);
    int end   = start + len - 1;

    //fprintf(stderr, ">> name     : %s\n", uname);
    //fprintf(stderr, ">> alias    : %s\n", alias);
    //fprintf(stderr, ">> start    : %d\n", start);
    //fprintf(stderr, ">> end      : %d\n", end);
    //fprintf(stderr, ">> alias[%d]: %c\n", start, alias[start]);
    //fprintf(stderr, ">> alias[%d]: %c\n", end, alias[end]);

    bool is_start = false;
    bool is_end = false;

    if (start == 0 || alias[start - 1] == ' ') {
      is_start = true;
    }
    if (end == strlen(alias) - 1 || alias[end + 1] == ' ') {
      is_end = true;
    }

    //fprintf(stderr, ">> is_start : %d\n", is_start);
    //fprintf(stderr, ">> is_end   : %d\n", is_end);

    return is_start && is_end;
  }

  return false;
}

/**
 * Returns encoding id by encoding name
 */
int lib_enc_get_encoding_id(const char* name) {
  if (!name) {
    return 0;
  }
  size_t size = lib_enc_get_encoding_size();
  for (size_t i = 0; i < size; i++) {
    lib_encoding_t e = lib_encodings[i];

    // TODO: Use alieas too
    if (_lib_enc_equals(name, &e)) {
      return e.id;
    }    
  }  
  return 0;
}

bool lib_enc_has_encoding_id(int id) {
    if (id <= 0) {
        return false;
    }
    size_t size = lib_enc_get_encoding_size();
    for (size_t i = 0; i < size; i++) {
        lib_encoding_t e = lib_encodings[i];
        if (e.id == id) {
            return true;
        }    
    }
    return false;
}

size_t lib_enc_get_encoding_size() {
  return sizeof(lib_encodings) / sizeof(lib_encoding_t);
}

lib_encoding_t lib_enc_get_encoding(size_t index) {
  return lib_encodings[index];
}

void lib_enc_print_encodings() {
    size_t size = lib_enc_get_encoding_size();
    for (size_t i = 0; i < size; i++) {
        lib_encoding_t e = lib_encodings[i];
        //printf("%s   \t%s\t\t %s\n", e.name, e.description, lib_enc_get_encoding_type_name(e.id));
        printf("%-15s %-45s %-15s %s\n", e.name, e.description, lib_enc_get_encoding_type_name(e.id), e.alias);
    }
}
