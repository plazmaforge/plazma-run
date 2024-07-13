#ifndef PLAZMA_LIB_SYSLOCALE_OS_H
#define PLAZMA_LIB_SYSLOCALE_OS_H

#include "syslocale.h"

static char* _locale;
static locale_t* _locale_os;

static int debug = 0;
static int check = 0;

/*
 * OS locale.
 * It depends on Operation System (Windows, MacOS)
 */

/*
 * Load OS locale by category
 */
locale_t* lib_locale_os_load_cat(int cat);

/*
 * Load OS locale
 */
locale_t* lib_locale_os_load();

/*
 * Initialize OS locale
 */

void lib_locale_os_init();

/*
 * Restore OS locale
 */
void lib_locale_os_restore();


#endif // PLAZMA_LIB_SYSLOCALE_OS_H
