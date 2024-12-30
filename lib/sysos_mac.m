#include <Foundation/Foundation.h>

#include "sysos.h"

// 10.9 SDK does not include the NSOperatingSystemVersion struct.
// For now, create our own
typedef struct {
        NSInteger majorVersion;
        NSInteger minorVersion;
        NSInteger patchVersion;
} OSVerStruct;

void lib_sys_load_os_version_info_objc(lib_os_info_t* os_info) {

    NSString *osVerStr = NULL;
    //char* osVerCStr = NULL;
    OSVerStruct osVer;

    // Mac OS 10.9 includes the [NSProcessInfo operatingSystemVersion] function,
    // but it's not in the 10.9 SDK.  So, call it via NSInvocation.
    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(operatingSystemVersion)]) {
        //OSVerStruct osVer;
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
                osVerStr = [NSString stringWithFormat:@"%ld.%ld",
                        (long)osVer.majorVersion, (long)osVer.minorVersion];
            } else {
                osVerStr = [NSString stringWithFormat:@"%ld.%ld.%ld",
                        (long)osVer.majorVersion, (long)osVer.minorVersion, (long)osVer.patchVersion];
            }
        } else {
            // Version 10.16, without explicit env setting of SYSTEM_VERSION_COMPAT
            // AKA 11+ Read the *real* ProductVersion from the hidden link to avoid SYSTEM_VERSION_COMPAT
            // If not found, fallback below to the SystemVersion.plist
            NSDictionary *version = [NSDictionary dictionaryWithContentsOfFile :
                             @"/System/Library/CoreServices/.SystemVersionPlatform.plist"];
            if (version != NULL) {
                osVerStr = [version objectForKey : @"ProductVersion"];
            }
        }
    }
    // Fallback if running on pre-10.9 Mac OS
    if (osVerStr == NULL) {
        NSDictionary *version = [NSDictionary dictionaryWithContentsOfFile :
                                 @"/System/Library/CoreServices/SystemVersion.plist"];
        if (version != NULL) {
            osVerStr = [version objectForKey : @"ProductVersion"];
        }
    }

    //if (osVerStr != NULL) {
    //    // Copy out the char*
    //    osVerCStr = strdup([osVerStr UTF8String]);
    //}
    //if (osVerCStr == NULL) {
    //    osVerCStr = strdup("Unknown");
    //}

    // Hardcode os_name, and fill in os_version
    os_info->os_name = strdup("Mac OS X");
    os_info->os_version = strdup(osVerStr ? [osVerStr UTF8String] : "Unknown");

    os_info->os_major_version = osVer.majorVersion;
    os_info->os_minor_version = osVer.minorVersion;
    os_info->os_build_version = osVer.patchVersion;

}
