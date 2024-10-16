
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

// #include "getopt.h"
 #include "strlib.h"
// #include "wstrlib.h"

#include "sysinfo.h"
#include "syslocale_os.h"

void usage() {
    printf("Usage: run-sysinfo\n");
}

int main(int argc, char* argv[]) {

    lib_sys_locale_os_init();

    lib_sys_print_sys_info();

    lib_sys_locale_os_restore();

    return 0;
}