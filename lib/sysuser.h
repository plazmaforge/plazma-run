#ifndef PLAZMA_LIB_SYSUSER_OS_H
#define PLAZMA_LIB_SYSUSER_OS_H

struct lib_user_info_t {
	char* user_name;
	char* user_home;
	char* user_dir;
    char* tmp_dir;
};

const lib_user_info_t* lib_sys_get_user_info();

#endif // PLAZMA_LIB_SYSUSER_H