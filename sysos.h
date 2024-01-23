#ifndef PLAZMA_LIB_SYSOS_H
#define PLAZMA_LIB_SYSOS_H

 /*
  * OS definition.
  * 
  * Structures: 
  * - os_info_t
  *
  */

struct os_info_t {
	char* os_name;
	char* os_version;
	char* os_release;     // *
    int os_major_version;
    int os_minor_version;
    int os_build_version;
    char* os_arch;        // cpu
    char* os_arch_data;   // cpu
	char* node_name;      // network
	char* version;        // kernel
    char* release;        // kernel
};

/**
 * Returns true if CPU is big endian 
 */
int is_cpu_big_endian();

/**
 * Returns CPU endian: big/little
 */
const char* get_cpu_endian();

/**
 * Returns OS Arch Data by OS Arch 
 */
const char* get_os_arch_data(const char* os_arch);

/**
 * Returns OS Info 
 */
os_info_t* get_os_info();

os_info_t* new_os_info();

void free_os_info(os_info_t* os_info);

#endif // PLAZMA_LIB_SYSOS_H