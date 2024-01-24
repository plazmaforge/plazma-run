#ifndef PLAZMA_LIB_SYSUSER_OS_H
#define PLAZMA_LIB_SYSUSER_OS_H

struct user_info_t {
	char* user_name;
	char* user_home;
	char* user_dir;
    char* tmp_dir;
};

user_info_t* get_user_info();

#endif // PLAZMA_LIB_SYSUSER_H