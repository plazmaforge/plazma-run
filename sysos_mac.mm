#include <Foundation/Foundation.h>

#include "sysos.h"

// 10.9 SDK does not include the NSOperatingSystemVersion struct.
// For now, create our own
typedef struct {
        NSInteger majorVersion;
        NSInteger minorVersion;
        NSInteger patchVersion;
} OSVerStruct;

os_info_t* get_os_info_objc() {

    os_info_t* os_info = (os_info_t*) malloc(sizeof(os_info_t));
    os_info->name = NULL;
    os_info->nodename = NULL;
    os_info->release = NULL;
    os_info->version = NULL;
    os_info->machine = NULL;

    // Hardcode os_name, and fill in os_version
    os_info->name = strdup("Mac OS X");

    NSString *nsVerStr = NULL;

    char* osVersionCStr = NULL;
    // Mac OS 10.9 includes the [NSProcessInfo operatingSystemVersion] function,
    // but it's not in the 10.9 SDK.  So, call it via NSInvocation.
    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(operatingSystemVersion)]) {
        OSVerStruct osVer;
        NSMethodSignature *sig = [[NSProcessInfo processInfo] methodSignatureForSelector:
                @selector(operatingSystemVersion)];
        NSInvocation *invoke = [NSInvocation invocationWithMethodSignature:sig];
        invoke.selector = @selector(operatingSystemVersion);
        [invoke invokeWithTarget:[NSProcessInfo processInfo]];
        [invoke getReturnValue:&osVer];

        // Copy out the char* if running on version other than 10.16 Mac OS (10.16 == 11.x)
        // or explicitly requesting version compatibility
        if (!((long)osVer.majorVersion == 10 && (long)osVer.minorVersion >= 16) ||
                (getenv("SYSTEM_VERSION_COMPAT") != NULL)) {
            if (osVer.patchVersion == 0) { // Omit trailing ".0"
                nsVerStr = [NSString stringWithFormat:@"%ld.%ld",
                        (long)osVer.majorVersion, (long)osVer.minorVersion];
            } else {
                nsVerStr = [NSString stringWithFormat:@"%ld.%ld.%ld",
                        (long)osVer.majorVersion, (long)osVer.minorVersion, (long)osVer.patchVersion];
            }
        } else {
            // Version 10.16, without explicit env setting of SYSTEM_VERSION_COMPAT
            // AKA 11+ Read the *real* ProductVersion from the hidden link to avoid SYSTEM_VERSION_COMPAT
            // If not found, fallback below to the SystemVersion.plist
            NSDictionary *version = [NSDictionary dictionaryWithContentsOfFile :
                             @"/System/Library/CoreServices/.SystemVersionPlatform.plist"];
            if (version != NULL) {
                nsVerStr = [version objectForKey : @"ProductVersion"];
            }
        }
    }
    // Fallback if running on pre-10.9 Mac OS
    if (nsVerStr == NULL) {
        NSDictionary *version = [NSDictionary dictionaryWithContentsOfFile :
                                 @"/System/Library/CoreServices/SystemVersion.plist"];
        if (version != NULL) {
            nsVerStr = [version objectForKey : @"ProductVersion"];
        }
    }

    if (nsVerStr != NULL) {
        // Copy out the char*
        osVersionCStr = strdup([nsVerStr UTF8String]);
    }
    if (osVersionCStr == NULL) {
        osVersionCStr = strdup("Unknown");
    }
    os_info->version = osVersionCStr;

   return os_info;
}
