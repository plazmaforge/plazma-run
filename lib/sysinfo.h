#ifndef PLAZMA_LIB_SYSINFO_OS_H
#define PLAZMA_LIB_SYSINFO_OS_H

struct lib_sys_info_t {

    // OS Info
    const char* os_name;
    const char* os_version;
    const char* os_release;         // *
    int os_major_version;
    int os_minor_version;
    int os_build_version;

    // Kernel Info
    const char* version;            // kernel
    const char* release;            // kernel

    // CPU Info
    const char* os_arch;            // cpu
    int os_arch_size;               // cpu

    const char* cpu_isalist;        // list of supported instruction sets
    const char* cpu_endian;         // endianness of platform
    int cpu_count;                  // number of processors

    // Network Info
    const char* node_name;          // network

    // FS Info
    const char* file_separator;
    const char* line_separator;

    // User Info
    const char* user_name;
    const char* user_home;
    const char* user_dir;
    const char* tmp_dir;

    // Current Locale Type (Format/Dysplay)
    int locale_type;
  
    // Format Locale Info
    const char* format_locale;
    const char* format_language;
    const char* format_script;
    const char* format_country;
    const char* format_variant;
    const char* format_encoding;

    // Display Locale Info
    const char* display_locale;
    const char* display_language;    
    const char* display_script;    
    const char* display_country;    
    const char* display_variant;
    const char* display_encoding;

    const char* encoding;           // file_encoding
    const char* stdout_encoding;
    const char* stderr_encoding;
    const char* unicode_encoding;   // The default endianness of unicode i.e. UnicodeBig or UnicodeLittle

};

const lib_sys_info_t* lib_sys_get_sys_info();

// OS Info

const char* lib_sys_get_os_name();

const char* lib_sys_get_os_version();

int lib_sys_get_os_major_version();

int lib_sys_get_os_minor_version();

int lib_sys_get_os_build_version();

// CPU Info

const char* lib_sys_get_os_arch();

int lib_sys_get_os_arch_size();

const char* lib_sys_get_cpu_isalist();

const char* lib_sys_get_cpu_endian();

int lib_sys_get_cpu_count();

// FS Info

const char* lib_sys_get_file_separator();

const char* lib_sys_get_line_separator();

const char* lib_sys_get_file_encoding();

const char* lib_sys_get_encoding();

// User Info

const char* lib_sys_get_user_name();

const char* lib_sys_get_user_home();

const char* lib_sys_get_user_dir();

const char* lib_sys_get_work_dir();

const char* lib_sys_get_tmp_dir();

// Locale Info

const char* lib_sys_get_locale_name();

const char* lib_sys_get_locale_language();

const char* lib_sys_get_locale_script();

const char* lib_sys_get_locale_country();

const char* lib_sys_get_locale_variant();

const char* lib_sys_get_locale_encoding();

// Format Locale Info

const char* lib_sys_get_format_locale_name();

const char* lib_sys_get_format_locale_language();

const char* lib_sys_get_format_locale_script();

const char* lib_sys_get_format_locale_country();

const char* lib_sys_get_format_locale_variant();

const char* lib_sys_get_format_locale_encoding();

// Display Locale Info

const char* lib_sys_get_display_locale_name();

const char* lib_sys_get_display_locale_language();

const char* lib_sys_get_display_locale_script();

const char* lib_sys_get_display_locale_country();

const char* lib_sys_get_display_locale_variant();

const char* lib_sys_get_display_locale_encoding();

////

void lib_sys_print_sys_info();

#endif // PLAZMA_LIB_SYSINFO_H