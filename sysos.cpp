#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sysos.h"

#if defined _WIN32
#include "sysos_win.h"
#elif defined __APPLE__ && defined __MACH__
#include "sysos_mac.h"
#else
#include "sysos_nix.h"
#endif

int is_cpu_big_endian() {
    unsigned int endianTest = 0xff000000;
    return ((char*) (&endianTest))[0] != 0;
}

const char* get_cpu_endian() {
    unsigned int endianTest = 0xff000000;
    return is_cpu_big_endian() ? "big" : "little";
}

const char* get_os_arch_data(const char* os_arch) {
    if (!os_arch) {
        return NULL;
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

    return NULL;
}

os_info_t* new_os_info() {
    os_info_t* os_info = (os_info_t*) malloc(sizeof(os_info_t));
    os_info->os_name = NULL;
	os_info->os_version = NULL;
	os_info->os_release = NULL;
    os_info->os_major_version = 0;
    os_info->os_minor_version = 0;
    os_info->os_build_version = 0;
    os_info->os_arch = NULL;
    os_info->os_arch_data = NULL;
	os_info->node_name = NULL;
	os_info->version = NULL;
    os_info->release = NULL;
    return os_info;
}

void free_os_info(os_info_t* os_info) {
    if (!os_info)  {
        return;
    }
    free(os_info->os_name);
	free(os_info->os_version);
	free(os_info->os_release);
    free(os_info->os_arch);
    free(os_info->os_arch_data);
	free(os_info->node_name);
	free(os_info->version);
    free(os_info->release);

    free(os_info);
}
