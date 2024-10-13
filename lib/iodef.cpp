#include <stdio.h>
#include "iodef.h"

#define LIB_IO_BUF_SIZE 65536

void lib_io_buf_init() {
    #ifdef _WIN32
    //char buf[LIB_IO_BUF_SIZE];
    //setvbuf(stdout, buf, _IOFBF, LIB_IO_BUF_SIZE);
    lib_io_set_buf_size(stdout, _IOFBF, LIB_IO_BUF_SIZE);
    #endif
}

void lib_io_set_buf_size(FILE* file, int type, size_t size) {
    char buf[size];
    setvbuf(stdout, buf, type, size);
}

