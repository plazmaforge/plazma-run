#ifndef PLAZMA_LIB_SYSLOCALE_NIX_H
#define PLAZMA_LIB_SYSLOCALE_NIX_H

#include "syslocale_os.h"
#if defined __APPLE__ && defined __MACH__
#include "syslocale_mac.h"
#else
lib_locale_t* lib_sys_load_locale_os(int cat) {
    return lib_sys_parse_locale(lib_sys_get_locale(cat)); // LC Locale
}
#endif

void lib_locale_nix_init() {
    setlocale(LC_ALL, ""); // set default locale
    //set_default_locale();

    if (debug) {
       printf("\n");
       printf("Change          :\n");
       printf("----------------:\n");
       printf("All LC Locale   : %s\n", lib_sys_get_locale(LC_ALL));
       printf("Std LC Locale   : %s\n", lib_sys_get_locale(LC_CTYPE));
       _locale_os = lib_sys_load_current_locale_os();
       printf("Std OS Locale   : %s\n", _locale_os ? lib_strsafe(_locale_os->name) : "");
       lib_sys_print_locale(_locale_os);

       // We can't use 'setlocale(LC_CTYPE, _locale_os->name)'
       // because this combination 'lang_COUNTRY.encoding' is not found

       free(_locale_os);
       _locale_os = NULL;

    }
}

void lib_locale_nix_restore() {
    setlocale(LC_ALL, _locale); // restore locale
    if (debug) {
       printf("\n");
       printf("Restore         :\n");
       printf("----------------:\n");
       printf("All LC Locale   : %s\n", lib_sys_get_locale(LC_ALL));
    }
    free(_locale);
    _locale = NULL;
}

#endif // PLAZMA_LIB_SYSLOCALE_NIX_H