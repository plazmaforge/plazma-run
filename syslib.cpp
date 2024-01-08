
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32

#include <io.h>
#include <windows.h>
#include <shlobj.h>
#include <objidl.h>

#else

#include <unistd.h>
#include <dlfcn.h>
#include <sys/utsname.h>        /* For os_name and os_version */
//#include <langinfo.h>         /* For nl_langinfo */
#include <pwd.h>
#include <limits.h>

#ifdef _ALLBSD_SOURCE
#ifndef P_tmpdir
#include <paths.h>
#define P_tmpdir _PATH_VARTMP
#endif
#endif

#ifndef P_tmpdir
#define P_tmpdir "/var/tmp"
#endif

#endif

#include "syslib.h"

#ifdef _WIN32

// wstrlib
wchar_t* char2wchar(const char* str, int len) {
    int wlen = MultiByteToWideChar(CP_UTF8, 0, str, len, NULL, 0);
    wchar_t* wstr = (wchar_t*) malloc(sizeof(wchar_t) * wlen + 1);
    MultiByteToWideChar(CP_UTF8, 0, str, len, wstr, wlen);
    wstr[wlen] = '\0';
    return wstr;
}

// wstrlib
wchar_t* char2wchar(const char* str) {
    return char2wchar(str, strlen(str));

}

// wstrlib
char* wchar2char(const wchar_t* wstr, int wlen) {
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, NULL, 0, NULL, NULL);
    char* str = (char*) malloc(sizeof(char) * len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    str[len] = '\0';
    return str;
}

// wstrlib
char* wchar2char(const wchar_t* wstr) {
    return wchar2char(wstr, wcslen(wstr)); 
}

////

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
    char* username = wchar2char(wusername);
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
    char* userhome = wchar2char(wuserhome);
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
    char* userdir = wchar2char(wuserdir);
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
    char* tmpdir = wchar2char(wtmpdir);
    if (tmpdir == NULL) {
        return NULL;
    }
    free(wtmpdir);
    return tmpdir;
}

////

#else

char* getUserName() {
  /* User */
  struct passwd *pwent = getpwuid(getuid());
  if (pwent) {
    return strdup(pwent->pw_name);
  } 
  return NULL;
}

char* getUserHome() {
  /* User */
  struct passwd *pwent = getpwuid(getuid());
  if (pwent) {
    return strdup(pwent->pw_dir);
  } 
  return NULL;
}

char* getUserDir() {
  /* Current directory */
  //int PATH_MAX_ = 1024;
  char buf[PATH_MAX];
  //errno = 0;
  if (getcwd(buf, sizeof(buf)) == NULL) {
       fprintf(stderr, "Can't get current working directory.\n");
  } else {
       return strdup(buf);
  }
  return NULL;
}

char* getTmpDir() {
   #if defined __APPLE__ && defined __MACH__
    /* Darwin has a per-user temp dir */
    static char tmp_path[PATH_MAX]; // static - important
    int pathSize = confstr(_CS_DARWIN_USER_TEMP_DIR, tmp_path, PATH_MAX);
    if (pathSize > 0 && pathSize <= PATH_MAX) {
        return tmp_path;
    }
   #else 
     return P_tmpdir;
   #endif
   return NULL;
}

#endif
