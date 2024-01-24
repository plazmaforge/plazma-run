#ifndef PLAZMA_LIB_SYSINFO_OS_H
#define PLAZMA_LIB_SYSINFO_OS_H

struct sys_info_t {

    // OS Info
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

    // User Info
	char* user_name;
	char* user_home;
	char* user_dir;
    char* tmp_dir;

};

sys_info_t* get_sys_info();

// OS Info

char* get_os_name();

char* get_os_version();

int get_os_major_version();

int get_os_minor_version();

int get_os_build_version();

char* get_os_arch();

char* get_os_arch_data();

// User Info

char* get_user_name();

char* get_user_home();

char* get_user_dir();

char* get_work_dir();

char* get_tmp_dir();

////

void print_sys_info();

#endif // PLAZMA_LIB_SYSINFO_H