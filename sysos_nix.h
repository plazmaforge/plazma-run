#ifndef PLAZMA_LIB_SYSOS_NIX_H
#define PLAZMA_LIB_SYSOS_NIX_H

//#if defined __unix__
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>        /* For os_name and os_version */

#include "sysos.h"

os_info_t* get_os_info() {

   struct utsname name;
   uname(&name);

    os_info_t* os_info = (os_info_t*) malloc(sizeof(os_info_t));
    os_info->name = NULL;
    os_info->nodename = NULL;
    os_info->release = NULL;
    os_info->version = NULL;
    os_info->machine = NULL;

    os_info->name = strdup(name.sysname);

    #ifdef _AIX
        char os_version[strlen(name.version) + strlen(name.release) + 2];
        strcpy(os_version, name.version);
        strcat(os_version, ".");
        strcat(os_version, name.release);
        os_info->version = os_version;
    #else
        os_info->version = strdup(name.release); // 'name.version' is full version string
    #endif

    return os_info;
}
//#endif

#endif // PLAZMA_LIB_SYSOS_NIX_H