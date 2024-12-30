
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#ifdef _WIN32
#include "sysuser_win.h"
#else
#include "sysuser_nix.h"
#endif

#include "sysuser.h"

static lib_user_info_t* user_info = NULL;

static void lib_sys_user_info_init(lib_user_info_t* user_info) {
    user_info->user_name = NULL;
	user_info->user_home = NULL;
	user_info->user_dir = NULL;
    user_info->tmp_dir = NULL;
}

/*
static lib_user_info_t* lib_sys_user_info_new() {
    lib_user_info_t* user_info = (lib_user_info_t*) malloc(sizeof(lib_user_info_t));
    init_user_info(user_info);
    return user_info;
}
*/

/*
static void lib_sys_user_info_free(lib_user_info_t* user_info) {
    if (!user_info)  {
        return;
    }
    free(user_info->user_name);
	free(user_info->user_home);
	free(user_info->user_dir);
    free(user_info->tmp_dir);

    free(user_info);
}
*/

const lib_user_info_t* lib_sys_get_user_info() {
    if (user_info) {
        return user_info;
    }
    //user_info = lib_user_info_new();
    static lib_user_info_t user_info_s;
    user_info = &user_info_s;
    lib_sys_user_info_init(user_info);
    
    user_info->user_name = getUserName();
    user_info->user_home = getUserHome();
    user_info->user_dir = getUserDir();
    user_info->tmp_dir = getTmpDir();

    return user_info;
}
