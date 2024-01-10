#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "wstrlib.h"

////

// std::string ConvertWideToAnsi(const std::wstring &wstr) {
//     int count = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length(),
//                                     NULL, 0, NULL, NULL);
//     std::string str(count, 0);
//     WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], count, NULL,
//                         NULL);
//     return str;
// }

// std::wstring ConvertAnsiToWide(const std::string &str) {
//     int count =
//         MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, 0);
//     std::wstring wstr(count, 0);
//     MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), &wstr[0], count);
//     return wstr;
// }

////

wchar_t* achar2wchar(const char* str, int len) {
    if (!str) {
        return NULL;
    }
    #ifdef _WIN32
    int wlen = MultiByteToWideChar(CP_ACP, 0, str, len, NULL, 0);
    wchar_t* wstr = (wchar_t*) malloc(sizeof(wchar_t) * wlen + 1);
    MultiByteToWideChar(CP_ACP, 0, str, len, wstr, wlen);
    wstr[wlen] = '\0';
    return wstr;
    #else
    // Locale dependency! - setlocale(LC_ALL, "");
    int wlen = mbstowcs((wchar_t*) 0, str, len);
    wchar_t* wstr = (wchar_t*) malloc(sizeof(wchar_t) * wlen + 1);
    wlen = mbstowcs(wstr, str, wlen);
    wstr[wlen] = '\0';
    return wstr;
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
    int len = WideCharToMultiByte(CP_ACP, 0, wstr, wlen, NULL, 0, NULL, NULL);
    char* str = (char*) malloc(sizeof(char) * len + 1);
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
    str[len] = '\0';
    return str;
    #else
    // Locale dependency! - setlocale(LC_ALL, "");
    int len = wcstombs((char*) 0, wstr, wlen);
    char* str = (char*) malloc(sizeof(char) * len + 1);
    wcstombs(str, wstr, len);
    str[len] = '\0';
    return str;
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
    int wlen = MultiByteToWideChar(CP_UTF8, 0, str, len, NULL, 0);
    wchar_t* wstr = (wchar_t*) malloc(sizeof(wchar_t) * wlen + 1);
    MultiByteToWideChar(CP_UTF8, 0, str, len, wstr, wlen);
    wstr[wlen] = '\0';
    return wstr;
    #else
    // Locale dependency! - setlocale(LC_ALL, "");
    int wlen = mbstowcs((wchar_t*) 0, str, len);
    wchar_t* wstr = (wchar_t*) malloc(sizeof(wchar_t) * wlen + 1);
    wlen = mbstowcs(wstr, str, wlen);
    wstr[wlen] = '\0';
    return wstr;
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
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, NULL, 0, NULL, NULL);
    char* str = (char*) malloc(sizeof(char) * len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    str[len] = '\0';
    return str;
    #else
    // Locale dependency! - setlocale(LC_ALL, "");
    int len = wcstombs((char*) 0, wstr, wlen);
    char* str = (char*) malloc(sizeof(char) * len + 1);
    wcstombs(str, wstr, len);
    str[len] = '\0';
    return str;
    #endif
}

char* wchar2char(const wchar_t* wstr) {
    if (!wstr) {
        return NULL;
    }
    return wchar2char(wstr, wcslen(wstr)); 
}
