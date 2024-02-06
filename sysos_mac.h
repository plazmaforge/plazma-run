#ifndef PLAZMA_LIB_SYSOS_MAC_H
#define PLAZMA_LIB_SYSOS_MAC_H

#if defined __APPLE__ && defined __MACH__

//#define PLAZMA_LIB_SYSOS_CORE

#include <CoreServices/CoreServices.h> // Gestalt: os_version
#include <sys/utsname.h>               // For os_name and os_version

#include "sysos.h"

void load_os_common_info(os_info_t* os_info) {
    load_posix_common_info(os_info);
}

void load_os_cpu_info(os_info_t* os_info) {
    load_posix_cpu_info(os_info);
}

#ifdef PLAZMA_LIB_SYSOS_CORE
void load_os_version_info_core(os_info_t* os_info) {
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
    load_os_version_info_core(os_info);
    load_os_common_info(os_info);
    load_os_cpu_info(os_info);
}

#else

void load_os_version_info_objc(os_info_t* os_info); // Implemented in *.mm file

void load_os_info(os_info_t* os_info) {
    load_os_version_info_objc(os_info);
    load_os_common_info(os_info);
    load_os_cpu_info(os_info);
}

#endif // PLAZMA_LIB_SYSOS_CORE

#endif

#endif // PLAZMA_LIB_SYSOS_MAC_H