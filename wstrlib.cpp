#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "strlib.h"
#include "wstrlib.h"

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
  //dst[size] = '\0';
  return dst;
}

#ifdef _WIN32
wchar_t* mchar2wchar_win(UINT cp, const char* str, int len) {
    if (!str) {
        return NULL;
    }
    int wlen = MultiByteToWideChar(cp, 0, str, len, NULL, 0);
    wchar_t* wstr = lib_wstrnew(len);
    MultiByteToWideChar(cp, 0, str, len, wstr, wlen);
    wstr[wlen] = '\0';
    return wstr;
}

char* wchar2wchar_win(UINT cp, const wchar_t* wstr, int wlen) {
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
wchar_t* mchar2wchar_nix(const char* str, int len) {
    // Locale dependency! - setlocale(LC_ALL, "");
    int wlen = mbstowcs((wchar_t*) 0, str, len);
    wchar_t* wstr = lib_wstrnew(len);
    wlen = mbstowcs(wstr, str, wlen);
    wstr[wlen] = '\0';
    return wstr;
}

char* wchar2mchar_nix(const wchar_t* wstr, int wlen) {
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

wchar_t* achar2wchar(const char* str, int len) {
    if (!str) {
        return NULL;
    }
    #ifdef _WIN32
    return mchar2wchar_win(CP_ACP, str, len);

    //int wlen = MultiByteToWideChar(cp /*CP_ACP*/, 0, str, len, NULL, 0);
    //wchar_t* wstr = (wchar_t*) malloc(sizeof(wchar_t) * wlen + 1);
    //wchar_t* wstr = lib_wstrnew(len);
    //MultiByteToWideChar(cp /*CP_ACP*/, 0, str, len, wstr, wlen);
    //wstr[wlen] = '\0';
    //return wstr;

    #else
    
    // Locale dependency! - setlocale(LC_ALL, "");
    //int wlen = mbstowcs((wchar_t*) 0, str, len);
    //wchar_t* wstr = (wchar_t*) malloc(sizeof(wchar_t) * wlen + 1);
    //wchar_t* wstr = lib_wstrnew(len);
    //wlen = mbstowcs(wstr, str, wlen);
    //wstr[wlen] = '\0';
    //return wstr;

    return mchar2wchar_nix(str, len);
    #endif
}

wchar_t* achar2wchar(const char* str) {
    if (!str) {
        return NULL;
    }
    return achar2wchar(str, strlen(str));
}


char* wchar2achar(const wchar_t* wstr, int wlen) {
    if (!wstr) {
        return NULL;
    }
    #ifdef _WIN32
    return wchar2wchar_win(CP_ACP, wstr, wlen);

    //int len = WideCharToMultiByte(cp /*CP_ACP*/, 0, wstr, wlen, NULL, 0, NULL, NULL);
    //char* str = (char*) malloc(sizeof(char) * len + 1);
    //char* str = lib_strnew(len);
    //WideCharToMultiByte(cp /*CP_ACP*/, 0, wstr, -1, str, len, NULL, NULL);
    //str[len] = '\0';
    //return str;

    #else
    
    // Locale dependency! - setlocale(LC_ALL, "");
    //int len = wcstombs((char*) 0, wstr, wlen);
    //char* str = (char*) malloc(sizeof(char) * len + 1);
    //char* str = lib_strnew(len);
    //wcstombs(str, wstr, len);
    //str[len] = '\0';
    //return str;

    return wchar2mchar_nix(wstr, wlen);
    #endif
}

char* wchar2achar(const wchar_t* wstr) {
    if (!wstr) {
        return NULL;
    }
    return wchar2achar(wstr, wcslen(wstr)); 
}

////

wchar_t* char2wchar(const char* str, int len) {
    if (!str) {
        return NULL;
    }
    #ifdef _WIN32
    return mchar2wchar_win(CP_UTF8, str, len);

    //int wlen = MultiByteToWideChar(CP_UTF8, 0, str, len, NULL, 0);
    //wchar_t* wstr = (wchar_t*) malloc(sizeof(wchar_t) * wlen + 1);
    //wchar_t* wstr = lib_wstrnew(wlen);
    //MultiByteToWideChar(CP_UTF8, 0, str, len, wstr, wlen);
    //wstr[wlen] = '\0';
    //return wstr;

    #else

    // Locale dependency! - setlocale(LC_ALL, "");
    //int wlen = mbstowcs((wchar_t*) 0, str, len);
    //wchar_t* wstr = (wchar_t*) malloc(sizeof(wchar_t) * wlen + 1);
    //wchar_t* wstr = lib_wstrnew(wlen);
    //wlen = mbstowcs(wstr, str, wlen);
    //wstr[wlen] = '\0';
    //return wstr;

    return mchar2wchar_nix(str, len);
    #endif
}

wchar_t* char2wchar(const char* str) {
    if (!str) {
        return NULL;
    }
    return char2wchar(str, strlen(str));
}

char* wchar2char(const wchar_t* wstr, int wlen) {
    if (!wstr) {
        return NULL;
    }
    #ifdef _WIN32
    return wchar2mchar_win(CP_UTF8, str, len);

    //int len = WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, NULL, 0, NULL, NULL);
    //char* str = (char*) malloc(sizeof(char) * len + 1);
    //char* str = lib_strnew(len);
    //WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    //str[len] = '\0';
    //return str;

    #else

    // Locale dependency! - setlocale(LC_ALL, "");
    //int len = wcstombs((char*) 0, wstr, wlen);
    //char* str = (char*) malloc(sizeof(char) * len + 1);
    //char* str = lib_strnew(len);
    //wcstombs(str, wstr, len);
    //str[len] = '\0';
    //return str;

    return wchar2mchar_nix(wstr, wlen);
    #endif
}

char* wchar2char(const wchar_t* wstr) {
    if (!wstr) {
        return NULL;
    }
    return wchar2char(wstr, wcslen(wstr)); 
}
