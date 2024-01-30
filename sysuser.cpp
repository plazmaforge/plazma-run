
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

static user_info_t* user_info = NULL;

static void init_user_info(user_info_t* user_info) {
    user_info->user_name = NULL;
	user_info->user_home = NULL;
	user_info->user_dir = NULL;
    user_info->tmp_dir = NULL;
}

/*
static user_info_t* new_user_info() {
    user_info_t* user_info = (user_info_t*) malloc(sizeof(user_info_t));
    init_user_info(user_info);
    return user_info;
}
*/

/*
static void free_user_info(user_info_t* user_info) {
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

const user_info_t* get_user_info() {
    if (user_info) {
        return user_info;
    }
    //user_info = new_user_info();
    static user_info_t user_info_s;
    user_info = &user_info_s;
    init_user_info(user_info);
    
    user_info->user_name = getUserName();
    user_info->user_home = getUserHome();
    user_info->user_dir = getUserDir();
    user_info->tmp_dir = getTmpDir();

    return user_info;
}
