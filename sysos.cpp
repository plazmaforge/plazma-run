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

typedef enum {
    ARCH_NONE,
    ARCH_ALPHA,        /* Alpha       64 BE https://en.wikipedia.org/wiki/DEC_Alpha */
    ARCH_ARMV6L,       /* ARMv6       32 LE https://en.wikipedia.org/wiki/ARM_architecture */
    ARCH_ARMV7L,       /* ARMv7       32 LE https://en.wikipedia.org/wiki/ARM_architecture */
    ARCH_ARMV7B,       /* ARMv7       32 BE https://en.wikipedia.org/wiki/ARM_architecture */

    ARCH_AARCH64,      /* ARMv8       64 LE https://en.wikipedia.org/wiki/ARM_architecture */
    ARCH_CRIS,         /* ETRAX       32 LE https://en.wikipedia.org/wiki/ETRAX_CRIS */
    ARCH_I686,         /* x86         32 LE https://en.wikipedia.org/wiki/X86 */
    ARCH_ITANIUM,      /* Itanium     64 LE https://en.wikipedia.org/wiki/Itanium */
    ARCH_LM32,         /* MilkyMist   32 BE https://en.wikipedia.org/wiki/Milkymist */

    ARCH_M68K,         /* m68k        32 BE https://en.wikipedia.org/wiki/Motorola_68000_family */
    ARCH_MICROBLAZE,   /* Microblaze  32 BE https://en.wikipedia.org/wiki/MicroBlaze */
    ARCH_MICROBLAZEEL, /* Microblaze  32 LE https://en.wikipedia.org/wiki/MicroBlaze */
    ARCH_MIPS,         /* MIPS        32 BE https://en.wikipedia.org/wiki/MIPS_architecture */
    ARCH_MIPSEL,       /* MIPS        32 LE https://en.wikipedia.org/wiki/MIPS_architecture */

    ARCH_MIPS64,       /* MIPS        64 BE https://en.wikipedia.org/wiki/MIPS_architecture */
    ARCH_MIPS64EL,     /* MIPS        64 LE https://en.wikipedia.org/wiki/MIPS_architecture */
    ARCH_OR32,         /* OpenRisc    32 BE https://en.wikipedia.org/wiki/OpenRISC#QEMU_support */
    ARCH_PARISC,       /* PA-Risc     32 BE https://en.wikipedia.org/wiki/PA-RISC */
    ARCH_PARISC64,     /* PA-Risc     64 BE https://en.wikipedia.org/wiki/PA-RISC */

    ARCH_PPC,          /* PowerPC     32 BE https://en.wikipedia.org/wiki/PowerPC */
    ARCH_PPCLE,        /* PowerPC     32 LE https://en.wikipedia.org/wiki/PowerPC */
    ARCH_PPC64,        /* PowerPC     64 BE https://en.wikipedia.org/wiki/PowerPC */
    ARCH_PPC64LE,      /* PowerPC     64 LE https://en.wikipedia.org/wiki/PowerPC */
    ARCH_PPCEMB,       /* PowerPC     32 BE https://en.wikipedia.org/wiki/PowerPC */

    ARCH_RISCV32,      /* RISC-V      32 LE https://en.wikipedia.org/wiki/RISC-V */
    ARCH_RISCV64,      /* RISC-V      64 LE https://en.wikipedia.org/wiki/RISC-V */
    ARCH_S390,         /* S390        32 BE https://en.wikipedia.org/wiki/S390 */
    ARCH_S390X,        /* S390        64 BE https://en.wikipedia.org/wiki/S390x */
    ARCH_SH4,          /* SuperH4     32 LE https://en.wikipedia.org/wiki/SuperH */

    ARCH_SH4EB,        /* SuperH4     32 BE https://en.wikipedia.org/wiki/SuperH */
    ARCH_SPARC,        /* Sparc       32 BE https://en.wikipedia.org/wiki/Sparc */
    ARCH_SPARC64,      /* Sparc       64 BE https://en.wikipedia.org/wiki/Sparc */
    ARCH_UNICORE32,    /* UniCore     32 LE https://en.wikipedia.org/wiki/Unicore */
    ARCH_X86_64,       /* x86         64 LE https://en.wikipedia.org/wiki/X86 */

    ARCH_XTENSA,       /* XTensa      32 LE https://en.wikipedia.org/wiki/Xtensa#Processor_Cores */
    ARCH_XTENSAEB,     /* XTensa      32 BE https://en.wikipedia.org/wiki/Xtensa#Processor_Cores */

    ARCH_LAST,
} arch_t;

#define ARCH_IS_X86(arch)  ((arch) == ARCH_X86_64 ||\
                            (arch) == ARCH_I686)

#define ARCH_IS_PPC(arch)  ((arch) == ARCH_PPC ||\
                            (arch) == ARCH_PPCLE ||\
                            (arch) == ARCH_PPC64 ||\
                            (arch) == ARCH_PPC64LE ||\
                            (arch) == ARCH_PPCEMB)

#define ARCH_IS_PPC64(arch)  ((arch) == ARCH_PPC64 ||\
                              (arch) == ARCH_PPC64LE)

#define ARCH_IS_ARM(arch)  ((arch) == ARCH_ARMV6L ||\
                            (arch) == ARCH_ARMV7L ||\
                            (arch) == ARCH_ARMV7B ||\
                            (arch) == ARCH_AARCH64)

#define ARCH_IS_RISCV(arch) ((arch) == ARCH_RISCV32 ||\
                             (arch) == ARCH_RISCV64)

#define ARCH_IS_S390(arch) ((arch) == ARCH_S390 ||\
                            (arch) == ARCH_S390X)

#define ARCH_IS_MIPS(arch) ((arch) == ARCH_MIPS ||\
                            (arch) == ARCH_MIPSEL ||\
                            (arch) == ARCH_MIPS64 ||\
                            (arch) == ARCH_MIPS64EL)

#define ARCH_IS_MIPS64(arch) ((arch) == ARCH_MIPS64 ||\
                              (arch) == ARCH_MIPS64EL)

#define ARCH_IS_SH4(arch) ((arch) == ARCH_SH4 ||\
                           (arch) == ARCH_SH4EB)
          
static const struct arch_data_t {
    const char *name;
    unsigned int size;
    arch_endian_t endian;
} arch_data[] = {
    { "none",          0, ARCH_LITTLE_ENDIAN },
    { "alpha",        64, ARCH_BIG_ENDIAN },
    { "armv6l",       32, ARCH_LITTLE_ENDIAN },
    { "armv7l",       32, ARCH_LITTLE_ENDIAN },
    { "armv7b",       32, ARCH_BIG_ENDIAN },

    { "aarch64",      64, ARCH_LITTLE_ENDIAN },
    { "cris",         32, ARCH_LITTLE_ENDIAN },
    { "i686",         32, ARCH_LITTLE_ENDIAN },
    { "ia64",         64, ARCH_LITTLE_ENDIAN },
    { "lm32",         32, ARCH_BIG_ENDIAN },

    { "m68k",         32, ARCH_BIG_ENDIAN },
    { "microblaze",   32, ARCH_BIG_ENDIAN },
    { "microblazeel", 32, ARCH_LITTLE_ENDIAN},
    { "mips",         32, ARCH_BIG_ENDIAN },
    { "mipsel",       32, ARCH_LITTLE_ENDIAN },

    { "mips64",       64, ARCH_BIG_ENDIAN },
    { "mips64el",     64, ARCH_LITTLE_ENDIAN },
    { "openrisc",     32, ARCH_BIG_ENDIAN },
    { "parisc",       32, ARCH_BIG_ENDIAN },
    { "parisc64",     64, ARCH_BIG_ENDIAN },

    { "ppc",          32, ARCH_BIG_ENDIAN },
    { "ppcle",        32, ARCH_LITTLE_ENDIAN },
    { "ppc64",        64, ARCH_BIG_ENDIAN },
    { "ppc64le",      64, ARCH_LITTLE_ENDIAN },
    { "ppcemb",       32, ARCH_BIG_ENDIAN },

    { "riscv32",      32, ARCH_LITTLE_ENDIAN },
    { "riscv64",      64, ARCH_LITTLE_ENDIAN },
    { "s390",         32, ARCH_BIG_ENDIAN },
    { "s390x",        64, ARCH_BIG_ENDIAN },
    { "sh4",          32, ARCH_LITTLE_ENDIAN },

    { "sh4eb",        64, ARCH_BIG_ENDIAN },
    { "sparc",        32, ARCH_BIG_ENDIAN },
    { "sparc64",      64, ARCH_BIG_ENDIAN },
    { "unicore32",    32, ARCH_LITTLE_ENDIAN },
    { "x86_64",       64, ARCH_LITTLE_ENDIAN },

    { "xtensa",       32, ARCH_LITTLE_ENDIAN },
    { "xtensaeb",     32, ARCH_BIG_ENDIAN },
};

int is_cpu_big_endian() {
    /* Endianness of platform */
    unsigned int endianTest = 0xff000000;
    return ((char*) (&endianTest))[0] != 0;
}

const char* get_cpu_endian() {
    return is_cpu_big_endian() ? "BE" : "LE";
}

int get_os_arch_size(const char* os_arch) {
    if (!os_arch) {
        return 0;
    }
    for (int i = 0; i < 37; i++) {
        const char* name = arch_data[i].name;
        if (strcmp(os_arch, name) == 0) {
            return arch_data[i].size;
        }
    }
    return 0;
}

const char* get_os_arch_data(const char* os_arch) {
    if (!os_arch) {
        return NULL;
    }
    int arch_size = get_os_arch_size(os_arch);

    // TODO: convert int to str
    if (arch_size == 32) {
        return "32";
    }
    if (arch_size == 64) {
        return "64";
    }

    /*
    $ clang -print-targets

    Registered Targets:
      aarch64    - AArch64 (little endian)
      aarch64_32 - AArch64 (little endian ILP32)
      aarch64_be - AArch64 (big endian)
      amdgcn     - AMD GCN GPUs
      arm        - ARM
      arm64      - ARM64 (little endian)
      arm64_32   - ARM64 (little endian ILP32)
      armeb      - ARM (big endian)
      avr        - Atmel AVR Microcontroller
      bpf        - BPF (host endian)
      bpfeb      - BPF (big endian)
      bpfel      - BPF (little endian)
      hexagon    - Hexagon
      lanai      - Lanai
      mips       - MIPS (32-bit big endian)
      mips64     - MIPS (64-bit big endian)
      mips64el   - MIPS (64-bit little endian)
      mipsel     - MIPS (32-bit little endian)
      msp430     - MSP430 [experimental]
      nvptx      - NVIDIA PTX 32-bit
      nvptx64    - NVIDIA PTX 64-bit
      ppc32      - PowerPC 32
      ppc64      - PowerPC 64
      ppc64le    - PowerPC 64 LE
      r600       - AMD GPUs HD2XXX-HD6XXX
      riscv32    - 32-bit RISC-V
      riscv64    - 64-bit RISC-V
      sparc      - Sparc
      sparcel    - Sparc LE
      sparcv9    - Sparc V9
      systemz    - SystemZ
      thumb      - Thumb
      thumbeb    - Thumb (big endian)
      wasm32     - WebAssembly 32-bit
      wasm64     - WebAssembly 64-bit
      x86        - 32-bit X86: Pentium-Pro and above
      x86-64     - 64-bit X86: EM64T and AMD64
      xcore      - XCore
    */

    /*
    // 64
    if (strcmp(os_arch, "x86_64") == 0         
        || strcmp(os_arch, "amd64") == 0
        || strcmp(os_arch, "amd64_32") == 0
        || strcmp(os_arch, "aarch64") == 0
        || strcmp(os_arch, "aarch64_32") == 0
        || strcmp(os_arch, "aarch64_be") == 0
        || strcmp(os_arch, "arm64") == 0
        || strcmp(os_arch, "arm64_32") == 0
        || strcmp(os_arch, "mips64") == 0
        || strcmp(os_arch, "mips64el") == 0
        || strcmp(os_arch, "nvptx64") == 0
        || strcmp(os_arch, "ppc64") == 0
        || strcmp(os_arch, "ppc64le") == 0
        || strcmp(os_arch, "riscv64") == 0
        || strcmp(os_arch, "wasm64") == 0) {
        return "64";
    }

    // 32
    if (strcmp(os_arch, "x86") == 0
        || strcmp(os_arch, "mips") == 0
        || strcmp(os_arch, "mipsel") == 0
        || strcmp(os_arch, "nvptx") == 0
        || strcmp(os_arch, "ppc32") == 0
        || strcmp(os_arch, "riscv32") == 0
        || strcmp(os_arch, "wasm32") == 0) {
        return "32";
    }
    */

    return NULL;
}

const char* get_arc_name(arch_t arch) {
    if (arch >= ARCH_LAST)
        arch = ARCH_NONE;

    return arch_data[arch].name;
}

int get_arch_size(arch_t arch) {
    if (arch >= ARCH_LAST)
        arch = ARCH_NONE;
    return arch_data[arch].size;
}

arch_endian_t get_arc_endian(arch_t arch) {
    if (arch >= ARCH_LAST)
        arch = ARCH_NONE;

    return arch_data[arch].endian;
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
    os_info->os_arch_data = NULL;
    os_info->cpu_isalist = NULL;
    os_info->cpu_endian = NULL;

    // Network Info
	os_info->node_name = NULL;

    // FS Info
    os_info->file_separator = NULL;
    os_info->line_separator = NULL;

}

/*
static os_info_t* new_os_info() {
    os_info_t* os_info = (os_info_t*) malloc(sizeof(os_info_t));

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
    os_info->os_arch_data = NULL;
    os_info->cpu_isalist = NULL;
    os_info->cpu_endian = NULL;

    // Network Info
	os_info->node_name = NULL;

    // FS Info
    os_info->file_separator = NULL;
    os_info->line_separator = NULL;

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
    free(os_info->os_arch_data);
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

    //os_info = new_os_info();
    //if (!os_info) {
    //    // Allocation error
    //    return NULL;
    //}

    load_os_info(os_info);
    return os_info;
}
