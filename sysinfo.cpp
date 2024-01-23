#include <stdlib.h>
#include <stdio.h>

#include "sysos.h"
#include "sysuser.h"
#include "sysinfo.h"
#include "strlib.h"

static sys_info_t* sys_info = NULL;

static sys_info_t* new_sys_info() {
    sys_info_t* sys_info = (sys_info_t*) malloc(sizeof(sys_info_t));

    // OS Info
    sys_info->os_major_version = 0;
    sys_info->os_minor_version = 0;
    sys_info->os_build_version = 0;
    sys_info->os_arch = NULL;
    sys_info->os_arch_data = NULL;
	sys_info->node_name = NULL;
	sys_info->version = NULL;
    sys_info->release = NULL;

    // User Info
    sys_info->user_name = NULL;
	sys_info->user_home = NULL;
	sys_info->user_dir = NULL;
    sys_info->tmp_dir = NULL;

    return sys_info;
}

static void free_sys_info(sys_info_t* sys_info) {
    if (!sys_info)  {
        return;
    }

    // OS Info
    free(sys_info->os_name);
	free(sys_info->os_version);
	free(sys_info->os_release);
    free(sys_info->os_arch);
    free(sys_info->os_arch_data);
	free(sys_info->node_name);
	free(sys_info->version);
    free(sys_info->release);

    // User Info
    free(sys_info->user_name);
    free(sys_info->user_home);
    free(sys_info->user_dir);
    free(sys_info->tmp_dir);
	
    free(sys_info);
}

sys_info_t* get_sys_info() {

    if (sys_info) {
        return sys_info;
    }
    sys_info = new_sys_info();

    // OS Info
    os_info_t* os_info = get_os_info();
    if (os_info) {
        sys_info->os_name = os_info->os_name;
        sys_info->os_version = os_info->os_version;
        sys_info->os_major_version = os_info->os_major_version;
        sys_info->os_minor_version = os_info->os_minor_version;
        sys_info->os_build_version = os_info->os_build_version;
        sys_info->os_arch = os_info->os_arch;
        sys_info->os_arch_data = os_info->os_arch_data;
    }

    // User Info
    user_info_t* user_info = get_user_info();
    if (user_info) {
        sys_info->user_name = user_info->user_name;
        sys_info->user_home = user_info->user_home;
        sys_info->user_dir = user_info->user_dir;
        sys_info->tmp_dir = user_info->tmp_dir;
    }

    return sys_info;
}

void print_sys_info() {
    if (!sys_info) {
        // Load sys_info
        sys_info = get_sys_info();
    }
    if (!sys_info) {
        // Error loading sys_info
        return;
    }

    printf("\n");

    // OS Info
    printf("os.name        : %s\n", lib_strsaf(sys_info->os_name));
    printf("os.version     : %s\n", lib_strsaf(sys_info->os_version));
    printf("os.major       : %d\n", sys_info->os_major_version);
    printf("os.minor       : %d\n", sys_info->os_minor_version);
    printf("os.build       : %d\n", sys_info->os_build_version);
    printf("os.arch        : %s\n", lib_strsaf(sys_info->os_arch));
    printf("os.arch.data   : %s\n", lib_strsaf(sys_info->os_arch_data));   

    // User Info
    printf("user.name      : %s\n", lib_strsaf(sys_info->user_name));
    printf("user.home      : %s\n", lib_strsaf(sys_info->user_home));
    printf("user.dir       : %s\n", lib_strsaf(sys_info->user_dir));
    printf("tmp.dir        : %s\n", lib_strsaf(sys_info->tmp_dir));
      
}
