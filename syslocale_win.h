#ifndef PLAZMA_LIB_SYSLOCALE_WIN_H
#define PLAZMA_LIB_SYSLOCALE_WIN_H

#if defined _WIN32
#include <io.h>
#include <windows.h>
//#include <shlobj.h>
//#include <objidl.h>

#include "strlib.h"
#include "syslocale.h"

#define PROPSIZE 9      // eight-letter + null terminator
#define SNAMESIZE 86    // max number of chars for LOCALE_SNAME is 85

/*
 * Returns Windows Locale ID (LCID) by locale category (LC_CTYPE, LC_MESSAGES)
 */
static LCID getLocaleID(int cat) {

   if (cat == LC_CTYPE) {
      return GetUserDefaultLCID(); // LC_CTYPE
   }

   // query the system for the current system default locale
   // (which is a Windows LCID value),
   LCID userDefaultLCID = GetUserDefaultLCID();
   LANGID userDefaultUILang = GetUserDefaultUILanguage();
   LCID userDefaultUILCID = MAKELCID(userDefaultUILang, SORTIDFROMLCID(userDefaultLCID));

   //char * display_encoding;
   //HANDLE hStdOutErr;

    // Windows UI Language selection list only cares "language"
    // information of the UI Language. For example, the list
    // just lists "English" but it actually means "en_US", and
    // the user cannot select "en_GB" (if exists) in the list.
    // So, this hack is to use the user LCID region information
    // for the UI Language, if the "language" portion of those
    // two locales are the same.
    if (PRIMARYLANGID(LANGIDFROMLCID(userDefaultLCID)) == PRIMARYLANGID(userDefaultUILang)) {
      userDefaultUILCID = userDefaultLCID;
    }

    return userDefaultUILCID; // LC_MESSAGES = LC_TIME + 1
}

/*
 * Returns Windows codepage by Locale ID
 */

static int getCodepage(LCID lcid) {
    int codepage = 0;
    char ret[16];
    if (ret == NULL) {
        return NULL;
    }

    if (lcid == 0) {
        codepage = GetACP();
        _itoa_s(codepage, ret + 2, 14, 10);
    } else if (GetLocaleInfo(lcid, LOCALE_IDEFAULTANSICODEPAGE, ret + 2, 14) != 0) {
        codepage = atoi(ret + 2);
    } 
    return codepage;
}

/*
 * Returns Windows codepage by locale category (LC_CTYPE, LC_MESSAGES)
 */

int getCodepage(int cat) {
    LCID lcid = getLocaleID(cat);
    return getCodepage(lcid);
}

static char* getEncoding(LCID lcid) {
    int codepage = 0;
    char* ret = (char*) malloc(16);
    if (ret == NULL) {
        return NULL;
    }

    if (lcid == 0) {
        codepage = GetACP();
        _itoa_s(codepage, ret + 2, 14, 10);
    } else if (GetLocaleInfo(lcid, LOCALE_IDEFAULTANSICODEPAGE, ret + 2, 14) != 0) {
        codepage = atoi(ret + 2);
    }

    switch (codepage) {
    case 0:
    case 65001:
        strcpy(ret, "UTF-8");
        break;
    case 874:     /*  9:Thai     */
    case 932:     /* 10:Japanese */
    case 949:     /* 12:Korean Extended Wansung */
    case 950:     /* 13:Chinese (Taiwan, Hongkong, Macau) */
    case 1361:    /* 15:Korean Johab */
        ret[0] = 'M';
        ret[1] = 'S';
        break;
    case 936:
        strcpy(ret, "GBK");
        break;
    case 54936:
        strcpy(ret, "GB18030");
        break;
    default:
        ret[0] = 'C';
        ret[1] = 'p';
        break;
    }
 
    return ret;
}

static locale_t* loadLocale(LCID lcid) {

    /* script */
    char tmp[SNAMESIZE];
    char* script = (char*) malloc(PROPSIZE);
    if (script == NULL) {
        return NULL;
    }

    #ifdef LOCALE_SNAME
    if (GetLocaleInfo(lcid,
                      LOCALE_SNAME, tmp, SNAMESIZE) == 0 ||
        sscanf(tmp, "%*[a-z\\-]%1[A-Z]%[a-z]", script, &((script)[1])) == 0 ||
        strlen(script) != 4) {
        script[0] = '\0';
    }
    #else
    script[0] = '\0';
    #endif

    /* country */
    char* country = (char*) malloc(PROPSIZE);
    if (country == NULL) {
        return NULL;
    }
    if (GetLocaleInfo(lcid, LOCALE_SISO3166CTRYNAME, country, PROPSIZE) == 0) {
        #ifdef LOCALE_SNAME
        if (GetLocaleInfo(lcid, LOCALE_SISO3166CTRYNAME2, country, PROPSIZE) == 0) {
          country[0] = '\0';
        }
        #else
        country[0] = '\0';
        #endif                
    }

    /* language */
    char* language = (char*) malloc(PROPSIZE);
    if (language == NULL) {
        return NULL;
    }
    if (GetLocaleInfo(lcid, LOCALE_SISO639LANGNAME, language, PROPSIZE) == 0) {

        #ifdef LOCALE_SISO639LANGNAME2
        if (GetLocaleInfo(lcid, LOCALE_SISO639LANGNAME2, language, PROPSIZE) == 0) {
            /* defaults to en_US */
            strcpy(language, "en");
            strcpy(country, "US");
        }
        #else
        /* defaults to en_US */
        strcpy(language, "en");
        strcpy(country, "US");
        #endif
    }

    /* variant */
    char* variant = (char*) malloc(PROPSIZE);
    if (variant == NULL) {
        return NULL;
    }
    (variant)[0] = '\0';

    /* handling for Norwegian */
    if (strcmp(language, "nb") == 0) {
        strcpy(language, "no");
        strcpy(country , "NO");
    } else if (strcmp(language, "nn") == 0) {
        strcpy(language, "no");
        strcpy(country , "NO");
        strcpy(variant, "NY");
    }

    /* encoding */
    char* encoding = getEncoding(lcid);

    locale_t* locale = (locale_t*) malloc(sizeof(locale_t));
    locale->language = language;
    locale->script = script;
    locale->country = country;
    locale->variant = variant;
    locale->encoding = encoding;

    char* le = encoding;
    if (le) {
       if (  (le[0] == 'M' && le[1] == 'S') 
          || (le[0] == 'C' && le[1] == 'P') 
          || (le[0] == 'C' && le[1] == 'p')) {
         le += 2;
       }
    }

    locale->name = get_locale_name(language, country, le);
 
    return locale;
}

static locale_t* loadLocaleWin(int cat) {

   if (cat == LC_CTYPE) {
      return loadLocale(GetUserDefaultLCID()); // LC_CTYPE
   }

   // query the system for the current system default locale
   // (which is a Windows LCID value),
   LCID userDefaultLCID = GetUserDefaultLCID();
   LANGID userDefaultUILang = GetUserDefaultUILanguage();
   LCID userDefaultUILCID = MAKELCID(userDefaultUILang, SORTIDFROMLCID(userDefaultLCID));

   //char * display_encoding;
   //HANDLE hStdOutErr;

    // Windows UI Language selection list only cares "language"
    // information of the UI Language. For example, the list
    // just lists "English" but it actually means "en_US", and
    // the user cannot select "en_GB" (if exists) in the list.
    // So, this hack is to use the user LCID region information
    // for the UI Language, if the "language" portion of those
    // two locales are the same.
    if (PRIMARYLANGID(LANGIDFROMLCID(userDefaultLCID)) == PRIMARYLANGID(userDefaultUILang)) {
      userDefaultUILCID = userDefaultLCID;
    }

    return loadLocale(userDefaultUILCID); // LC_MESSAGES = LC_TIME + 1
}

/*
static void loadLocaleAllWin(SysInfo& sysInfo) {
      
   // query the system for the current system default locale
   // (which is a Windows LCID value),
   LCID userDefaultLCID = GetUserDefaultLCID();
   LANGID userDefaultUILang = GetUserDefaultUILanguage();
   LCID userDefaultUILCID = MAKELCID(userDefaultUILang, SORTIDFROMLCID(userDefaultLCID));

   //char * display_encoding;
   //HANDLE hStdOutErr;

    // Windows UI Language selection list only cares "language"
    // information of the UI Language. For example, the list
    // just lists "English" but it actually means "en_US", and
    // the user cannot select "en_GB" (if exists) in the list.
    // So, this hack is to use the user LCID region information
    // for the UI Language, if the "language" portion of those
    // two locales are the same.
    if (PRIMARYLANGID(LANGIDFROMLCID(userDefaultLCID)) == PRIMARYLANGID(userDefaultUILang)) {
      userDefaultUILCID = userDefaultLCID;
    }

    locale_t* formatLocale = loadLocale(userDefaultLCID);    // LC_CTYPE
    locale_t* displayLocale = loadLocale(userDefaultUILCID); // LC_MESSAGES = LC_TIME + 1

    initLocale(sysInfo, LC_CTYPE, formatLocale);
    initLocale(sysInfo, LC_TIME + 1, displayLocale);

}
*/

locale_t* load_locale_os(int cat) {
    //return parse_locale(get_locale(cat));
    return loadLocaleWin(cat);
}

#endif

#endif // PLAZMA_LIB_SYSLOCALE_WIN_H
