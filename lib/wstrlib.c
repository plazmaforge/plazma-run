#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "strlib.h"
#include "wstrlib.h"

/**
 * [allocate]
 */
wchar_t* lib_wstrnew(size_t size) {
  if (size < 0) {
    return NULL;
  }
  wchar_t* dst = (wchar_t*) malloc(sizeof(wchar_t) * size + 1);
  if (!dst) {
    return NULL;
  }
  for (int i = 0; i <= size; i++) {
    dst[i] = '\0';
  }    
  return dst;
}

#ifdef _WIN32
static wchar_t* lib_mbs_to_wcs_win(UINT cp, const char* str, int len) {
    if (!str) {
        return NULL;
    }
    int wlen = MultiByteToWideChar(cp, 0, str, len, NULL, 0);
    wchar_t* wstr = lib_wstrnew(len);
    MultiByteToWideChar(cp, 0, str, len, wstr, wlen);
    wstr[wlen] = '\0';
    return wstr;
}

static char* lib_wcs_to_mbs_win(UINT cp, const wchar_t* wstr, int wlen) {
    if (!wstr) {
        return NULL;
    }
    int len = WideCharToMultiByte(cp, 0, wstr, wlen, NULL, 0, NULL, NULL);
    char* str = lib_strnew(len);
    WideCharToMultiByte(cp, 0, wstr, -1, str, len, NULL, NULL);
    str[len] = '\0';
    return str;
}
#else
static wchar_t* lib_mbs_to_wcs_nix(const char* str, int len) {
    if (!str) {
        return NULL;
    }
    // Locale dependency! - setlocale(LC_ALL, "");
    int wlen = mbstowcs((wchar_t*) 0, str, len);
    wchar_t* wstr = lib_wstrnew(len);
    wlen = mbstowcs(wstr, str, wlen);
    wstr[wlen] = '\0';
    return wstr;
}

static char* lib_wcs_to_mbs_nix(const wchar_t* wstr, int wlen) {
    if (!wstr) {
        return NULL;
    }
    // Locale dependency! - setlocale(LC_ALL, "");
    int len = wcstombs((char*) 0, wstr, wlen);
    char* str = lib_strnew(len);
    wcstombs(str, wstr, len);
    str[len] = '\0';
    return str;
}
#endif

////

/**
 * [allocate]
 */
wchar_t* lib_acs_to_wcs_n(const char* str, int len) {
    if (!str) {
        return NULL;
    }
    #ifdef _WIN32
    return lib_mbs_to_wcs_win(CP_ACP, str, len);
    #else    
    // Locale dependency! - setlocale(LC_ALL, "");    
    return lib_mbs_to_wcs_nix(str, len);
    #endif
}

/**
 * [allocate]
 */
wchar_t* lib_acs_to_wcs(const char* str) {
    if (!str) {
        return NULL;
    }
    return lib_acs_to_wcs_n(str, strlen(str));
}

/**
 * [allocate]
 */
char* lib_wcs_to_acs_n(const wchar_t* wstr, int wlen) {
    if (!wstr) {
        return NULL;
    }
    #ifdef _WIN32
    return lib_wcs_to_mbs_win(CP_ACP, wstr, wlen);
    #else    
    // Locale dependency! - setlocale(LC_ALL, "");
    return lib_wcs_to_mbs_nix(wstr, wlen);
    #endif
}

/**
 * [allocate]
 */
char* lib_wcs_to_acs(const wchar_t* wstr) {
    if (!wstr) {
        return NULL;
    }
    return lib_wcs_to_acs_n(wstr, wcslen(wstr)); 
}

////

/**
 * [allocate]
 */
wchar_t* lib_mbs_to_wcs_n(const char* str, int len) {
    if (!str) {
        return NULL;
    }
    #ifdef _WIN32
    return lib_mbs_to_wcs_win(CP_UTF8, str, len);
    #else
    // Locale dependency! - setlocale(LC_ALL, "");
    return lib_mbs_to_wcs_nix(str, len);
    #endif
}

/**
 * [allocate]
 */
wchar_t* lib_mbs_to_wcs(const char* str) {
    if (!str) {
        return NULL;
    }
    return lib_mbs_to_wcs_n(str, strlen(str));
}

////

/**
 * [allocate]
 */
char* lib_wcs_to_mbs_n(const wchar_t* wstr, int wlen) {
    if (!wstr) {
        return NULL;
    }
    #ifdef _WIN32
    return lib_wcs_to_mbs_win(CP_UTF8, wstr, wlen);
    #else
    // Locale dependency! - setlocale(LC_ALL, "");    
    return lib_wcs_to_mbs_nix(wstr, wlen);
    #endif
}

/**
 * [allocate]
 */
char* lib_wcs_to_mbs(const wchar_t* wstr) {
    if (!wstr) {
        return NULL;
    }
    return lib_wcs_to_mbs_n(wstr, wcslen(wstr)); 
}
