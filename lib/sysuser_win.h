#ifndef PLAZMA_LIB_SYSUSER_WIN_H
#define PLAZMA_LIB_SYSUSER_WIN_H

#if defined _WIN32

// STD
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

// WIN
#include <io.h>
#include <windows.h>
#include <shlobj.h>
#include <objidl.h>

// LIB
#include "wstrlib.h"      // convert char/wchar, string/wstring

/*
 * User name
 * We try to avoid calling GetUserName as it turns out to
 * be surprisingly expensive on NT.  It pulls in an extra
 * 100 K of footprint.
 */
wchar_t* getUserNameW() {
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

wchar_t* getUserHomeW() {
    /*
     * Note that we don't free the memory allocated
     * by getHomeFromShell32.
     */
    static WCHAR *u_path = NULL;
    if (u_path == NULL) {
        HRESULT hr;

        /*
         * SHELL32 DLL is delay load DLL and we can use the trick with
         * __try/__except block.
         */

        
        //__try {
        //    /*
        //     * For Windows Vista and later (or patched MS OS) we need to use
        //     * [SHGetKnownFolderPath] call to avoid MAX_PATH length limitation.
        //     * Shell32.dll (version 6.0.6000 or later)
        //     */
        //    hr = SHGetKnownFolderPath(&FOLDERID_Profile, KF_FLAG_DONT_VERIFY, NULL, &u_path);
        //} __except(EXCEPTION_EXECUTE_HANDLER) {
        //    /* Exception: no [SHGetKnownFolderPath] entry */
        //    hr = E_FAIL;
        //}
        
        hr = E_FAIL; // TODO

        if (FAILED(hr)) {
            WCHAR path[MAX_PATH+1];

            /* fallback solution for WinXP and Windows 2000 */
            hr = SHGetFolderPathW(NULL, CSIDL_FLAG_DONT_VERIFY | CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, path);
            if (FAILED(hr)) {
                /* we can't find the shell folder. */
                u_path = NULL;
            } else {
                /* Just to be sure about the path length until Windows Vista approach.
                 * [S_FALSE] could not be returned due to [CSIDL_FLAG_DONT_VERIFY] flag and UNICODE version.
                 */
                path[MAX_PATH] = 0;
                u_path = _wcsdup(path);
            }
        }
    }

    // if (u_path == NULL) {
    //  u_path = _wcsdup(L"C:\\");
    // }

    return u_path;
}

wchar_t* getUserDirW() {
    /* Current directory */
    WCHAR buf[MAX_PATH];
    if (GetCurrentDirectoryW(sizeof(buf)/sizeof(WCHAR), buf) != 0) {
      return _wcsdup(buf);
    }
    return NULL;
}

wchar_t* getTmpDirW() {
  /* tmp dir */
  WCHAR tmpdir[MAX_PATH + 1];
  GetTempPathW(MAX_PATH + 1, tmpdir);
  return _wcsdup(tmpdir);
}

////

char* getUserName() {
    wchar_t* wusername = getUserNameW();
    if (wusername == NULL) {
        return NULL;
    }
    char* username = wchar_char(wusername);
    if (username == NULL) {
        return NULL;
    }
    free(wusername);
    return username;
}

char* getUserHome() {
    wchar_t* wuserhome = getUserHomeW();
    if (wuserhome == NULL) {
        return NULL;
    }
    char* userhome = wchar_char(wuserhome);
    if (userhome == NULL) {
        return NULL;
    }
    free(wuserhome);
    return userhome;
}

char* getUserDir() {
    wchar_t* wuserdir = getUserDirW();
    if (wuserdir == NULL) {
        return NULL;
    }
    char* userdir = wchar_char(wuserdir);
    if (userdir == NULL) {
        return NULL;
    }
    free(wuserdir);
    return userdir;
}

char* getTmpDir() {
    wchar_t* wtmpdir = getTmpDirW();
    if (wtmpdir == NULL) {
        return NULL;
    }
    char* tmpdir = wchar_char(wtmpdir);
    if (tmpdir == NULL) {
        return NULL;
    }
    free(wtmpdir);
    return tmpdir;
}

////

#endif

#endif // PLAZMA_LIB_SYSUSER_WIN_H