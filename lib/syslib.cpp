
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include "syslocale_os.h"

void init_locale() {
    init_locale_os();
}

void restore_locale() {
    restore_locale_os();
}


