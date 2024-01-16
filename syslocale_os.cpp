#include "strlib.h"
#include "syslocale_os.h"

#if defined _WIN32

#include <windows.h>
#include <shlobj.h>
#include <objidl.h>

#elif defined __APPLE__ && defined __MACH__
#include <CoreFoundation/CoreFoundation.h>
//#include <CoreServices/CoreServices.h>
#endif

#if defined _WIN32

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

#elif defined __APPLE__ && defined __MACH__

static const char* getLocaleValue(CFLocaleRef locale, CFLocaleKey key) {
  CFStringRef value = (CFStringRef) CFLocaleGetValue(locale, key); 
  const char* ch = CFStringGetCStringPtr(value, kCFStringEncodingUTF8);
  if (!ch) {
    return NULL;
  }
  return ch;
}

//https://github.com/apple/swift-corelibs-foundation/blob/main/CoreFoundation/StringEncodings.subproj/CFStringEncodingDatabase.c

static const char* getEncodingName(CFStringEncoding encoding) {
    // TODO: UTF only
    switch (encoding) {
        case kCFStringEncodingUTF8: return "UTF-8";
        case kCFStringEncodingUTF16: return "UTF-16";
        case kCFStringEncodingUTF16BE: return "UTF-16BE";
        case kCFStringEncodingUTF16LE: return "UTF-16LE";
        case kCFStringEncodingUTF32: return "UTF-32";
        case kCFStringEncodingUTF32BE: return "UTF-32BE";
        case kCFStringEncodingUTF32LE: return "UTF-32LE";
        //case kCFStringEncodingNonLossyASCII: return "Non-lossy ASCII";
        case kCFStringEncodingUTF7: return "UTF-7";
    }
    //kCFStringEncodingDOSCyrillic;
    return NULL; 
}

static char* getEncoding(CFStringEncoding encoding) {
    const char* name = getEncodingName(encoding);
    return name ? strdup(name) : NULL;
}

static char* getDefaultEncoding() {
    //return  "UTF-8";
    char* locale = get_locale();
    if (!locale) {
        return NULL;
    }
    char* encoding = parse_encoding(locale);
    return encoding;
}

static locale_t* loadLocaleMac() {

  CFLocaleRef cflocale = CFLocaleCopyCurrent();
  const char* name = getLocaleValue(cflocale, kCFLocaleIdentifier);
  if (!name) {
    CFRelease(cflocale);
    locale_t* locale = (locale_t*) malloc(sizeof(locale_t));
    locale->name = NULL;
    locale->language = NULL;
    locale->script = NULL;
    locale->country = NULL;
    locale->variant = NULL;
    locale->encoding = NULL;
    return locale;
  }

  const char* language = getLocaleValue(cflocale, kCFLocaleLanguageCode);
  const char* script = getLocaleValue(cflocale, kCFLocaleScriptCode);
  const char* country = getLocaleValue(cflocale, kCFLocaleCountryCode);  
  const char* variant = getLocaleValue(cflocale, kCFLocaleVariantCode);
  const char* encoding = NULL;

  CFStringEncoding system_encoding = CFStringGetSystemEncoding();
  if (system_encoding == 0) {
    encoding = lib_strdup(getDefaultEncoding());
  } else {
    //const char* encoding_name = getEncoding(system_encoding);
    //encoding = encoding_name ? encoding_name : getDefaultEncoding();
    encoding = getEncoding(system_encoding);
    if (!encoding) {
        encoding = getDefaultEncoding();
    }    
  }

  //CFStringRef identifier = CFLocaleGetIdentifier(cflocale);
  //const char* ch = CFStringGetCStringPtr(identifier, kCFStringEncodingUTF8);

  CFRelease(cflocale);

  locale_t* locale = (locale_t*) malloc(sizeof(locale_t));
  locale->name = encoding ? lib_strnew(name, "." , encoding) : lib_strdup(name);  
  locale->language = lib_strdup(language);
  locale->script = lib_strdup(script);
  locale->country = lib_strdup(country);
  locale->variant = lib_strdup(variant);
  locale->encoding = lib_strdup(encoding);

  return locale;

}

static locale_t* loadLocaleMac(int cat) {
    return loadLocaleMac(); // Category independed. Why?
}

locale_t* load_locale_os(int cat) {
    return loadLocaleMac(cat);
}

#else

locale_t* load_locale_os(int cat) {
    return parse_locale(get_locale(cat)); // LC Locale
}

#endif

locale_t* load_locale_os() {
    return load_locale_os(LC_CTYPE);
}

