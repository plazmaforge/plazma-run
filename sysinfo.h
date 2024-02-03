#ifndef PLAZMA_LIB_SYSINFO_OS_H
#define PLAZMA_LIB_SYSINFO_OS_H

struct sys_info_t {

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
    //const char* os_arch_data;       // cpu

    const char* cpu_isalist;        // list of supported instruction sets
    const char* cpu_endian;         // endianness of platform
    int cpu_processors;             // number of processors

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

const sys_info_t* get_sys_info();

// OS Info

const char* get_os_name();

const char* get_os_version();

int get_os_major_version();

int get_os_minor_version();

int get_os_build_version();

// CPU Info

const char* get_os_arch();

int get_os_arch_size();

//const char* get_os_arch_data();

const char* get_cpu_isalist();

// FS Info

const char* get_file_separator();

const char* get_line_separator();

// User Info

const char* get_user_name();

const char* get_user_home();

const char* get_user_dir();

const char* get_work_dir();

const char* get_tmp_dir();

////

void print_sys_info();

#endif // PLAZMA_LIB_SYSINFO_H