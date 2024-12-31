#ifndef PLAZMA_LIB_SYSCPU_WIN_H
#define PLAZMA_LIB_SYSCPU_WIN_H

#if defined _WIN32
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "syscpu.h"

#ifndef PROCESSOR_ARCHITECTURE_ARM64
#define PROCESSOR_ARCHITECTURE_ARM64 12
#endif

static int has_mmx() {
    return IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE);
}

// legacy
static const char* lib_cpu_get_cpu_arch_name_by_si_1(SYSTEM_INFO* info) {
   switch (info->wProcessorArchitecture) {
    #ifdef PROCESSOR_ARCHITECTURE_IA64
    case PROCESSOR_ARCHITECTURE_IA64: return "x86_64";
    #endif
    #ifdef PROCESSOR_ARCHITECTURE_AMD64
    case PROCESSOR_ARCHITECTURE_AMD64: return "x86_64";
    #endif
    case PROCESSOR_ARCHITECTURE_INTEL:
        switch (info->wProcessorLevel) {
         case 6: 
         case 5: 
         case 4:
         case 3: return "x86";
         default: return "x86_64";
        }
    }     
  return "";
}

// legacy (?)
const char* lib_cpu_get_cpu_isalist_by_si(SYSTEM_INFO* info) {
   switch (info->wProcessorArchitecture) {
    #ifdef PROCESSOR_ARCHITECTURE_IA64
    case PROCESSOR_ARCHITECTURE_IA64: return "ia64";
    #endif
    #ifdef PROCESSOR_ARCHITECTURE_AMD64
    case PROCESSOR_ARCHITECTURE_AMD64: return "x86_64"; //"amd64";
    #endif
    case PROCESSOR_ARCHITECTURE_INTEL:
        switch (info->wProcessorLevel) {
        case 6: return has_mmx()
            ? "pentium_pro+mmx pentium_pro pentium+mmx pentium i486 i386 i86"
            : "pentium_pro pentium i486 i386 i86";
        case 5: return has_mmx()
            ? "pentium+mmx pentium i486 i386 i86"
            : "pentium i486 i386 i86";
        case 4: return "i486 i386 i86";
        case 3: return "i386 i86";
        }
    }
    //return ""; 
    return lib_cpu_get_cpu_arch_name_by_si(info);
}

lib_arch_t lib_cpu_get_cpu_arch_type_by_si(SYSTEM_INFO* info) {

    switch (info->wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_IA64:
        return LIB_ARCH_ITANIUM;
    case PROCESSOR_ARCHITECTURE_AMD64:
        return LIB_ARCH_X86_64;
    case PROCESSOR_ARCHITECTURE_INTEL:
        switch (info->wProcessorLevel) {
         case 6: 
         case 5: 
         case 4:
         case 3: return LIB_ARCH_I686;
         default: return LIB_ARCH_X86_64;
        }

    
    case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
        return LIB_ARCH_I686;
    case PROCESSOR_ARCHITECTURE_MIPS:
        return LIB_ARCH_MIPS;
    case PROCESSOR_ARCHITECTURE_ALPHA:
        return LIB_ARCH_ALPHA;
    case PROCESSOR_ARCHITECTURE_PPC:
        return LIB_ARCH_PPC;
    case PROCESSOR_ARCHITECTURE_SHX:
        return LIB_ARCH_SH4;
    case PROCESSOR_ARCHITECTURE_ARM:
        return LIB_ARCH_ARMV7L;
    case PROCESSOR_ARCHITECTURE_ARM64:
        return LIB_ARCH_AARCH64;
    

    default:
        return LIB_ARCH_NONE;
    }
}

const char* lib_cpu_get_cpu_arch_name_by_si_2(SYSTEM_INFO* info) {
    lib_arch_t arch = lib_cpu_get_cpu_arch_type_by_si(info);
    return lib_cpu_get_arch_name(arch);
}

const char* lib_cpu_get_cpu_arch_name_by_si(SYSTEM_INFO* info) {
    return lib_cpu_get_cpu_arch_name_by_si_2(info);
}

int lib_cpu_get_cpu_count_by_si(SYSTEM_INFO* info) {
   int count = info->dwNumberOfProcessors;

   DWORD_PTR process_cpus;
   DWORD_PTR system_cpus;

   //gl
   if (GetProcessAffinityMask (GetCurrentProcess (), &process_cpus, &system_cpus)) {
      unsigned int af_count;
      for (af_count = 0; process_cpus != 0; process_cpus >>= 1)
        if (process_cpus & 1)
          af_count++;

      //printf("Processors count %d\n", af_count);
  
      /* Prefer affinity-based result, if available */
      if (af_count > 0)
        count = af_count;
    }
   return count > 0 ? count : 1;
}

////

void load_system_info(SYSTEM_INFO* info) {
    ZeroMemory(info, sizeof(SYSTEM_INFO));
    GetNativeSystemInfo(info);
}

////

lib_arch_t lib_cpu_get_cpu_arch_type() {
    SYSTEM_INFO info;
    load_system_info(&info);
    return lib_cpu_get_cpu_arch_type_by_si(&info);
}

const char* lib_cpu_get_cpu_issalist() {
    SYSTEM_INFO info;
    load_system_info(&info);
    return lib_cpu_get_cpu_isalist_by_si(&info);
}

int lib_cpu_get_cpu_count() {
    SYSTEM_INFO info;
    load_system_info(&info);
    return lib_cpu_get_cpu_count_by_si(&info);
}

#endif

#endif // PLAZMA_LIB_SYSCPU_WIN_H

