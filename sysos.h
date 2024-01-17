#ifndef PLAZMA_LIB_SYSOS_H
#define PLAZMA_LIB_SYSOS_H

 /*
  * OS definition.
  * 
  * Structures: 
  * - os_info_t
  *
  */

struct  os_info_t {
	char* name;
	char* nodename;
	char* release;
	char* version;
	char* machine;
};


int is_cpu_big_endian();

const char* get_cpu_endian();

os_info_t* get_os_info();

#endif // PLAZMA_LIB_SYSOS_H