
#include <stdio.h>
#include <stdlib.h>

// #include "getopt.h"
// #include "strlib.h"
// #include "wstrlib.h"

#include "sysinfo.h"

void printSysInfo() {    
    print_sys_info();
}

void printUsage() {
    printf("Usage: run-sysinfo\n");
}

int main(int argc, char* argv[]) {
    printSysInfo();    
    return 0;
}