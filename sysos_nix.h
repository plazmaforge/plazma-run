#ifndef PLAZMA_LIB_SYSOS_NIX_H
#define PLAZMA_LIB_SYSOS_NIX_H

//#if defined __unix__
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>        /* For os_name and os_version */

#include "sysos.h"

#if defined __APPLE__ && defined __MACH__
#include "sysos_mac.h"
#else
os_info_t* get_os_info() {

   struct utsname name;
   uname(&name);

    os_info_t* os_info = new_os_info();
    os_info->os_name = strdup(name.sysname);
    #ifdef _AIX
        char os_version[strlen(name.version) + strlen(name.release) + 2];
        strcpy(os_version, name.version);
        strcat(os_version, ".");
        strcat(os_version, name.release);
        os_info->os_version = os_version;
        //os_info->os_major_version = atoi(name.version);
        //os_info->os_minor_version = atoi(name.release);
    #else
        os_info->os_version = strdup(name.release); // 'name.version' is full version string
        // TODO: split os_version by '.' and set major/minor/build
    #endif
    os_info->os_arch = strdup(name.machine);
    os_info->os_arch_data = strdup(get_os_arch_data(os_info->os_arch));

    return os_info;
}
#endif

//#endif

#endif // PLAZMA_LIB_SYSOS_NIX_H