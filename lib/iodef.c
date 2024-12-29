#include <stdio.h>
#include "iodef.h"

#define LIB_IO_STDOUT_BUF_SIZE 65536

static size_t stdin_buf_size = 0;
static size_t stdout_buf_size = 0;
static size_t stderr_buf_size = 0;

int lib_io_buf_init() {
    if (lib_io_std_use_buf_size()) {
        return lib_io_stdout_set_buf_size(_IOFBF, LIB_IO_STDOUT_BUF_SIZE);
    }
    return 0;
}

int lib_io_set_buf_size(FILE* file, int mode, size_t size) {
    if (!file) {
        return -1;
    }
    char buf[size];
    return setvbuf(file, buf, mode, size);
}

int lib_io_stdin_set_buf_size(int mode, size_t size) {
    int err = lib_io_set_buf_size(stdin, mode, size);
    if (err == 0) {
        stdin_buf_size = size;
        //fprintf(stderr, "Set stdin buf size: %lu\n", size);
    } else {
        fprintf(stderr, "Can't set stdin buf size: %lu, errno: %i\n", size, err);
    }    
    return err;
}

size_t lib_io_get_stdin_buf_size() {
    return stdin_buf_size;
}

bool lib_io_stdin_use_buf_size() {
    return stdin_buf_size > 0;
}

int lib_io_stdout_set_buf_size(int mode, size_t size) {
    int err = lib_io_set_buf_size(stdout, mode, size);
    if (err == 0) {
        stdout_buf_size = size;
        //fprintf(stderr, "Set stdout buf size: %lu\n", size);
    } else {
        fprintf(stderr, "Can't set stdout buf size: %lu, errno: %i\n", size, err);
    }    
    return err;
}

size_t lib_io_stdout_get_buf_size() {
    return stdout_buf_size;
}

bool lib_io_stdout_use_buf_size() {
    return stdout_buf_size > 0;
}

int lib_io_stderr_set_buf_size(int mode, size_t size) {
    int err = lib_io_set_buf_size(stderr, mode, size);
    if (err == 0) {
        stderr_buf_size = size;
        //fprintf(stderr, "Set stderr buf size: %lu\n", size);        
    } else {
        fprintf(stderr, "Can't set stderr buf size: %lu, errno: %i\n", size, err);
    } 
    return err;
}

size_t lib_io_stderr_get_buf_size() {
    return stderr_buf_size;
}

bool lib_io_stderr_use_buf_size() {
    return stderr_buf_size > 0;
}

bool lib_io_std_use_buf_size() {
    #ifdef _WIN32
    return true;
    #else
    return false;
    #endif    
}

size_t lib_io_get_buf_page(size_t size) {
    //if (size <= 4096) {
    //    return size / 3;
    //}
    if (size <= 5120) {
        return size / 3;
    }
    if (size <= 6141) {
        return size / 2;
    }
    //if (size <= 7168) {
    //    return size / 1.5;
    //}
    //if (size <= 8192) {
    //    return size / 1.5;
    //}
    //if (size <= 9216) {
    //    return size / 1.5;
    //}
    //if (size <= 10240) {
    //    return size / 1.5;
    //}
    if (size <= 20480) {
        return size / 1.5;
    }
    return size / 1.1;
}
