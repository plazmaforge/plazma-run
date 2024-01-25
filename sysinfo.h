#ifndef PLAZMA_LIB_SYSINFO_OS_H
#define PLAZMA_LIB_SYSINFO_OS_H

struct sys_info_t {

    // OS Info
	char* os_name;
	char* os_version;
	char* os_release;         // *
    int os_major_version;
    int os_minor_version;
    int os_build_version;

    // Kernel Info
	char* version;            // kernel
    char* release;            // kernel

    // CPU Info
    char* os_arch;            // cpu
    char* os_arch_data;       // cpu
    const char* cpu_isalist;  // list of supported instruction sets
    char* cpu_endian;         // endianness of platform

    // Network Info
	char* node_name;          // network

    // FS Info
    char* file_separator;
    char* line_separator;

    // User Info
	char* user_name;
	char* user_home;
	char* user_dir;
    char* tmp_dir;

    // Current Locale Type (Format/Dysplay)
    int locale_type;
  
    // Format Locale Info
    char* format_locale;
    char* format_language;
    char* format_script;
    char* format_country;
    char* format_variant;
    char* format_encoding;

    // Display Locale Info
    char* display_locale;
    char* display_language;    
    char* display_script;    
    char* display_country;    
    char* display_variant;
    char* display_encoding;

    char* encoding;           // file_encoding
    char* stdout_encoding;
    char* stderr_encoding;
    char* unicode_encoding;   // The default endianness of unicode i.e. UnicodeBig or UnicodeLittle

};

sys_info_t* get_sys_info();

// OS Info

char* get_os_name();

char* get_os_version();

int get_os_major_version();

int get_os_minor_version();

int get_os_build_version();

char* get_os_arch();

char* get_os_arch_data();

// User Info

char* get_user_name();

char* get_user_home();

char* get_user_dir();

char* get_work_dir();

char* get_tmp_dir();

////

void print_sys_info();

#endif // PLAZMA_LIB_SYSINFO_H