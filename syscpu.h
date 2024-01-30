#ifndef PLAZMA_LIB_SYSCPU_H
#define PLAZMA_LIB_SYSCPU_H

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

typedef enum {
    ARCH_LITTLE_ENDIAN,
    ARCH_BIG_ENDIAN,
} arch_endian_t;

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

const char* get_arc_name(arch_t arch);

int get_arch_size(arch_t arch);

arch_endian_t get_arc_endian(arch_t arch);

////

/**
 * Returns true if CPU is big endian 
 */
int is_cpu_big_endian();

/**
 * Returns CPU endian: big/little
 */
const char* get_cpu_endian();

/**
 * Returns CPU Arch Size by CPU Arch 
 */
int get_cpu_arch_size(const char* arch);

/**
 * Returns CPU Arch Data by CPU Arch 
 */
const char* get_cpu_arch_data(const char* arch);

#endif // PLAZMA_LIB_SYSCPU_H
