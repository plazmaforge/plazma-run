#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sysos.h"

#if defined _WIN32
#include "sysos_win.h"
#else
#include "sysos_nix.h"
#endif

static os_info_t* os_info = NULL;

int get_os_arch_size(const char* arch_name) {
    return get_cpu_arch_size(arch_name);
}

const char* get_os_arch_data_model(const char* arch_name) {
    return get_cpu_arch_data_model(arch_name);
}

static void init_os_info(os_info_t* os_info) {
    if (!os_info) {
        return;
    }

    // Version Info
    os_info->os_name = NULL;
	os_info->os_version = NULL;
	os_info->os_release = NULL;
    os_info->os_major_version = 0;
    os_info->os_minor_version = 0;
    os_info->os_build_version = 0;

    // Kernel Info
    os_info->version = NULL;
    os_info->release = NULL;

    // CPU Info
    os_info->os_arch = NULL;
    os_info->os_arch_size = 0;
    //os_info->os_arch_data = NULL;
    os_info->cpu_isalist = NULL;
    os_info->cpu_endian = NULL;
    os_info->cpu_count = 0;

    // Network Info
	os_info->node_name = NULL;

    // FS Info
    os_info->file_separator = NULL;
    os_info->line_separator = NULL;

}

/*
static os_info_t* new_os_info() {
    os_info_t* os_info = (os_info_t*) malloc(sizeof(os_info_t));
    init_os_info(os_info);
    return os_info;
}
*/

/*
static void free_os_info(os_info_t* os_info) {
    if (!os_info)  {
        return;
    }

    // Version Info
    free(os_info->os_name);
	free(os_info->os_version);
	free(os_info->os_release);

    // Kernel Info
	free(os_info->version);
    free(os_info->release);

    // CPU Info
    free(os_info->os_arch);
    //free(os_info->os_arch_data);
    //free(os_info->cpu_isalist);
    free(os_info->cpu_endian);

    // Network Info
	free(os_info->node_name);

    // FS Info
    free(os_info->file_separator);
    free(os_info->line_separator);

    free(os_info);
}
*/

const os_info_t* get_os_info() {
    if (os_info) {
        return os_info;
    }

    static os_info_t os_info_s;
    os_info = &os_info_s;
    init_os_info(os_info);
    load_os_info(os_info);

    return os_info;
}
