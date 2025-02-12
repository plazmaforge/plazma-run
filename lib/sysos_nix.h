#ifndef PLAZMA_LIB_SYSOS_NIX_H
#define PLAZMA_LIB_SYSOS_NIX_H

#if defined __unix__ || defined __linux__ || (defined __APPLE__ && defined __MACH__)
//#if defined(__linux__) || defined(_ALLBSD_SOURCE)
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>    /* For os_name and os_version */
#include <unistd.h>         /* CPU - sysconf(_SC_NPROCESSORS_ONLN) [NonMacOS?] */
#include "sysos.h"

void lib_sys_load_posix_common_info_opts(lib_os_info_t* os_info, struct utsname* name) {

   // CPU Info
   os_info->os_arch = strdup(lib_cpu_get_cpu_arch_name_by_machine(name->machine));
   os_info->os_arch_size = lib_cpu_get_cpu_arch_size(os_info->os_arch);
   os_info->cpu_endian = strdup(lib_cpu_get_cpu_endian());
   os_info->cpu_isalist = strdup(lib_cpu_get_cpu_issalist_by_machine(name->machine));

   // FS Info
   os_info->file_separator = strdup("/");
   os_info->line_separator = strdup("\n");
}

void lib_sys_load_posix_common_info(lib_os_info_t* os_info) {
   struct utsname name;
   uname(&name);
   lib_sys_load_posix_common_info_opts(os_info, &name);
}

void lib_sys_load_posix_cpu_info(lib_os_info_t* os_info) {
    os_info->cpu_count = lib_cpu_get_cpu_count();
}

#if defined __APPLE__ && defined __MACH__
#include "sysos_mac.h"
#else
void lib_sys_os_info_load(lib_os_info_t* os_info) {

   struct utsname name;
   uname(&name);

    os_info->os_name = strdup(name.sysname);
    #ifdef _AIX
        char os_version[strlen(name.version) + strlen(name.release) + 2];
        strcpy(os_version, name.version);
        strcat(os_version, ".");
        strcat(os_version, name.release);
        os_info->os_version = strdup(os_version);
        //os_info->os_major_version = atoi(name.version);
        //os_info->os_minor_version = atoi(name.release);
    #else
        os_info->os_version = strdup(name.release); // 'name.version' is full version string
        // TODO: split os_version by '.' and set major/minor/build
    #endif

    lib_sys_load_posix_common_info(os_info, &name);
    lib_sys_load_posix_cpu_info(os_info);

}
#endif

#endif

#endif // PLAZMA_LIB_SYSOS_NIX_H