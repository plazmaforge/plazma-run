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
static const char* get_cpu_arch_name_by_si_1(SYSTEM_INFO& info) {
   switch (info.wProcessorArchitecture) {
    #ifdef PROCESSOR_ARCHITECTURE_IA64
    case PROCESSOR_ARCHITECTURE_IA64: return "x86_64";
    #endif
    #ifdef PROCESSOR_ARCHITECTURE_AMD64
    case PROCESSOR_ARCHITECTURE_AMD64: return "x86_64";
    #endif
    case PROCESSOR_ARCHITECTURE_INTEL:
        switch (info.wProcessorLevel) {
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
const char* get_cpu_isalist_by_si(SYSTEM_INFO& info) {
   switch (info.wProcessorArchitecture) {
    #ifdef PROCESSOR_ARCHITECTURE_IA64
    case PROCESSOR_ARCHITECTURE_IA64: return "ia64";
    #endif
    #ifdef PROCESSOR_ARCHITECTURE_AMD64
    case PROCESSOR_ARCHITECTURE_AMD64: return "x86_64"; //"amd64";
    #endif
    case PROCESSOR_ARCHITECTURE_INTEL:
        switch (info.wProcessorLevel) {
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
    return get_cpu_arch_name_by_si(info);
}

arch_t get_cpu_arch_type_by_si(SYSTEM_INFO& info) {

    switch (info.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_IA64:
        return ARCH_ITANIUM;
    case PROCESSOR_ARCHITECTURE_AMD64:
        return ARCH_X86_64;
    case PROCESSOR_ARCHITECTURE_INTEL:
        switch (info.wProcessorLevel) {
         case 6: 
         case 5: 
         case 4:
         case 3: return ARCH_I686;
         default: return ARCH_X86_64;
        }

    
    case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
        return ARCH_I686;
    case PROCESSOR_ARCHITECTURE_MIPS:
        return ARCH_MIPS;
    case PROCESSOR_ARCHITECTURE_ALPHA:
        return ARCH_ALPHA;
    case PROCESSOR_ARCHITECTURE_PPC:
        return ARCH_PPC;
    case PROCESSOR_ARCHITECTURE_SHX:
        return ARCH_SH4;
    case PROCESSOR_ARCHITECTURE_ARM:
        return ARCH_ARMV7L;
    case PROCESSOR_ARCHITECTURE_ARM64:
        return ARCH_AARCH64;
    

    default:
        return ARCH_NONE;
    }
}

const char* get_cpu_arch_name_by_si_2(SYSTEM_INFO& info) {
    arch_t arch = get_cpu_arch_type_by_si(info);
    return lib_sys_get_arch_name(arch);
}

const char* get_cpu_arch_name_by_si(SYSTEM_INFO& info) {
    return get_cpu_arch_name_by_si_2(info);
}

int get_cpu_count_by_si(SYSTEM_INFO& info) {
   int count = info.dwNumberOfProcessors;

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

void load_system_info(SYSTEM_INFO& info) {
    //GetSystemInfo(&info);

    ZeroMemory(&info, sizeof(SYSTEM_INFO));
    GetNativeSystemInfo(&info);
}

////

arch_t lib_sys_get_cpu_arch_type() {
    SYSTEM_INFO info;
    load_system_info(info);
    return get_cpu_arch_type_by_si(info);
}

const char* get_cpu_issalist() {
    SYSTEM_INFO info;
    load_system_info(info);
    return get_cpu_isalist_by_si(info);
}

int _get_cpu_count() {
    SYSTEM_INFO info;
    load_system_info(info);
    return get_cpu_count_by_si(info);
}

#endif

#endif // PLAZMA_LIB_SYSCPU_WIN_H

