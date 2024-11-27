#ifndef PLAZMA_LIB_SYSLOCALE_OS_H
#define PLAZMA_LIB_SYSLOCALE_OS_H

#include "syslocale.h"

static char* _locale;
static lib_locale_t* _locale_os;
static bool use_utf8 = false;

static int debug = 0;
static int check = 0;

/*
 * OS locale.
 * It depends on Operation System (Windows, MacOS)
 */

/*
 * Load OS locale by category
 */
lib_locale_t* lib_sys_load_locale_os(int cat);

/*
 * Load OS locale
 */
lib_locale_t* lib_sys_load_current_locale_os();

void lib_sys_locale_os_init_utf8();

/*
 * Initialize OS locale
 */

void lib_sys_locale_os_init();

/*
 * Restore OS locale
 */
void lib_sys_locale_os_restore();


#endif // PLAZMA_LIB_SYSLOCALE_OS_H
