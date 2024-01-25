
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

// #include "getopt.h"
 #include "strlib.h"
// #include "wstrlib.h"

#include "sysinfo.h"
#include "syslocale_os.h"

void printSysInfo() {    
    print_sys_info();
}

void printUsage() {
    printf("Usage: run-sysinfo\n");
}

int main(int argc, char* argv[]) {

    init_locale_os();

    printSysInfo();

    restore_locale_os();

    return 0;
}