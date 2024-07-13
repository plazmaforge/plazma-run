#ifndef PLAZMA_LIB_SYSLOCALE_NIX_H
#define PLAZMA_LIB_SYSLOCALE_NIX_H

#include "syslocale_os.h"
#if defined __APPLE__ && defined __MACH__
#include "syslocale_mac.h"
#else
locale_t* lib_locale_os_load_locale(int cat) {
    return lib_locale_parse_locale(lib_locale_get_locale(cat)); // LC Locale
}
#endif

void lib_locale_nix_init() {
    setlocale(LC_ALL, ""); // set default locale
    //set_default_locale();

    if (debug) {
       printf("\n");
       printf("Change          :\n");
       printf("----------------:\n");
       printf("All LC Locale   : %s\n", lib_locale_get_locale(LC_ALL));
       printf("Std LC Locale   : %s\n", lib_locale_get_locale(LC_CTYPE));
       _locale_os = lib_locale_os_load_current_locale();
       printf("Std OS Locale   : %s\n", _locale_os ? lib_strsaf(_locale_os->name) : "");
       lib_locale_print_locale(_locale_os);

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
       printf("All LC Locale   : %s\n", lib_locale_get_locale(LC_ALL));
    }
    free(_locale);
    _locale = NULL;
}

#endif // PLAZMA_LIB_SYSLOCALE_NIX_H