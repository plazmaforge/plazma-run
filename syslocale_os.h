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
locale_t* load_locale_os(int cat);

/*
 * Load OS locale
 */
locale_t* load_locale_os();

/*
 * Initialize OS locale
 */

void init_locale_os();

/*
 * Reset OS locale
 */
void reset_locale_os();


#endif // PLAZMA_LIB_SYSLOCALE_OS_H
