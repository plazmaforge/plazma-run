#include <string.h>
#include "syscpu.h"

#if defined _WIN32
#include "syscpu_win.h"
#else
#include "syscpu_nix.h"
#endif

const char* lib_cpu_get_arch_name(arch_t arch) {
    if (arch >= ARCH_LAST)
        arch = ARCH_NONE;

    return arch_data[arch].name;
}

int lib_cpu_get_arch_size(arch_t arch) {
    if (arch >= ARCH_LAST)
        arch = ARCH_NONE;
    return arch_data[arch].size;
}

arch_endian_t lib_cpu_get_arch_endian(arch_t arch) {
    if (arch >= ARCH_LAST)
        arch = ARCH_NONE;

    return arch_data[arch].endian;
}

//// CPU Only

static arch_t lib_cpu_find_arch_by_name(const char *arch_name) {
    if (!arch_name) {
        return ARCH_NONE; // none
    }
    size_t i;
    for (i = 1; i < ARCH_LAST; i++) {
        if (strcmp(arch_data[i].name, arch_name) == 0) {
            return (arch_t) i;
        }
    }
    return ARCH_NONE; // none
}

static arch_data_t lib_cpu_find_arch_data_by_name(const char *arch_name) {
    if (!arch_name) {
        return arch_data[0]; // none
    }
    arch_t arch = lib_cpu_find_arch_by_name(arch_name);
    return arch_data[arch];
}

//// CPU Only

int lib_cpu_is_cpu_big_endian() {
    /* Endianness of platform */
    unsigned int endianTest = 0xff000000;
    return ((char*) (&endianTest))[0] != 0;
}

const char* lib_cpu_get_cpu_endian() {
    return lib_cpu_is_cpu_big_endian() ? "BE" : "LE";
}

const char* lib_cpu_get_cpu_arch_name() {
    arch_t arch = lib_cpu_get_cpu_arch_type(); // OS Depending
    return lib_cpu_get_arch_name(arch);
}

////

const char* get_cpu_issalist_by_machine(const char* machine) {
    if (!machine) {
        return "";
    }
    if (strcmp(machine, "amd64") == 0) {
        return "x86_64";    /* amd64  -> x86_64  */
    } else if (strcmp(machine, "ia64") == 0) {
        return "ia64";      /* WHY?  */
    }
    return machine;         /* WHAT? */
}

arch_t get_cpu_arch_type_by_machine(const char* machine) {
    if (!machine) {
        return ARCH_NONE;
    }

    /* Some special cases we need to handle first
     * for non-canonical names */
    
    if (strlen(machine) == 4 &&
        machine[0] == 'i' &&
        machine[2] == '8' &&
        machine[3] == '6' &&
        machine[4] == '\0') {
        return ARCH_I686;      /* i<n>86 -> i686    */
    } else if (strcmp(machine, "amd64") == 0) {
        return ARCH_X86_64;    /* amd64  -> x86_64  */
    } else if (strcmp(machine, "arm64") == 0) {
        return ARCH_AARCH64;   /* arm64  -> aarch64 */
    }

    /* Otherwise assume the canonical name          */
    return lib_cpu_find_arch_by_name(machine);        
}

const char* get_cpu_arch_name_by_machine(const char* machine) {
    if (!machine) {
        return NULL;
    }
    // Try find an arch type by machine
    arch_t arch = get_cpu_arch_type_by_machine(machine);

    // If the arch type is not found return original machine name
    return arch == ARCH_NONE ? machine : lib_cpu_get_arch_name(arch);
}

int lib_cpu_get_cpu_arch_size(const char* arch_name) {
    if (!arch_name) {
        return 0;
    }
    arch_data_t data = lib_cpu_find_arch_data_by_name(arch_name);
    return data.size;
}

const char* lib_cpu_get_cpu_arch_data_model(const char* arch_name) {
    if (!arch_name) {
        return NULL;
    }
    int arch_size = lib_cpu_get_cpu_arch_size(arch_name);

    if (arch_size == 16) {
        return "16"; // WHAT?
    } else  if (arch_size == 32) {
        return "32";
    } else if (arch_size == 64) {
        return "64";
    } else if (arch_size == 128) {
        return "128"; // WHAT?
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

    return NULL;
}


////

// deprecated
// by definition
const char* get_cpu_arch_data_model_cc() {
   #if defined(_M_AMD64)
   return "x86_64"; //"amd64";
   #elif defined(_X86_)
   return "x86";
   #elif defined(_M_ARM64)
   return "aarch64";
   #else
   return NULL;
   #endif
}

// deprecated
// by definition
const char* get_cpu_arch_cc() {
   #if defined(_M_AMD64)
   return "64";
   #elif defined(_X86_)
   return "32";
   #elif defined(_M_ARM64)
   return "64";
   #else
   return NULL;
   #endif
}
