#ifndef PLAZMA_LIB_SYSOS_MAC_H
#define PLAZMA_LIB_SYSOS_MAC_H

#if defined __APPLE__ && defined __MACH__

//#define PLAZMA_LIB_SYSOS_CORE

#include <CoreServices/CoreServices.h> // Gestalt: os_version
#include <sys/utsname.h>               // For os_name and os_version

#include "sysos.h"

void load_os_arch(os_info_t* os_info) {
   struct utsname name;
   uname(&name);
   os_info->os_arch = strdup(name.machine);
   os_info->os_arch_data = strdup(get_os_arch_data(os_info->os_arch));
}

#ifdef PLAZMA_LIB_SYSOS_CORE
void load_os_info_core(os_info_t* os_info) {
    // Gestalt is deprecated in macOS 10.8 - Use NSProcessInfo's (*.mm)

    SInt32 majorVersion = 0;
    SInt32 minorVersion = 0;
    SInt32 bugFixVersion = 0;

    Gestalt(gestaltSystemVersionMajor, &majorVersion);
    Gestalt(gestaltSystemVersionMinor, &minorVersion);
    Gestalt(gestaltSystemVersionBugFix, &bugFixVersion);

    char version[100];
    sprintf(version, "%d.%d.%d", majorVersion, minorVersion, bugFixVersion);

    os_info->os_name = strdup("Mac OS X");
    os_info->os_version = strdup(version);

    os_info->os_major_version = majorVersion;
    os_info->os_minor_version = minorVersion;
    os_info->os_build_version = bugFixVersion;

}

void load_os_info(os_info_t* os_info) {
    load_os_info_core(os_info);
    load_os_arch(os_info);
}

#else

void load_os_info_objc(os_info_t* os_info); // Implemented in *.mm file

void load_os_info(os_info_t* os_info) {
    load_os_info_objc(os_info);
    load_os_arch(os_info);
}

#endif // PLAZMA_LIB_SYSOS_CORE

#endif

#endif // PLAZMA_LIB_SYSOS_MAC_H