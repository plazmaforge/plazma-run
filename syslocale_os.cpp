#include "strlib.h"
#include "syslocale_os.h"

#if defined _WIN32

#elif defined __APPLE__ && defined __MACH__
#include <CoreFoundation/CoreFoundation.h>
//#include <CoreServices/CoreServices.h>
#endif

#if defined _WIN32

locale_t* load_locale_os(int cat) {
    return parse_locale(get_locale(cat));
    //return loadLocaleWin(cat);
}

#elif defined __APPLE__ && defined __MACH__

const char* getLocaleValue(CFLocaleRef locale, CFLocaleKey key) {
  CFStringRef value = (CFStringRef) CFLocaleGetValue(locale, key); 
  const char* ch = CFStringGetCStringPtr(value, kCFStringEncodingUTF8);
  if (!ch) {
    return NULL;
  }
  return ch;
}

//https://github.com/apple/swift-corelibs-foundation/blob/main/CoreFoundation/StringEncodings.subproj/CFStringEncodingDatabase.c

const char* getEncodingName(CFStringEncoding encoding) {
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

char* getDefaultEncoding() {
    //return  "UTF-8";
    char* locale = get_locale();
    if (!locale) {
        return NULL;
    }
    char* encoding = parse_encoding(locale);
    return encoding;
}

locale_t* loadLocaleMac() {

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
    const char* encoding_name = getEncodingName(system_encoding);
    encoding = lib_strdup(encoding_name ? encoding_name : getDefaultEncoding());     
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

locale_t* loadLocaleMac(int cat) {
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

