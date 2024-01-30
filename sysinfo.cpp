#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include "sysos.h"
#include "sysuser.h"
#include "sysinfo.h"
#include "syslocale_os.h"
#include "strlib.h"

int LC_FORMAT_TYPE = LC_CTYPE;
int LC_DISPLAY_TYPE = LC_TIME + 1;
  
static sys_info_t* sys_info = NULL;

static sys_info_t* init_sys_info(sys_info_t* sys_info) {
    
    // Version Info
    sys_info->os_major_version = 0;
    sys_info->os_minor_version = 0;
    sys_info->os_build_version = 0;
	sys_info->version = NULL;
    sys_info->release = NULL;

    // CPU Info
    sys_info->os_arch = NULL;
    sys_info->os_arch_size = 0;
    //sys_info->os_arch_data = NULL;
    sys_info->cpu_isalist = NULL;
    sys_info->cpu_endian = NULL;

    // FS Info
    sys_info->file_separator = NULL;
    sys_info->line_separator = NULL;

    // Network Info
	sys_info->node_name = NULL;

    // User Info
    sys_info->user_name = NULL;
	sys_info->user_home = NULL;
	sys_info->user_dir = NULL;
    sys_info->tmp_dir = NULL;

     // Current Locale Type (Format/Dysplay)
    sys_info->locale_type = 0;
  
    // Format Locale Info
    sys_info->format_locale = NULL;
    sys_info->format_language = NULL;
    sys_info->format_script = NULL;
    sys_info->format_country = NULL;
    sys_info->format_variant = NULL;
    sys_info->format_encoding = NULL;

    // Display Locale Info
    sys_info->display_locale = NULL;
    sys_info->display_language = NULL;    
    sys_info->display_script = NULL;    
    sys_info->display_country = NULL;    
    sys_info->display_variant = NULL;
    sys_info->display_encoding = NULL;
 
    // Encoding Info
    sys_info->encoding = NULL;
    sys_info->stdout_encoding = NULL;
    sys_info->stderr_encoding = NULL;
    sys_info->unicode_encoding = NULL;

    return sys_info;
}

/*
static void free_sys_info(sys_info_t* sys_info) {
    if (!sys_info)  {
        return;
    }

    // Version Info
    free(sys_info->os_name);
	free(sys_info->os_version);
	free(sys_info->os_release);
	free(sys_info->version);
    free(sys_info->release);

    // CPU Info
    free(sys_info->os_arch);
    //free(sys_info->os_arch_data);
    //free(sys_info->cpu_isalist);
    free(sys_info->cpu_endian);

    // Network Info
	free(sys_info->node_name);

    // FS Info
    free(sys_info->file_separator);
    free(sys_info->line_separator);

    // User Info
    free(sys_info->user_name);
    free(sys_info->user_home);
    free(sys_info->user_dir);
    free(sys_info->tmp_dir);

    // Format Locale Info
    free(sys_info->format_locale);
    free(sys_info->format_language);
    free(sys_info->format_script);
    free(sys_info->format_country);
    free(sys_info->format_variant);
    free(sys_info->format_encoding);

    // Display Locale Info
    free(sys_info->display_locale);
    free(sys_info->display_language);    
    free(sys_info->display_script);    
    free(sys_info->display_country);    
    free(sys_info->display_variant);
    free(sys_info->display_encoding);

    // Encoding Info
    free(sys_info->encoding);
    free(sys_info->stdout_encoding);
    free(sys_info->stderr_encoding);
    free(sys_info->unicode_encoding);
	
    free(sys_info);
}
*/

/*
static sys_info_t* new_sys_info() {
    sys_info_t* sys_info = (sys_info_t*) malloc(sizeof(sys_info_t));
    init_sys_info(sys_info);
    return sys_info;
}
*/

const sys_info_t* get_sys_info() {

    if (sys_info) {
        return sys_info;
    }
    //sys_info = new_sys_info();
    static sys_info_t sys_info_s;
    sys_info = &sys_info_s;
    init_sys_info(sys_info);

    // OS Info
    const os_info_t* os_info = get_os_info();
    if (os_info) {

        // Version Info
        sys_info->os_name = os_info->os_name;
        sys_info->os_version = os_info->os_version;
        sys_info->os_major_version = os_info->os_major_version;
        sys_info->os_minor_version = os_info->os_minor_version;
        sys_info->os_build_version = os_info->os_build_version;

        // CPU Info
        sys_info->os_arch = os_info->os_arch;
        sys_info->os_arch_size = os_info->os_arch_size;
        //sys_info->os_arch_data = os_info->os_arch_data;
        sys_info->cpu_isalist = os_info->cpu_isalist;
        sys_info->cpu_endian = os_info->cpu_endian;

        // FS Info
        sys_info->file_separator = os_info->file_separator;
        sys_info->line_separator = os_info->line_separator;
    }

    // User Info
    const user_info_t* user_info = get_user_info();
    if (user_info) {
        sys_info->user_name = user_info->user_name;
        sys_info->user_home = user_info->user_home;
        sys_info->user_dir = user_info->user_dir;
        sys_info->tmp_dir = user_info->tmp_dir;
    }

    sys_info->locale_type = 0;
  
    // Format Locale Info  
    locale_t* format_locale = load_locale_os(LC_FORMAT_TYPE);
    if (format_locale) {
        sys_info->locale_type = LC_FORMAT_TYPE;
        sys_info->format_locale = lib_strdup(format_locale->name);
        sys_info->format_language = lib_strdup(format_locale->language);
        sys_info->format_script = lib_strdup(format_locale->script);
        sys_info->format_country = lib_strdup(format_locale->country);
        sys_info->format_variant = lib_strdup(format_locale->variant);
        sys_info->format_encoding = lib_strdup(format_locale->encoding);
    }

    // Display Locale Info
    locale_t* display_locale = load_locale_os(LC_DISPLAY_TYPE);
    if (display_locale) {
        sys_info->locale_type = LC_DISPLAY_TYPE;
        sys_info->display_locale = lib_strdup(display_locale->name);
        sys_info->display_language = lib_strdup(display_locale->language);
        sys_info->display_script = lib_strdup(display_locale->script);
        sys_info->display_country = lib_strdup(display_locale->country);
        sys_info->display_variant = lib_strdup(display_locale->variant);
        sys_info->display_encoding = lib_strdup(display_locale->encoding);
    }

    // Select correct locale form format and display
    if (sys_info->locale_type == LC_DISPLAY_TYPE && !display_locale->encoding) {
        sys_info->locale_type = LC_FORMAT_TYPE;
    }

    if (sys_info->locale_type == LC_FORMAT_TYPE) {
        sys_info->encoding = lib_strdup(format_locale->encoding);
    } else if (sys_info->locale_type == LC_DISPLAY_TYPE) {
        sys_info->encoding = lib_strdup(display_locale->encoding);
    } else {
        sys_info->encoding = "UTF-8"; // TODO: ISO
    }

    free_locale(format_locale);
    free_locale(display_locale);

    /*
    if (isatty(STDOUT_FILENO) == 1) {
        sys_info->stdout_encoding = sys_info->encoding;
    }
    if (isatty(STDERR_FILENO) == 1) {
        sys_info->stderr_encoding = sys_info->encoding;
    }
    */

    /*
    #ifdef _ALLBSD_SOURCE
    #if BYTE_ORDER == _LITTLE_ENDIAN
        sys_info->unicode_encoding = "UnicodeLittle";
    #else
        sys_info->unicode_encoding = "UnicodeBig";
    #endif
    #else  // !_ALLBSD_SOURCE
    #ifdef __linux__
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        sys_info->unicode_encoding = "UnicodeLittle";
    #else
        sys_info->unicode_encoding = "UnicodeBig";
    #endif
    #else
        sys_info->unicode_encoding = "UnicodeBig";
    #endif
    #endif // _ALLBSD_SOURCE
    */

   /*
   // WIN32
   sys_info->unicode_encoding = "UnicodeLittle";
   */

    return sys_info;
}

// Version Info

const char* get_os_name() {
    const os_info_t* os_info = get_os_info();
    return os_info ? os_info->os_name : NULL;
}

const char* get_os_version() {
    const os_info_t* os_info = get_os_info();
    return os_info ? os_info->os_version : NULL;
}

int get_os_major_version() {
    const os_info_t* os_info = get_os_info();
    return os_info ? os_info->os_major_version : 0;
}

int get_os_minor_version() {
    const os_info_t* os_info = get_os_info();
    return os_info ? os_info->os_minor_version : 0;
}

int get_os_build_version() {
    const os_info_t* os_info = get_os_info();
    return os_info ? os_info->os_build_version : 0;
}

// CPU Info

const char* get_os_arch() {
    const os_info_t* os_info = get_os_info();
    return os_info ? os_info->os_arch : NULL;
}

int get_os_arch_size() {
    const os_info_t* os_info = get_os_info();
    return get_os_arch_size(os_info->os_arch);
}

// arch_model: char* value
// const char* get_os_arch_data() {
//     const os_info_t* os_info = get_os_info();
//     return os_info ? os_info->os_arch_data : NULL;
// }

const char* get_cpu_isalist() {
    const os_info_t* os_info = get_os_info();
    return os_info ? os_info->cpu_isalist : NULL;
}

// FS Info

const char* get_file_separator() {
    const os_info_t* os_info = get_os_info();
    return os_info ? os_info->file_separator : NULL;
}

const char* get_line_separator() {
    const os_info_t* os_info = get_os_info();
    return os_info ? os_info->line_separator : NULL;
}

// User Info

const char* get_user_name() {
    const user_info_t* user_info = get_user_info();
    return user_info ? user_info->user_name : NULL;
}

const char* get_user_home() {
    const user_info_t* user_info = get_user_info();
    return user_info ? user_info->user_home : NULL;
}

const char* get_user_dir() {
    const user_info_t* user_info = get_user_info();
    return user_info ? user_info->user_dir : NULL;
}

const char* get_work_dir() {
    return get_user_dir();
}

const char* get_tmp_dir() {
    const user_info_t* user_info = get_user_info();
    return user_info ? user_info->tmp_dir : NULL;
}

////

void print_sys_info() {

    const sys_info_t* sys_info = get_sys_info();
    
    //HARD TEST
    //free(sys_info);
    //sys_info = NULL;

    if (!sys_info) {
        // Error loading sys_info
        return;
    }
      
    printf("\n");
    
    // Version Info
    printf("os.name          : %s\n", lib_strsaf(sys_info->os_name));
    printf("os.version       : %s\n", lib_strsaf(sys_info->os_version));
    printf("os.major         : %d\n", sys_info->os_major_version);
    printf("os.minor         : %d\n", sys_info->os_minor_version);
    printf("os.build         : %d\n", sys_info->os_build_version);

    // CPU Info
    printf("os.arch          : %s\n", lib_strsaf(sys_info->os_arch));
    printf("os.arch.size     : %d\n", sys_info->os_arch_size);
    //printf("os.arch.data   : %s\n", lib_strsaf(sys_info->os_arch_data));
    
    printf("cpu.isalist      : %s\n", lib_strsaf(sys_info->cpu_isalist));
    printf("cpu.endian       : %s\n", lib_strsaf(sys_info->cpu_endian));

    // FS Info
    printf("file.separator   : %s\n", lib_strsaf(sys_info->file_separator));
    //printf("line.separator   : %s\n", lib_strsaf(sys_info->line_separator));
    printf("file.encoding    : %s\n", lib_strsaf(sys_info->encoding));

    // User Info
    printf("\n");
    printf("user.name        : %s\n", lib_strsaf(sys_info->user_name));
    printf("user.home        : %s\n", lib_strsaf(sys_info->user_home));
    printf("user.dir         : %s\n", lib_strsaf(sys_info->user_dir));
    printf("tmp.dir          : %s\n", lib_strsaf(sys_info->tmp_dir));
 
    printf("\n");
    printf("user.locale      : %s\n", lib_strsaf(sys_info->locale_type == LC_FORMAT_TYPE ? sys_info->format_locale : sys_info->display_locale)); 
    printf("user.language    : %s\n", lib_strsaf(sys_info->locale_type == LC_FORMAT_TYPE ? sys_info->format_language : sys_info->display_language));
    printf("user.script      : %s\n", lib_strsaf(sys_info->locale_type == LC_FORMAT_TYPE ? sys_info->format_script : sys_info->display_script));
    printf("user.country     : %s\n", lib_strsaf(sys_info->locale_type == LC_FORMAT_TYPE ? sys_info->format_country : sys_info->display_country));
    printf("user.variant     : %s\n", lib_strsaf(sys_info->locale_type == LC_FORMAT_TYPE ? sys_info->format_variant: sys_info->display_variant));
    printf("user.encoding    : %s\n", lib_strsaf(sys_info->locale_type == LC_FORMAT_TYPE ? sys_info->format_encoding: sys_info->display_encoding));

    printf("\n");
    printf("format.locale    : %s\n", lib_strsaf(sys_info->format_locale));
    printf("format.language  : %s\n", lib_strsaf(sys_info->format_language));
    printf("format.script    : %s\n", lib_strsaf(sys_info->format_script));
    printf("format.country   : %s\n", lib_strsaf(sys_info->format_country));
    printf("format.variant   : %s\n", lib_strsaf(sys_info->format_variant));
    printf("format.encoding  : %s\n", lib_strsaf(sys_info->format_encoding));

    printf("\n");
    printf("display.locale   : %s\n", lib_strsaf(sys_info->display_locale));    
    printf("display.language : %s\n", lib_strsaf(sys_info->display_language));
    printf("display.script   : %s\n", lib_strsaf(sys_info->display_script));
    printf("display.country  : %s\n", lib_strsaf(sys_info->display_country));
    printf("display.variant  : %s\n", lib_strsaf(sys_info->display_variant));
    printf("display.encoding : %s\n", lib_strsaf(sys_info->display_encoding));

    /*
    // Version Info
    printf("os.name        : %s\n", lib_strsaf(get_os_name()));
    printf("os.version     : %s\n", lib_strsaf(get_os_version()));
    printf("os.major       : %d\n", get_os_major_version());
    printf("os.minor       : %d\n", get_os_minor_version());
    printf("os.build       : %d\n", get_os_build_version());

    // CPU Info
    printf("os.arch        : %s\n", lib_strsaf(get_os_arch()));
    printf("os.arch.size     : %d\n", get_os_arch_size());
    //printf("os.arch.data   : %s\n", lib_strsaf(get_os_arch_data()));
    printf("cpu.isalist    : %s\n", lib_strsaf(get_cpu_isalist()));
    printf("cpu.endian     : %s\n", lib_strsaf(get_cpu_endian()));

    // FS Info
    printf("file.separator : %s\n", lib_strsaf(get_file_separator()));
    //printf("line.separator : %s\n", lib_strsaf(get_line_separator()));

    // User Info
    printf("user.name      : %s\n", lib_strsaf(get_user_name()));
    printf("user.home      : %s\n", lib_strsaf(get_user_home()));
    printf("user.dir       : %s\n", lib_strsaf(get_user_dir()));
    printf("tmp.dir        : %s\n", lib_strsaf(get_tmp_dir()));
    */    

}
