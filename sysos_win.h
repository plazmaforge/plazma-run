#ifndef PLAZMA_LIB_SYSOS_WIN_H
#define PLAZMA_LIB_SYSOS_WIN_H

#if defined _WIN32
#include <io.h>
#include <windows.h>

//#include <shlobj.h>
//#include <objidl.h>
//#include <wincon.h>

#include <stdlib.h>
#include <string.h>
#include "sysos.h"


typedef struct {

  int majorVersion = 0;
  int minorVersion = 0;
  int buildNumber = 0;

  boolean is_workstation;
  boolean is_64bit;
  DWORD platformId;

} VersionInfo;

/*
 * From msdn page on OSVERSIONINFOEX, current as of this
 * writing, decoding of dwMajorVersion and dwMinorVersion.
 *
 *  Operating system            dwMajorVersion  dwMinorVersion
 * ==================           ==============  ==============
 *
 * Windows 95                   4               0
 * Windows 98                   4               10
 * Windows ME                   4               90
 * Windows 3.51                 3               51
 * Windows NT 4.0               4               0
 * Windows 2000                 5               0
 * Windows XP 32 bit            5               1
 * Windows Server 2003 family   5               2
 * Windows XP 64 bit            5               2
 *       where ((&ver.wServicePackMinor) + 2) = 1
 *       and  si.wProcessorArchitecture = 9
 * Windows Vista family         6               0  (VER_NT_WORKSTATION)
 * Windows Server 2008          6               0  (!VER_NT_WORKSTATION)
 * Windows 7                    6               1  (VER_NT_WORKSTATION)
 * Windows Server 2008 R2       6               1  (!VER_NT_WORKSTATION)
 * Windows 8                    6               2  (VER_NT_WORKSTATION)
 * Windows Server 2012          6               2  (!VER_NT_WORKSTATION)
 * Windows Server 2012 R2       6               3  (!VER_NT_WORKSTATION)
 * Windows 10                   10              0  (VER_NT_WORKSTATION)
 * Windows 11                   10              0  (VER_NT_WORKSTATION)
 *       where (buildNumber >= 22000)
 * Windows Server 2016          10              0  (!VER_NT_WORKSTATION)
 * Windows Server 2019          10              0  (!VER_NT_WORKSTATION)
 *       where (buildNumber > 17762)
 * Windows Server 2022          10              0  (!VER_NT_WORKSTATION)
 *       where (buildNumber > 20347)
 *
 * This mapping will presumably be augmented as new Windows
 * versions are released.
 */

const char* getWIN32_WINDOWS_Name(VersionInfo ver) {
  //printf("Windows Name\n");
  if (ver.majorVersion == 4) {
    switch (ver.minorVersion) {
      case  0: return "Windows 95";
      case 10: return "Windows 98";
      case 90: return "Windows Me";
    }
    return "Windows 9X (unknown)";
   }
   return "Windows 9X (unknown)"; 
}

const char* getWIN32_NT_5_Name(VersionInfo ver) {
  //printf("Windows NT 5 Name\n");
  switch (ver.minorVersion) {
    case  0: return "Windows 2000";
    case  1: return "Windows XP";
    case  2:
        /*
         * From MSDN OSVERSIONINFOEX and SYSTEM_INFO documentation:
         *
         * "Because the version numbers for Windows Server 2003
         * and Windows XP 6u4 bit are identical, you must also test
         * whether the wProductType member is VER_NT_WORKSTATION.
         * and si.wProcessorArchitecture is
         * PROCESSOR_ARCHITECTURE_AMD64 (which is 9)
         * If it is, the operating system is Windows XP 64 bit;
         * otherwise, it is Windows Server 2003."
         */
         if (ver.is_workstation && ver.is_64bit) {
            return "Windows XP"; /* 64 bit */
         } else {
             return "Windows 2003";
         }

  }
  return "Windows NT (unknown)"; 
}

const char* getWIN32_NT_6_Name(VersionInfo ver) {
  //printf("Windows NT 6 Name\n");
  /*
   * See table in MSDN OSVERSIONINFOEX documentation.
   */
   if (ver.is_workstation) {
        switch (ver.minorVersion) {
            case  0: return "Windows Vista";
            case  1: return "Windows 7";
            case  2: return "Windows 8";
            case  3: return "Windows 8.1";
          }
    } else {
        switch (ver.minorVersion) {
            case  0: return "Windows Server 2008";
            case  1: return "Windows Server 2008 R2";
            case  2: return "Windows Server 2012";
            case  3: return "Windows Server 2012 R2";
        }
    }  
    return "Windows NT (unknown)";
}

const char* getWIN32_NT_10_Name(VersionInfo ver) {
   if (ver.is_workstation) {
        switch (ver.minorVersion) {
            case  0:
              /* Windows 11 21H2 (original release) build number is 22000 */
              if (ver.buildNumber >= 22000) {
                  return "Windows 11";
               } else {
                  return "Windows 10";
               }
        }
    } else {
         switch (ver.minorVersion) {
            case  0:
              /* Windows server 2019 GA 10/2018 build number is 17763 */
              /* Windows server 2022 build number is 20348 */
              if (ver.buildNumber > 20347) {
                  return "Windows Server 2022";
               } else if (ver.buildNumber > 17762) {
                  return "Windows Server 2019";
               } else {
                  return "Windows Server 2016";
               }
           }
    }
    return "Windows NT (unknown)";
  
}

const char* getWIN32_NT_Name(VersionInfo ver) {
      
    if (ver.majorVersion <= 4) {
        return "Windows NT";
    }
    if (ver.majorVersion == 5) {
        return getWIN32_NT_5_Name(ver);
    }
    if (ver.majorVersion == 6) {
        return getWIN32_NT_6_Name(ver);
    }
    if (ver.majorVersion == 10) {
        return getWIN32_NT_10_Name(ver);
    }

    return "Windows NT (unknown)";
}

const char* getOsName(VersionInfo ver) {

    // WINDOWS
    if (ver.platformId == VER_PLATFORM_WIN32_WINDOWS) {
        return getWIN32_WINDOWS_Name(ver);
    }

    // NT
    if (ver.platformId == VER_PLATFORM_WIN32_NT) {
        return getWIN32_NT_Name(ver);
    }

    // UNKNOWN
    return  "Windows (unknown)";
}

char* getOsVersion(VersionInfo ver) {
   char buf[100];
   sprintf(buf, "%d.%d", ver.majorVersion, ver.minorVersion);
   return _strdup(buf);
}

// // by SYSTEM_INFO
// const char* getOsArch(SYSTEM_INFO& si) {
//    switch (si.wProcessorArchitecture) {
//     #ifdef PROCESSOR_ARCHITECTURE_IA64
//     case PROCESSOR_ARCHITECTURE_IA64: return "x86_64";
//     #endif
//     #ifdef PROCESSOR_ARCHITECTURE_AMD64
//     case PROCESSOR_ARCHITECTURE_AMD64: return "x86_64";
//     #endif
//     case PROCESSOR_ARCHITECTURE_INTEL:
//         switch (si.wProcessorLevel) {
//          case 6: 
//          case 5: 
//          case 4:
//          case 3: return "x86";
//          default: return "x86_64";
//         }
//     }     

//   return NULL;
// }

// by SYSTEM_INFO
// const char* getOsArchData(SYSTEM_INFO& si) {
//    switch (si.wProcessorArchitecture) {
//     #ifdef PROCESSOR_ARCHITECTURE_IA64
//     case PROCESSOR_ARCHITECTURE_IA64: return "64";
//     #endif
//     #ifdef PROCESSOR_ARCHITECTURE_AMD64
//     case PROCESSOR_ARCHITECTURE_AMD64: return "64";
//     #endif
//     case PROCESSOR_ARCHITECTURE_INTEL:
//         switch (si.wProcessorLevel) {
//          case 6: 
//          case 5: 
//          case 4:
//          case 3: return "32";
//          default: return "64";
//         }
//     }     
 
//    return NULL;
// }

////

// static boolean haveMMX(void) {
//     return IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE);
// }

// static const char* getCpuIsalist(SYSTEM_INFO& si) {
//    switch (si.wProcessorArchitecture) {
//     #ifdef PROCESSOR_ARCHITECTURE_IA64
//     case PROCESSOR_ARCHITECTURE_IA64: return "ia64";
//     #endif
//     #ifdef PROCESSOR_ARCHITECTURE_AMD64
//     case PROCESSOR_ARCHITECTURE_AMD64: return "amd64";
//     #endif
//     case PROCESSOR_ARCHITECTURE_INTEL:
//         switch (si.wProcessorLevel) {
//         case 6: return haveMMX()
//             ? "pentium_pro+mmx pentium_pro pentium+mmx pentium i486 i386 i86"
//             : "pentium_pro pentium i486 i386 i86";
//         case 5: return haveMMX()
//             ? "pentium+mmx pentium i486 i386 i86"
//             : "pentium i486 i386 i86";
//         case 4: return "i486 i386 i86";
//         case 3: return "i386 i86";
//         }
//     }
//     return ""; 
// }

void loadOsInfo(os_info_t* os_info) {

  int majorVersion = 0;
  int minorVersion = 0;
  int buildNumber = 0;

  /* tmp dir */
  //WCHAR tmpdir[MAX_PATH + 1];
  //GetTempPathW(MAX_PATH + 1, tmpdir);
  //sysInfo.tmp_dir = _wcsdup(tmpdir);

  /* OS properties */
  char buf[100];
  boolean is_workstation;
  boolean is_64bit;
  //int cpu_count = 0;
  DWORD platformId;

  ////

  OSVERSIONINFOEX ver;
  ver.dwOSVersionInfoSize = sizeof(ver);

  GetVersionEx((OSVERSIONINFO *) &ver);
  majorVersion = ver.dwMajorVersion;
  minorVersion = ver.dwMinorVersion;

  /* Distinguish Windows Server 2016+ by build number */
  buildNumber = ver.dwBuildNumber;
  is_workstation = (ver.wProductType == VER_NT_WORKSTATION);
  platformId = ver.dwPlatformId;

  //sysInfo.patch_level = _strdup(ver.szCSDVersion); 

  //printf("majorVersion  : %d\n", majorVersion); 
  //printf("minorVersion  : %d\n", minorVersion);  
  //printf("buildNumber   : %d\n", buildNumber);
  //printf("platformId    : %d\n", platformId); 
  //printf("is_workstation: %d\n", is_workstation);

  ////

  SYSTEM_INFO si;
  ZeroMemory(&si, sizeof(SYSTEM_INFO));
  GetNativeSystemInfo(&si);

  is_64bit = (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);
  //cpu_count = si.dwNumberOfProcessors;

  ////

  do {

      // Read the major and minor version number from kernel32.dll
      VS_FIXEDFILEINFO *file_info;
      WCHAR kernel32_path[MAX_PATH];
      DWORD version_size;
      LPTSTR version_info;
      UINT len, ret;

      // Get the full path to \Windows\System32\kernel32.dll and use that for
      // determining what version of Windows we're running on.
      len = MAX_PATH - (UINT)strlen("\\kernel32.dll") - 1;
      ret = GetSystemDirectoryW(kernel32_path, len);

      if (ret == 0 || ret > len) {
          break;
      }

      wcsncat(kernel32_path, L"\\kernel32.dll", MAX_PATH - ret);
      version_size = GetFileVersionInfoSizeW(kernel32_path, NULL);

      if (version_size == 0) {
          break;
      }

      version_info = (LPTSTR)malloc(version_size);
      if (version_info == NULL) {
          break;
      }

      if (!GetFileVersionInfoW(kernel32_path, 0, version_size, version_info)) {
          free(version_info);
          break;
      }

      if (!VerQueryValueW(version_info, L"\\", (LPVOID *)&file_info, &len)) {
          free(version_info);
          break;
      }

      majorVersion = HIWORD(file_info->dwProductVersionMS);
      minorVersion = LOWORD(file_info->dwProductVersionMS);
      buildNumber = HIWORD(file_info->dwProductVersionLS);
      free(version_info);
      
  } while (0);

  ////

  VersionInfo versionInfo;

  versionInfo.platformId = platformId;
  versionInfo.majorVersion = majorVersion;
  versionInfo.minorVersion = minorVersion;

  versionInfo.is_workstation = is_workstation;
  versionInfo.is_64bit = is_64bit;

  // Version Info
  os_info->os_name = _strdup(getOsName(versionInfo));
  os_info->os_version = _strdup(getOsVersion(versionInfo));

  os_info->os_major_version = majorVersion;
  os_info->os_minor_version = minorVersion;
  os_info->os_build_version = buildNumber;

  // CPU Info
  //os_info->os_arch = _strdup(getOsArch(si));
  //os_info->os_arch_size = get_os_arch_size(os_info->os_arch);
  //os_info->os_arch_data = _strdup(getOsArchData(si));
  //os_info->cpu_endian = _strdup(get_cpu_endian());
  //os_info->cpu_isalist = getCpuIsalist(si);

  os_info->os_arch = _strdup(get_cpu_arch_name_by_si(si));
  os_info->os_arch_size = get_os_arch_size(os_info->os_arch);
  os_info->cpu_endian = _strdup(get_cpu_endian());
  os_info->cpu_isalist = get_cpu_isalist_by_si(si);

  // C CPU Issue: os.cpu_count() WIN: GetLogicalProcessorInformationEx
  // https://github.com/giampaolo/psutil/issues/771
  // https://github.com/umezawatakeshi/GetLogicalProcessorInformationEx/blob/master/GetLogicalProcessorInformationEx.cc
  os_info->cpu_count = get_cpu_count_by_si(si); //cpu_count;

  // FS Info
  os_info->file_separator = _strdup("\\");
  os_info->line_separator = _strdup("\r\n");
 
}

void load_os_info(os_info_t* os_info) {
    loadOsInfo(os_info);
}

#endif

#endif // PLAZMA_LIB_SYSOS_WIN_H