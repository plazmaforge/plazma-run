#ifndef PLAZMA_LIB_SYSLOCALE_NIX_H
#define PLAZMA_LIB_SYSLOCALE_NIX_H

#include "syslocale_os.h"
#if defined __APPLE__ && defined __MACH__
#include "syslocale_mac.h"
#else
locale_t* load_locale_os(int cat) {
    return parse_locale(get_locale(cat)); // LC Locale
}
#endif

void init_locale_nix() {
    setlocale(LC_ALL, ""); // set default locale
    //set_default_locale();

    if (debug) {
       printf("\n");
       printf("Change          :\n");
       printf("----------------:\n");
       printf("All LC Locale   : %s\n", get_locale(LC_ALL));
       printf("Std LC Locale   : %s\n", get_locale(LC_CTYPE));
       _locale_os = load_locale_os();
       printf("Std OS Locale   : %s\n", _locale_os ? lib_strsaf(_locale_os->name) : "");
       print_locale(_locale_os);

       // We can't use 'setlocale(LC_CTYPE, _locale_os->name)'
       // because this combination 'lang_COUNTRY.encoding' is not found

       free(_locale_os);
       _locale_os = NULL;

    }
}

void restore_locale_nix() {
    setlocale(LC_ALL, _locale); // restore locale
    if (debug) {
       printf("\n");
       printf("Restore         :\n");
       printf("----------------:\n");
       printf("All LC Locale   : %s\n", get_locale(LC_ALL));
    }
    free(_locale);
    _locale = NULL;
}

#endif // PLAZMA_LIB_SYSLOCALE_NIX_H