
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include "syslocale_os.h"

void lib_sys_locale_init() {
    lib_sys_locale_os_init();
}

void lib_sys_locale_restore() {
    lib_sys_locale_os_restore();
}


