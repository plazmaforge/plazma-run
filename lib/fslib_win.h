#ifndef PLAZMA_LIB_FSLIB_WIN_H
#define PLAZMA_LIB_FSLIB_WIN_H

#ifdef _WIN32

#include <windows.h>
#include <errno.h>
#include <wchar.h>
#include <io.h>

//#include <stdio.h>
//#include <stdlib.h>

//#include <fileapi.h>
//#include <aclapi.h>

//#include <winioctl.h>
//#include <direct.h>
//#include <shlobj.h>
//#include <shlwapi.h>

#include "strlib.h"
#include "wstrlib.h"
#include "fslib.h"

static int lib_fs_match_file_internal(const char* pattern, const char* name);

static int lib_fs_match_file_internal_mode(const char* pattern, const char* name, int mode);

static wchar_t* getUserNameW();

static int lib_fs_match_file_internal(const char* pattern, const char* name) {
    return lib_fs_match_file_internal_mode(pattern, name, 0);
}

static int lib_fs_match_file_internal_mode(const char* pattern, const char* name, int mode) {

    // PathMatchSpecA
    //printf(" %s -> %s, %d, %d\n", pattern, name, val, res);

    //wchar_t* wpattern = lib_mbs_to_wcs(pattern, strlen(pattern));
    //wchar_t* wname = lib_mbs_to_wcs(name, strlen(name));

    //return PathMatchSpecW(wname, wpattern);
    //return PathMatchSpecA(name, pattern);

    return lib_fs_match_file(name, pattern); // rotate pattern, name !
}

//// uname, gname

// [allocate]
static wchar_t* getUserNameW() {
   WCHAR *uname = _wgetenv(L"USERNAME");
   if (uname != NULL && wcslen(uname) > 0) {
      return _wcsdup(uname);
   } 
   
   DWORD buflen = 0;
   if (GetUserNameW(NULL, &buflen) == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
      uname = (WCHAR*) malloc(buflen * sizeof(WCHAR));
      if (uname != NULL && GetUserNameW(uname, &buflen) == 0) {
        free(uname);
        uname = NULL;
      }
   } else {
        uname = NULL;
   }

   if (uname != NULL) {
      return _wcsdup(uname);
   }

   return NULL;  
}

// [allocate]
static char* getUserName() {
    wchar_t* wusername = getUserNameW();
    if (wusername == NULL) {
        return NULL;
    }
    char* username = lib_wcs_to_mbs(wusername);
    if (username == NULL) {
        return NULL;
    }
    free(wusername);
    return username;
}

////

static char* _uname = NULL;
static char* _gname = NULL;

// fs-file

char lib_file_get_amode_by_path(const char* path) {
    // TODO: Stub
    return ' ';
}

char* lib_file_get_uname(lib_file_t* file) {
    if (_uname) {
        return _uname;
    }
    _uname = getUserName();
    if (!_uname) {
        _uname = lib_strdup("");
    }
    return _uname;
}

char* lib_file_get_gname(lib_file_t* file) {    
    if (_gname) {
        return _gname;
    }
    _gname = "197121"; // TODO: Stub
    //if (!_gname) {
    //    _gname = lib_strdup("");
    //}
    return _gname;
}

#endif

// https://github.com/Quintus/pathie-cpp/blob/master/src/path.cpp

#endif // PLAZMA_LIB_FSLIB_WIN_H