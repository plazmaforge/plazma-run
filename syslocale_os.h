#ifndef PLAZMA_LIB_SYSLOCALE_OS_H
#define PLAZMA_LIB_SYSLOCALE_OS_H

#include "syslocale.h"

/*
 * Load OS style locale by category
 */
locale_t* load_locale_os(int cat);

/*
 * Load OS style locale
 */
locale_t* load_locale_os();

#endif // PLAZMA_LIB_SYSLOCALE_OS_H
