#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include "sysos.h"
#include "sysuser.h"
#include "sysinfo.h"
#include "syslocale_os.h"
#include "strlib.h"

int LC_FORMAT_TYPE = LC_CTYPE;
#ifdef LC_MESSAGES
int LC_DISPLAY_TYPE = LC_MESSAGES;
#else
int LC_DISPLAY_TYPE = LC_TIME + 1;
#endif

static lib_sys_info_t* sys_info = NULL;

static lib_sys_info_t* lib_sys_info_init(lib_sys_info_t* sys_info) {
    
    // Version Info
    sys_info->os_major_version = 0;
    sys_info->os_minor_version = 0;
    sys_info->os_build_version = 0;
    sys_info->version = NULL;
    sys_info->release = NULL;

    // CPU Info
    sys_info->os_arch = NULL;
    sys_info->os_arch_size = 0;
    sys_info->cpu_isalist = NULL;
    sys_info->cpu_endian = NULL;
    sys_info->cpu_count = 0;

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
static void lib_sys_info_free(lib_sys_info_t* sys_info) {
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
static lib_sys_info_t* lib_sys_info_new() {
    lib_sys_info_t* sys_info = (lib_sys_info_t*) malloc(sizeof(lib_sys_info_t));
    init_sys_info(sys_info);
    return sys_info;
}
*/

const lib_sys_info_t* lib_sys_get_sys_info() {

    if (sys_info) {
        return sys_info;
    }
    //sys_info = lib_sys_sys_info_new();
    static lib_sys_info_t sys_info_s;
    sys_info = &sys_info_s;
    lib_sys_info_init(sys_info);

    // OS Info
    const lib_os_info_t* os_info = lib_sys_get_os_info();
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
        sys_info->cpu_isalist = os_info->cpu_isalist;
        sys_info->cpu_endian = os_info->cpu_endian;
        sys_info->cpu_count = os_info->cpu_count;

        // FS Info
        sys_info->file_separator = os_info->file_separator;
        sys_info->line_separator = os_info->line_separator;
    }

    // User Info
    const lib_user_info_t* user_info = lib_sys_get_user_info();
    if (user_info) {
        sys_info->user_name = user_info->user_name;
        sys_info->user_home = user_info->user_home;
        sys_info->user_dir = user_info->user_dir;
        sys_info->tmp_dir = user_info->tmp_dir;
    }

    sys_info->locale_type = 0;
  
    // Format Locale Info  
    lib_locale_t* format_locale = lib_sys_load_locale_os(LC_FORMAT_TYPE);
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
    lib_locale_t* display_locale = lib_sys_load_locale_os(LC_DISPLAY_TYPE);
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

    lib_sys_locale_free(format_locale);
    lib_sys_locale_free(display_locale);

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

const char* lib_sys_get_os_name() {
    const lib_os_info_t* os_info = lib_sys_get_os_info();
    return os_info ? os_info->os_name : NULL;
}

const char* lib_sys_get_os_version() {
    const lib_os_info_t* os_info = lib_sys_get_os_info();
    return os_info ? os_info->os_version : NULL;
}

int lib_sys_get_os_major_version() {
    const lib_os_info_t* os_info = lib_sys_get_os_info();
    return os_info ? os_info->os_major_version : 0;
}

int lib_sys_get_os_minor_version() {
    const lib_os_info_t* os_info = lib_sys_get_os_info();
    return os_info ? os_info->os_minor_version : 0;
}

int lib_sys_get_os_build_version() {
    const lib_os_info_t* os_info = lib_sys_get_os_info();
    return os_info ? os_info->os_build_version : 0;
}

// CPU Info

const char* lib_sys_get_os_arch() {
    const lib_os_info_t* os_info = lib_sys_get_os_info();
    return os_info ? os_info->os_arch : NULL;
}

int lib_sys_get_os_arch_size() {
    const lib_os_info_t* os_info = lib_sys_get_os_info();
    return lib_sys_get_os_arch_size(os_info->os_arch);
}

const char* lib_sys_get_cpu_isalist() {
    const lib_os_info_t* os_info = lib_sys_get_os_info();
    return os_info ? os_info->cpu_isalist : NULL;
}

// const char* lib_sys_get_cpu_endian() {
//     return get_cpu_endian(); // lib_cpu
// }

int lib_sys_get_cpu_count() {
    const lib_os_info_t* os_info = lib_sys_get_os_info();
    return os_info ? os_info->cpu_count : 0;
}

// FS Info

const char* lib_sys_get_file_separator() {
    const lib_os_info_t* os_info = lib_sys_get_os_info();
    return os_info ? os_info->file_separator : NULL;
}

const char* lib_sys_get_line_separator() {
    const lib_os_info_t* os_info = lib_sys_get_os_info();
    return os_info ? os_info->line_separator : NULL;
}

const char* lib_sys_get_file_encoding() {
    return lib_sys_get_encoding();
}

const char* lib_sys_get_encoding() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    return sys_info ? sys_info->encoding : NULL;
}

// User Info

const char* lib_sys_get_user_name() {
    const lib_user_info_t* user_info = lib_sys_get_user_info();
    return user_info ? user_info->user_name : NULL;
}

const char* lib_sys_get_user_home() {
    const lib_user_info_t* user_info = lib_sys_get_user_info();
    return user_info ? user_info->user_home : NULL;
}

const char* lib_sys_get_user_dir() {
    const lib_user_info_t* user_info = lib_sys_get_user_info();
    return user_info ? user_info->user_dir : NULL;
}

const char* lib_sys_get_work_dir() {
    return lib_sys_get_user_dir();
}

const char* lib_sys_get_tmp_dir() {
    const lib_user_info_t* user_info = lib_sys_get_user_info();
    return user_info ? user_info->tmp_dir : NULL;
}

// Locale Info

const char* lib_sys_get_locale_name() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    if (!sys_info) {
        return NULL;
    }
    return sys_info->locale_type == LC_FORMAT_TYPE ? sys_info->format_locale : sys_info->display_locale;
}

const char* lib_sys_get_locale_language() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    if (!sys_info) {
        return NULL;
    }
    return sys_info->locale_type == LC_FORMAT_TYPE ? sys_info->format_language : sys_info->display_language;
}

const char* lib_sys_get_locale_script() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    if (!sys_info) {
        return NULL;
    }
    return sys_info->locale_type == LC_FORMAT_TYPE ? sys_info->format_script : sys_info->display_script;
}

const char* lib_sys_get_locale_country() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    if (!sys_info) {
        return NULL;
    }
    return sys_info->locale_type == LC_FORMAT_TYPE ? sys_info->format_country : sys_info->display_country;
}

const char* lib_sys_get_locale_variant() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    if (!sys_info) {
        return NULL;
    }
    return sys_info->locale_type == LC_FORMAT_TYPE ? sys_info->format_variant : sys_info->display_variant;
}

const char* lib_sys_get_locale_encoding() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    if (!sys_info) {
        return NULL;
    }
    return sys_info->locale_type == LC_FORMAT_TYPE ? sys_info->format_encoding : sys_info->display_encoding;
}

// Format Locale Info

const char* lib_sys_get_format_locale_name() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    return sys_info ? sys_info->format_locale : NULL;
}

const char* lib_sys_get_format_locale_language() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    return sys_info ? sys_info->format_language : NULL;
}

const char* lib_sys_get_format_locale_script() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    return sys_info ? sys_info->format_script : NULL;
}

const char* lib_sys_get_format_locale_country() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    return sys_info ? sys_info->format_country : NULL;
}

const char* lib_sys_get_format_locale_variant() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    return sys_info ? sys_info->format_variant : NULL;
}

const char* lib_sys_get_format_locale_encoding() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    return sys_info ? sys_info->format_encoding : NULL;
}

// Display Locale Info

const char* lib_sys_get_display_locale_name() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    return sys_info ? sys_info->display_locale : NULL;
}

const char* lib_sys_get_display_locale_language() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    return sys_info ? sys_info->display_language : NULL;
}

const char* lib_sys_get_display_locale_script() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    return sys_info ? sys_info->display_script : NULL;
}

const char* lib_sys_get_display_locale_country() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    return sys_info ? sys_info->display_country : NULL;
}

const char* lib_sys_get_display_locale_variant() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    return sys_info ? sys_info->display_variant : NULL;
}

const char* lib_sys_get_display_locale_encoding() {
    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    return sys_info ? sys_info->display_encoding : NULL;
}

////

void lib_sys_print_sys_info() {

    const lib_sys_info_t* sys_info = lib_sys_get_sys_info();
    
    //HARD TEST
    //free(sys_info);
    //sys_info = NULL;

    if (!sys_info) {
        // Error loading sys_info
        return;
    }
      
    /*
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
    
    printf("cpu.isalist      : %s\n", lib_strsaf(sys_info->cpu_isalist));
    printf("cpu.endian       : %s\n", lib_strsaf(sys_info->cpu_endian));
    printf("cpu.count        : %d\n", sys_info->cpu_count);

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

    */

    printf("\n");

    // Version Info
    printf("os.name          : %s\n", lib_strsaf(lib_sys_get_os_name()));
    printf("os.version       : %s\n", lib_strsaf(lib_sys_get_os_version()));
    printf("os.major         : %d\n", lib_sys_get_os_major_version());
    printf("os.minor         : %d\n", lib_sys_get_os_minor_version());
    printf("os.build         : %d\n", lib_sys_get_os_build_version());

    // CPU Info
    printf("os.arch          : %s\n", lib_strsaf(lib_sys_get_os_arch()));
    printf("os.arch.size     : %d\n", lib_sys_get_os_arch_size());
    printf("cpu.isalist      : %s\n", lib_strsaf(lib_sys_get_cpu_isalist()));
    printf("cpu.endian       : %s\n", lib_strsaf(lib_sys_get_cpu_endian()));
    printf("cpu.count        : %d\n", lib_sys_get_cpu_count());

    // FS Info
    printf("file.separator   : %s\n", lib_strsaf(lib_sys_get_file_separator()));
    //printf("line.separator   : %s\n", lib_strsaf(lib_sys_get_line_separator()));
    printf("file.encoding    : %s\n", lib_strsaf(lib_sys_get_file_encoding()));

    // User Info
    printf("\n");
    printf("user.name        : %s\n", lib_strsaf(lib_sys_get_user_name()));
    printf("user.home        : %s\n", lib_strsaf(lib_sys_get_user_home()));
    printf("user.dir         : %s\n", lib_strsaf(lib_sys_get_user_dir()));
    printf("tmp.dir          : %s\n", lib_strsaf(lib_sys_get_tmp_dir()));

    printf("\n");
    printf("user.locale      : %s\n", lib_strsaf(lib_sys_get_locale_name())); 
    printf("user.language    : %s\n", lib_strsaf(lib_sys_get_locale_language()));
    printf("user.script      : %s\n", lib_strsaf(lib_sys_get_locale_script()));
    printf("user.country     : %s\n", lib_strsaf(lib_sys_get_locale_country()));
    printf("user.variant     : %s\n", lib_strsaf(lib_sys_get_locale_variant()));
    printf("user.encoding    : %s\n", lib_strsaf(lib_sys_get_locale_encoding()));

    printf("\n");
    printf("format.locale    : %s\n", lib_strsaf(lib_sys_get_format_locale_name()));
    printf("format.language  : %s\n", lib_strsaf(lib_sys_get_format_locale_language()));
    printf("format.script    : %s\n", lib_strsaf(lib_sys_get_format_locale_script()));
    printf("format.country   : %s\n", lib_strsaf(lib_sys_get_format_locale_country()));
    printf("format.variant   : %s\n", lib_strsaf(lib_sys_get_format_locale_variant()));
    printf("format.encoding  : %s\n", lib_strsaf(lib_sys_get_format_locale_encoding()));

    printf("\n");
    printf("display.locale   : %s\n", lib_strsaf(lib_sys_get_display_locale_name()));
    printf("display.language : %s\n", lib_strsaf(lib_sys_get_display_locale_language()));
    printf("display.script   : %s\n", lib_strsaf(lib_sys_get_display_locale_script()));
    printf("display.country  : %s\n", lib_strsaf(lib_sys_get_display_locale_country()));
    printf("display.variant  : %s\n", lib_strsaf(lib_sys_get_display_locale_variant()));
    printf("display.encoding : %s\n", lib_strsaf(lib_sys_get_display_locale_encoding()));

}
