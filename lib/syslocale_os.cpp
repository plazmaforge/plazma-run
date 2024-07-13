#include <locale.h>

#include "strlib.h"
#include "syslocale_os.h"

#if defined _WIN32
#include "syslocale_win.h"
#else
#include "syslocale_nix.h"
#endif

locale_t* lib_locale_os_load_current_locale() {
    return lib_locale_os_load_locale(LC_CTYPE);
}

void lib_locale_os_init() {

    #if defined __APPLE__ && defined __MACH__
    // Bug with empty locale for MacOS
    // const char* name = getLocaleValue(cflocale, kCFLocaleIdentifier);
    // name is NULL. Why?
    locale_t* locale = lib_locale_os_load_current_locale(); // TODO: Force load locale (Why?)
    lib_locale_free(locale);     
    #endif

    // debug = 1;
    // check = 1;

    _locale = NULL;
    _locale_os = NULL;

    if (debug) {
      printf("\n");
      printf("Current         :\n");
      printf("----------------:\n");
      printf("All LC Locale   : %s\n", lib_strsaf(lib_locale_get_locale(LC_ALL)));
      printf("Std LC Locale   : %s\n", lib_strsaf(lib_locale_get_locale(LC_CTYPE)));
    }

    _locale = lib_strdup(lib_locale_get_locale(LC_ALL)); // get current locale LC_ALL

    if (debug) {
      _locale_os = lib_locale_os_load_current_locale();
      printf("Std OS Locale   : %s\n", _locale_os ? lib_strsaf(_locale_os->name) : "");
      lib_locale_print_locale(_locale_os);
      free(_locale_os);
      _locale_os = NULL;
    }
    
    if (debug && check) {
      setlocale(LC_ALL, "");         // SET

      printf("\n");
      printf("Default         :\n");
      printf("----------------:\n");
      printf("All LC Locale   : %s\n", lib_strsaf(lib_locale_get_locale(LC_ALL)));
      printf("Std LC Locale   : %s\n", lib_strsaf(lib_locale_get_locale(LC_CTYPE)));
      _locale_os = lib_locale_os_load_current_locale();
      printf("Std OS Locale   : %s\n", _locale_os ? lib_strsaf(_locale_os->name) : "");
      lib_locale_print_locale(_locale_os);
      free(_locale_os);
      _locale_os = NULL;

      setlocale(LC_ALL, _locale);   // RESTORE
    }
   
    #ifdef _WIN32
    lib_locale_win_init();
    #else
    lib_locale_nix_init();
    #endif
}

void lib_locale_os_restore() {
    #ifdef _WIN32
    lib_locale_win_restore();
    #else
    lib_locale_nix_restore();
    #endif
}

