
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#ifdef _WIN32

#include <io.h>
#include <windows.h>
#include <shlobj.h>
#include <objidl.h>

#include "wstrlib.h"      // convert char/wchar, string/wstring

#else

#include <unistd.h>       // getuid (user_name, user_home), getcwd (current_dir), macos:confstr (tmp_dir)
//#include <dlfcn.h>
#include <sys/utsname.h>  // os_name, os_version
//#include <langinfo.h>   // nl_langinfo
#include <pwd.h>          // getpwuid (user_name, user_home)
#include <limits.h>       // PATH_MAX

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

#include "strlib.h"
#include "syslocale.h"
#include "syslocale_os.h"
#include "syslib.h"

#ifdef _WIN32


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
  //printf("%p\n", pwent->pw_dir);
  if (pwent) {
    return strdup(pwent->pw_dir);
  } 
  return NULL;
}

char* getUserDir() {
  /* Current directory */
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
    return (char*) P_tmpdir;
   #else 
     return (char*) P_tmpdir;
   #endif
}

#endif


static char* _locale;
static int debug = 1;
static int check = 1;

#ifdef _WIN32
static UINT _cp;
static UINT _out_cp;
#endif

// /**
//  * Print current locale
//  */
// void print_locale() {
//     char* _locale_ = NULL;

//     _locale_ = get_locale(LC_COLLATE);
//     printf("Get LC_COLLATE     : %s\n", _locale_);

//     _locale_ = get_locale(LC_CTYPE);
//     printf("Get LC_CTYPE       : %s\n", _locale_);

//     _locale_ = get_locale(LC_MESSAGES);
//     printf("Get LC_MESSAGES    : %s\n", _locale_);

// }

const char* lib_strsaf(const char* str) {
    return str ? str : "";
}

void init_locale() {

    if (debug) {
      printf("\n");
      printf("All LC Locale   : %s\n", lib_strsaf(get_locale(LC_ALL)));
      printf("Get LC Locale   : %s\n", lib_strsaf(get_locale(LC_CTYPE)));
    }

    _locale = lib_strdup(get_locale(LC_ALL)); // get current locale LC_ALL
    locale_t* _locale_os = NULL;

    if (debug) {
      _locale_os = load_locale_os();
      printf("Get OS Locale   : %s\n", _locale_os ? lib_strsaf(_locale_os->name) : "");
      print_locale(_locale_os);
      free(_locale_os);
      _locale_os = NULL;
    }
    
    if (debug && check) {
      setlocale(LC_ALL, "");         // SET

      printf("\n");
      printf("All LC Locale   : %s\n", lib_strsaf(get_locale(LC_ALL)));
      printf("Def LC Locale   : %s\n", lib_strsaf(get_locale(LC_CTYPE)));
      _locale_os = load_locale_os();
      printf("Get OS Locale   : %s\n", _locale_os ? lib_strsaf(_locale_os->name) : "");
      print_locale(_locale_os);
      free(_locale_os);
      _locale_os = NULL;

      setlocale(LC_ALL, _locale);   // RESTORE
    }
   
    #ifdef _WIN32
    _cp = GetConsoleCP();
    _out_cp = GetConsoleOutputCP();
    if (debug) {
      printf("\nGet ConsoleCP   : %d\n", _cp);
      printf("Get ConsoleOutCP: %d\n", _out_cp);
    }

    if (_out_cp == 65001) {
        // UTF-8
        return;
    }

    UINT _new_cp = 0;
    if (_out_cp == 866) {
       _new_cp = 1251;       
    }

    if (_new_cp > 0) {
       SetConsoleCP(_new_cp);
       SetConsoleOutputCP(_new_cp);
       if (debug) {
         printf("\nSet ConsoleCP   : %d\n", _new_cp);
         printf("Set ConsoleOutCP: %d\n", _new_cp);
       }
    } else {

       setlocale(LC_ALL, ""); // set default locale
       //set_default_locale();

       if (debug) {
         //printf("\nSet LC Locale   : %s\n", get_locale(LC_ALL));
         printf("\n");
         printf("All LC Locale   : %s\n", get_locale(LC_ALL));
         printf("Get LC Locale   : %s\n", get_locale(LC_CTYPE));
         _locale_os = load_locale_os();
         printf("Get OS Locale   : %s\n", _locale_os ? lib_strsaf(_locale_os->name) : "");
         print_locale(_locale_os);

       }
    }

    #else

    setlocale(LC_ALL, ""); // set default locale
    //set_default_locale();

    if (debug) {
       printf("\n");
       printf("All LC Locale   : %s\n", get_locale(LC_ALL));
       printf("Get LC Locale   : %s\n", get_locale(LC_CTYPE));
       _locale_os = load_locale_os();
       printf("Get OS Locale   : %s\n", _locale_os ? lib_strsaf(_locale_os->name) : "");
       print_locale(_locale_os);

       // We can't use 'setlocale(LC_CTYPE, _locale_os->name)'
       // because this combination 'lang_COUNTRY.encoding' is not found

       //if (_locale_os && _locale_os->name) {
       //   setlocale(LC_CTYPE, _locale_os->name);
       //   printf("Set LC Locale   : %s\n", get_locale(LC_CTYPE));
       //}       

       free(_locale_os);
       _locale_os = NULL;

    }
    #endif
}

void reset_locale() {
    #ifdef _WIN32
    if (_out_cp == 65001) {
        // UTF-8
        return;
    }

    SetConsoleCP(_cp);
    SetConsoleOutputCP(_out_cp);
    if (debug) {
       printf("Set ConsoleCP   : %d\n", _cp);
       printf("Set ConsoleOutCP: %d\n", _out_cp);
    }
    #else
    setlocale(LC_ALL, _locale); // reset locale
    if (debug) {
       printf("Set LC Locale   : %s\n", get_locale(LC_ALL));
    }
    free(_locale);
    _locale = NULL;
    #endif
}
