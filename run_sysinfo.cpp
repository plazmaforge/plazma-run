
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

    setlocale(LC_ALL, ""); // set default locale    
    load_locale_os();      // TODO: Force load locale (?)
    //init_locale_os();

    printSysInfo();    
    return 0;
}