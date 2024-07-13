#include <locale.h>

#include "strlib.h"
#include "syslocale_os.h"

#if defined _WIN32
#include "syslocale_win.h"
#else
#include "syslocale_nix.h"
#endif

locale_t* lib_locale_os_load() {
    return lib_locale_os_load_cat(LC_CTYPE);
}

void lib_locale_os_init() {

    #if defined __APPLE__ && defined __MACH__
    // Bug with empty locale for MacOS
    // const char* name = getLocaleValue(cflocale, kCFLocaleIdentifier);
    // name is NULL. Why?
    locale_t* locale = lib_locale_os_load(); // TODO: Force load locale (Why?)
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
      printf("All LC Locale   : %s\n", lib_strsaf(get_locale(LC_ALL)));
      printf("Std LC Locale   : %s\n", lib_strsaf(get_locale(LC_CTYPE)));
    }

    _locale = lib_strdup(get_locale(LC_ALL)); // get current locale LC_ALL

    if (debug) {
      _locale_os = lib_locale_os_load();
      printf("Std OS Locale   : %s\n", _locale_os ? lib_strsaf(_locale_os->name) : "");
      print_locale(_locale_os);
      free(_locale_os);
      _locale_os = NULL;
    }
    
    if (debug && check) {
      setlocale(LC_ALL, "");         // SET

      printf("\n");
      printf("Default         :\n");
      printf("----------------:\n");
      printf("All LC Locale   : %s\n", lib_strsaf(get_locale(LC_ALL)));
      printf("Std LC Locale   : %s\n", lib_strsaf(get_locale(LC_CTYPE)));
      _locale_os = lib_locale_os_load();
      printf("Std OS Locale   : %s\n", _locale_os ? lib_strsaf(_locale_os->name) : "");
      print_locale(_locale_os);
      free(_locale_os);
      _locale_os = NULL;

      setlocale(LC_ALL, _locale);   // RESTORE
    }
   
    #ifdef _WIN32
    init_locale_win();
    #else
    init_locale_nix();
    #endif
}

void lib_locale_os_restore() {
    #ifdef _WIN32
    restore_locale_win();
    #else
    restore_locale_nix();
    #endif
}

