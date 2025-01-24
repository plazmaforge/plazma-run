#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "wstrlib.h"

/**
 * [allocate]
 */
static char* lib_strnew(size_t size) {
  size_t _size = size * sizeof(char) + 1;
  char* str = (char*) malloc(_size);
  if (!str) {
    return NULL;
  }
  memset(str, 0, _size);
  return str;
}

/**
 * [allocate]
 */
static wchar_t* _lib_wstrndup(const wchar_t* src, size_t num) {
  wchar_t* dst = lib_wstrnew(num);
  if (!dst) {
    return NULL;
  }    
  memcpy(dst, src, num);
  //dst[num] = '\0';
  return dst;
}

// wstrnew

/**
 * [allocate]
 */
wchar_t* lib_wstrnew(size_t size) {
  size_t _size = size * sizeof(wchar_t) + 1;
  wchar_t* str = (wchar_t*) malloc(_size);
  if (!str) {
    return NULL;
  }
  memset(str, 0, _size);
  return str;

}

// wstrdup

/**
 * [allocate]
 */
wchar_t* lib_wstrdup(const wchar_t* src) {
  if (!src) {
    return NULL;
  }
  size_t len = wcslen(src);
  return _lib_wstrndup(src, len);
}

/**
 * [allocate]
 */
wchar_t* lib_wstrndup(const wchar_t* src, size_t num) {
  if (!src) {
    return NULL;
  }
  size_t len = wcslen(src);
  len = len < num ? len : num;
  return _lib_wstrndup(src, len);
}

// wstrlen

size_t lib_wstrlen(const wchar_t* str) {
  if (!str) {
    return 0;
  }
  return wcslen(str);
}

size_t lib_wstrnlen(const wchar_t* str, size_t num) {
  if (!str) {
    return 0;
  }
  return wcsnlen(str, num);
}

//

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
