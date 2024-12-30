#ifndef PLAZMA_LIB_SYSCPU_H
#define PLAZMA_LIB_SYSCPU_H

#if defined _WIN32
#include <windows.h>
#endif

typedef enum {

    LIB_ARCH_NONE,
    LIB_ARCH_ALPHA,        /* Alpha       64 BE https://en.wikipedia.org/wiki/DEC_Alpha */
    LIB_ARCH_ARMV6L,       /* ARMv6       32 LE https://en.wikipedia.org/wiki/ARM_architecture */
    LIB_ARCH_ARMV7L,       /* ARMv7       32 LE https://en.wikipedia.org/wiki/ARM_architecture */
    LIB_ARCH_ARMV7B,       /* ARMv7       32 BE https://en.wikipedia.org/wiki/ARM_architecture */

    LIB_ARCH_AARCH64,      /* ARMv8       64 LE https://en.wikipedia.org/wiki/ARM_architecture */
    LIB_ARCH_CRIS,         /* ETRAX       32 LE https://en.wikipedia.org/wiki/ETRAX_CRIS */
    LIB_ARCH_I686,         /* x86         32 LE https://en.wikipedia.org/wiki/X86 */
    LIB_ARCH_ITANIUM,      /* Itanium     64 LE https://en.wikipedia.org/wiki/Itanium */
    LIB_ARCH_LM32,         /* MilkyMist   32 BE https://en.wikipedia.org/wiki/Milkymist */

    LIB_ARCH_M68K,         /* m68k        32 BE https://en.wikipedia.org/wiki/Motorola_68000_family */
    LIB_ARCH_MICROBLAZE,   /* Microblaze  32 BE https://en.wikipedia.org/wiki/MicroBlaze */
    LIB_ARCH_MICROBLAZEEL, /* Microblaze  32 LE https://en.wikipedia.org/wiki/MicroBlaze */
    LIB_ARCH_MIPS,         /* MIPS        32 BE https://en.wikipedia.org/wiki/MIPS_architecture */
    LIB_ARCH_MIPSEL,       /* MIPS        32 LE https://en.wikipedia.org/wiki/MIPS_architecture */

    LIB_ARCH_MIPS64,       /* MIPS        64 BE https://en.wikipedia.org/wiki/MIPS_architecture */
    LIB_ARCH_MIPS64EL,     /* MIPS        64 LE https://en.wikipedia.org/wiki/MIPS_architecture */
    LIB_ARCH_OR32,         /* OpenRisc    32 BE https://en.wikipedia.org/wiki/OpenRISC#QEMU_support */
    LIB_ARCH_PARISC,       /* PA-Risc     32 BE https://en.wikipedia.org/wiki/PA-RISC */
    LIB_ARCH_PARISC64,     /* PA-Risc     64 BE https://en.wikipedia.org/wiki/PA-RISC */

    LIB_ARCH_PPC,          /* PowerPC     32 BE https://en.wikipedia.org/wiki/PowerPC */
    LIB_ARCH_PPCLE,        /* PowerPC     32 LE https://en.wikipedia.org/wiki/PowerPC */
    LIB_ARCH_PPC64,        /* PowerPC     64 BE https://en.wikipedia.org/wiki/PowerPC */
    LIB_ARCH_PPC64LE,      /* PowerPC     64 LE https://en.wikipedia.org/wiki/PowerPC */
    LIB_ARCH_PPCEMB,       /* PowerPC     32 BE https://en.wikipedia.org/wiki/PowerPC */

    LIB_ARCH_RISCV32,      /* RISC-V      32 LE https://en.wikipedia.org/wiki/RISC-V */
    LIB_ARCH_RISCV64,      /* RISC-V      64 LE https://en.wikipedia.org/wiki/RISC-V */
    LIB_ARCH_S390,         /* S390        32 BE https://en.wikipedia.org/wiki/S390 */
    LIB_ARCH_S390X,        /* S390        64 BE https://en.wikipedia.org/wiki/S390x */
    LIB_ARCH_SH4,          /* SuperH4     32 LE https://en.wikipedia.org/wiki/SuperH */

    LIB_ARCH_SH4EB,        /* SuperH4     32 BE https://en.wikipedia.org/wiki/SuperH */
    LIB_ARCH_SPARC,        /* Sparc       32 BE https://en.wikipedia.org/wiki/Sparc */
    LIB_ARCH_SPARC64,      /* Sparc       64 BE https://en.wikipedia.org/wiki/Sparc */
    LIB_ARCH_UNICORE32,    /* UniCore     32 LE https://en.wikipedia.org/wiki/Unicore */
    LIB_ARCH_X86_64,       /* x86         64 LE https://en.wikipedia.org/wiki/X86 */

    LIB_ARCH_XTENSA,       /* XTensa      32 LE https://en.wikipedia.org/wiki/Xtensa#Processor_Cores */
    LIB_ARCH_XTENSAEB,     /* XTensa      32 BE https://en.wikipedia.org/wiki/Xtensa#Processor_Cores */

    LIB_ARCH_LAST,
    
} lib_arch_t;

#define LIB_ARCH_IS_X86(arch)  ((arch) == LIB_ARCH_X86_64 ||\
                            (arch) == LIB_ARCH_I686)

#define LIB_ARCH_IS_PPC(arch)  ((arch) == LIB_ARCH_PPC ||\
                            (arch) == LIB_ARCH_PPCLE ||\
                            (arch) == LIB_ARCH_PPC64 ||\
                            (arch) == LIB_ARCH_PPC64LE ||\
                            (arch) == LIB_ARCH_PPCEMB)

#define LIB_ARCH_IS_PPC64(arch)  ((arch) == LIB_ARCH_PPC64 ||\
                              (arch) == LIB_ARCH_PPC64LE)

#define LIB_ARCH_IS_ARM(arch)  ((arch) == LIB_ARCH_ARMV6L ||\
                            (arch) == LIB_ARCH_ARMV7L ||\
                            (arch) == LIB_ARCH_ARMV7B ||\
                            (arch) == LIB_ARCH_AARCH64)

#define LIB_ARCH_IS_RISCV(arch) ((arch) == LIB_ARCH_RISCV32 ||\
                             (arch) == LIB_ARCH_RISCV64)

#define LIB_ARCH_IS_S390(arch) ((arch) == LIB_ARCH_S390 ||\
                            (arch) == LIB_ARCH_S390X)

#define LIB_ARCH_IS_MIPS(arch) ((arch) == LIB_ARCH_MIPS ||\
                            (arch) == LIB_ARCH_MIPSEL ||\
                            (arch) == LIB_ARCH_MIPS64 ||\
                            (arch) == LIB_ARCH_MIPS64EL)

#define LIB_ARCH_IS_MIPS64(arch) ((arch) == LIB_ARCH_MIPS64 ||\
                              (arch) == LIB_ARCH_MIPS64EL)

#define LIB_ARCH_IS_SH4(arch) ((arch) == LIB_ARCH_SH4 ||\
                           (arch) == LIB_ARCH_SH4EB)

typedef enum {
    LIB_ARCH_LITTLE_ENDIAN,
    LIB_ARCH_BIG_ENDIAN,
} lib_arch_endian_t;

typedef struct lib_arch_data_t {
    const char *name;
    unsigned int size;
    lib_arch_endian_t endian;
} lib_arch_data_t;

static const lib_arch_data_t arch_data[] = {
    { "none",          0, LIB_ARCH_LITTLE_ENDIAN },

    { "alpha",        64, LIB_ARCH_BIG_ENDIAN },
    { "armv6l",       32, LIB_ARCH_LITTLE_ENDIAN },
    { "armv7l",       32, LIB_ARCH_LITTLE_ENDIAN },
    { "armv7b",       32, LIB_ARCH_BIG_ENDIAN },

    { "aarch64",      64, LIB_ARCH_LITTLE_ENDIAN },
    { "cris",         32, LIB_ARCH_LITTLE_ENDIAN },
    { "i686",         32, LIB_ARCH_LITTLE_ENDIAN },
    { "ia64",         64, LIB_ARCH_LITTLE_ENDIAN },
    { "lm32",         32, LIB_ARCH_BIG_ENDIAN },

    { "m68k",         32, LIB_ARCH_BIG_ENDIAN },
    { "microblaze",   32, LIB_ARCH_BIG_ENDIAN },
    { "microblazeel", 32, LIB_ARCH_LITTLE_ENDIAN},
    { "mips",         32, LIB_ARCH_BIG_ENDIAN },
    { "mipsel",       32, LIB_ARCH_LITTLE_ENDIAN },

    { "mips64",       64, LIB_ARCH_BIG_ENDIAN },
    { "mips64el",     64, LIB_ARCH_LITTLE_ENDIAN },
    { "openrisc",     32, LIB_ARCH_BIG_ENDIAN },
    { "parisc",       32, LIB_ARCH_BIG_ENDIAN },
    { "parisc64",     64, LIB_ARCH_BIG_ENDIAN },

    { "ppc",          32, LIB_ARCH_BIG_ENDIAN },
    { "ppcle",        32, LIB_ARCH_LITTLE_ENDIAN },
    { "ppc64",        64, LIB_ARCH_BIG_ENDIAN },
    { "ppc64le",      64, LIB_ARCH_LITTLE_ENDIAN },
    { "ppcemb",       32, LIB_ARCH_BIG_ENDIAN },

    { "riscv32",      32, LIB_ARCH_LITTLE_ENDIAN },
    { "riscv64",      64, LIB_ARCH_LITTLE_ENDIAN },
    { "s390",         32, LIB_ARCH_BIG_ENDIAN },
    { "s390x",        64, LIB_ARCH_BIG_ENDIAN },
    { "sh4",          32, LIB_ARCH_LITTLE_ENDIAN },

    { "sh4eb",        64, LIB_ARCH_BIG_ENDIAN },
    { "sparc",        32, LIB_ARCH_BIG_ENDIAN },
    { "sparc64",      64, LIB_ARCH_BIG_ENDIAN },
    { "unicore32",    32, LIB_ARCH_LITTLE_ENDIAN },
    { "x86_64",       64, LIB_ARCH_LITTLE_ENDIAN },

    { "xtensa",       32, LIB_ARCH_LITTLE_ENDIAN },
    { "xtensaeb",     32, LIB_ARCH_BIG_ENDIAN },
};

const char* lib_cpu_get_arch_name(lib_arch_t arch);

int lib_cpu_get_arch_size(lib_arch_t arch);

lib_arch_endian_t lib_cpu_get_arch_endian(lib_arch_t arch);

////

/**
 * Returns true if CPU is big endian 
 */
int lib_cpu_is_cpu_big_endian();

/**
 * Returns CPU endian: big/little
 */
const char* lib_cpu_get_cpu_endian();

const char* lib_cpu_get_cpu_issalist();

/**
 * Returns CPU Arch Type
 */
lib_arch_t lib_cpu_get_cpu_arch_type();

/**
 * Returns CPU Arch Name
 */
const char* lib_cpu_get_cpu_arch_name();

/**
 * Returns CPU count
 */
int lib_cpu_get_cpu_count();

////

const char* lib_cpu_get_cpu_issalist_by_machine(const char* machine);

lib_arch_t lib_cpu_get_cpu_arch_type_by_machine(const char* machine);

const char* lib_cpu_get_cpu_arch_name_by_machine(const char* machine);

/**
 * Returns CPU Arch Size by CPU Arch 
 */
int lib_cpu_get_cpu_arch_size(const char* arch_name);

/**
 * Returns CPU Arch Data Model by CPU Arch 
 */
const char* lib_cpu_get_cpu_arch_data_model(const char* arch_name);

#if defined _WIN32

const char* lib_cpu_get_cpu_isalist_by_si(SYSTEM_INFO* info);

lib_arch_t lib_cpu_get_cpu_arch_type_by_si(SYSTEM_INFO* info);

const char* lib_cpu_get_cpu_arch_name_by_si_2(SYSTEM_INFO* info);

const char* lib_cpu_get_cpu_arch_name_by_si(SYSTEM_INFO* info);

int lib_cpu_get_cpu_count_by_si(SYSTEM_INFO* info);

#endif

#endif // PLAZMA_LIB_SYSCPU_H
