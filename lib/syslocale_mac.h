#ifndef PLAZMA_LIB_SYSLOCALE_MAC_H
#define PLAZMA_LIB_SYSLOCALE_MAC_H

#if defined __APPLE__ && defined __MACH__

#include <CoreFoundation/CoreFoundation.h>

#include "strlib.h"
#include "syslocale.h"

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
    // Why? But is real case!
    CFRelease(cflocale);
    locale_t* locale = locale_new();
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

  locale_t* locale = locale_new();
  locale->language = lib_strdup(language);
  locale->script = lib_strdup(script);
  locale->country = lib_strdup(country);
  locale->variant = lib_strdup(variant);
  locale->encoding = lib_strdup(encoding);

  //locale->name = encoding ? lib_strnew(name, "." , encoding) : lib_strdup(name);
  locale->name = get_locale_name(language, country, encoding);

  return locale;

}

static locale_t* loadLocaleMac(int cat) {
    return loadLocaleMac(); // Category independed. Why?
}

locale_t* load_locale_os(int cat) {
    return loadLocaleMac(cat);
}

#endif

#endif // PLAZMA_LIB_SYSLOCALE_MAC_H