#ifndef PLAZMA_LIB_SYSUSER_H
#define PLAZMA_LIB_SYSUSER_H

typedef struct lib_user_info_t {
	char* user_name;
	char* user_home;
	char* user_dir;
    char* tmp_dir;
} lib_user_info_t;

const lib_user_info_t* lib_sys_get_user_info();

#endif // PLAZMA_LIB_SYSUSER_H