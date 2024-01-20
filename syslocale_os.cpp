#include <locale.h>

#include "strlib.h"
#include "syslocale_os.h"

#if defined _WIN32
#include "syslocale_win.h"
#elif defined __APPLE__ && defined __MACH__
#include "syslocale_mac.h"
#else
locale_t* load_locale_os(int cat) {
    return parse_locale(get_locale(cat)); // LC Locale
}
#endif

locale_t* load_locale_os() {
    return load_locale_os(LC_CTYPE);
}

