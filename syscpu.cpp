#include <string.h>

#include "syscpu.h"

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

////

int is_cpu_big_endian() {
    /* Endianness of platform */
    unsigned int endianTest = 0xff000000;
    return ((char*) (&endianTest))[0] != 0;
}

const char* get_cpu_endian() {
    return is_cpu_big_endian() ? "BE" : "LE";
}

int get_cpu_arch_size(const char* arch) {
    if (!arch) {
        return 0;
    }
    for (int i = 0; i < 37; i++) {
        const char* name = arch_data[i].name;
        if (strcmp(arch, name) == 0) {
            return arch_data[i].size;
        }
    }
    return 0;
}

const char* get_cpu_arch_data(const char* arch) {
    if (!arch) {
        return NULL;
    }
    int arch_size = get_cpu_arch_size(arch);

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


