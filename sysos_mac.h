#ifndef PLAZMA_LIB_SYSOS_MAC_H
#define PLAZMA_LIB_SYSOS_MAC_H

#if defined __APPLE__ && defined __MACH__
//#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#include "sysos.h"

os_info_t* get_os_info() {

    // Gestalt is deprecated in macOS 10.8 - Use NSProcessInfo's (*.mm)

    SInt32 majorVersion = 0;
    SInt32 minorVersion = 0;
    SInt32 bugFixVersion = 0;

    Gestalt(gestaltSystemVersionMajor, &majorVersion);
    Gestalt(gestaltSystemVersionMinor, &minorVersion);
    Gestalt(gestaltSystemVersionBugFix, &bugFixVersion);

    os_info_t* os_info = (os_info_t*) malloc(sizeof(os_info_t));
    os_info->name = NULL;
    os_info->nodename = NULL;
    os_info->release = NULL;
    os_info->version = NULL;
    os_info->machine = NULL;

    char version[100];
    sprintf(version, "%d.%d.%d", majorVersion, minorVersion, bugFixVersion);

    os_info->name = strdup("Mac OS X");
    os_info->version = strdup(version);
  
    return os_info;
}

#endif

#endif // PLAZMA_LIB_SYSOS_MAC_H