#ifndef PLAZMA_LIB_SYSLOCALE_OS_H
#define PLAZMA_LIB_SYSLOCALE_OS_H

#include "syslocale.h"

/*
 * OS locale.
 * It depends on Operation System (Windows, MacOS)
 */

/*
 * Load OS locale by category
 */
locale_t* load_locale_os(int cat);

/*
 * Load OS locale
 */
locale_t* load_locale_os();

#endif // PLAZMA_LIB_SYSLOCALE_OS_H
