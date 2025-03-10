#ifndef PLAZMA_LIB_SYSOS_H
#define PLAZMA_LIB_SYSOS_H

#include "syscpu.h"

 /*
  * OS definition.
  * 
  * Structures: 
  * - lib_os_info_t
  *
  */

typedef struct lib_os_info_t {

    // OS Version Info
    char* os_name;
    char* os_version;
    char* os_release;        // *
    int os_major_version;
    int os_minor_version;
    int os_build_version;

    // Kernel Version Info
    char* version;           // kernel
    char* release;           // kernel

    // CPU Info
    char* os_arch;           // cpu
    int os_arch_size;        // cpu
    const char* cpu_isalist; // list of supported instruction sets
    char* cpu_endian;        // endianness of platform
    int cpu_count;           // number of processors

    // Network Info
    char* node_name;         // network

    // FS Info
    char* file_separator;
    char* line_separator;

} lib_os_info_t;


/**
 * Returns OS Arch Size by OS Arch Name
 */
//int lib_sys_get_os_arch_size(const char* arch_name);

/**
 * Returns OS Arch Data Model by OS Arch 
 */
const char* lib_sys_get_os_arch_data_model(const char* arch_name);

/**
 * Returns OS Info 
 */
const lib_os_info_t* lib_sys_get_os_info();


#endif // PLAZMA_LIB_SYSOS_H