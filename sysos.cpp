#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sysos.h"

#if defined _WIN32
#include "sysos_win.h"
#elif defined __APPLE__ && defined __MACH__
#include "sysos_mac.h"
#else
#include "sysos_nix.h"
#endif

int is_cpu_big_endian() {
    unsigned int endianTest = 0xff000000;
    return ((char*) (&endianTest))[0] != 0;
}

const char* get_cpu_endian() {
    unsigned int endianTest = 0xff000000;
    return is_cpu_big_endian() ? "big" : "little";
}