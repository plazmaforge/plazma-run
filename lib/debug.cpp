#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "debug.h"

/* DEBUG_BUF_SIZE must be at least 2 */
#define DEBUG_BUF_SIZE      512

static int debug_threshold = 3; //0;

void lib_debug_set_threshold(int threshold) {
    debug_threshold = threshold;
}

static void debug_print_item(int level, const char *file, int line,
                        const char *str) {
    printf("%s: %i: %s", file, line, str);
}

static inline void debug_send_line(const void* obj, int level,
                                   const char* file, int line,
                                   const char* str) {
     char idstr[20 + DEBUG_BUF_SIZE]; /* 0x + 16 nibbles + ': ' */
     snprintf(idstr, sizeof(idstr), "%p: %s", obj, str);
     debug_print_item(level, file, line, idstr);
}

void lib_obj_debug_print_msg(const void* obj, int level,
                             const char* file, int line,
                             const char* format, ...) {

    if (NULL == obj || level > debug_threshold) {
       return;
    }

    va_list argp;
    char str[DEBUG_BUF_SIZE];
    int ret = -1;


    va_start(argp, format);
    ret = vsnprintf(str, DEBUG_BUF_SIZE, format, argp);
    va_end(argp);

    if (ret < 0) {
        ret = 0;
    } else {
        if (ret >= DEBUG_BUF_SIZE - 1) {
            ret = DEBUG_BUF_SIZE - 2;
        }
    }
    str[ret]     = '\n';
    str[ret + 1] = '\0';

    debug_send_line(obj, level, file, line, str);
}

void lib_obj_debug_print_ret(const void* obj, int level,
                             const char* file, int line,
                             const char* text, int ret) {

    if (NULL == obj || level > debug_threshold) {
       return;
    }

    char str[DEBUG_BUF_SIZE];

    /*
     * With non-blocking I/O and examples that just retry immediately,
     * the logs would be quickly flooded with WANT_READ, so ignore that.
     * Don't ignore WANT_WRITE however, since it is usually rare.
     */

    if (ret == LIB_ERR_IO_WANT_READ) {
        return;
    }

    snprintf(str, sizeof(str), "%s() returned %d (-0x%04x)\n",
                     text, ret, (unsigned int) -ret);

    debug_send_line(obj, level, file, line, str);
}

void lib_debug_print_msg(int level,
                             const char* file, int line,
                             const char* format, ...) {

    if (level > debug_threshold) {
       return;
    }

    va_list argp;
    char str[DEBUG_BUF_SIZE];
    int ret = -1;


    va_start(argp, format);
    ret = vsnprintf(str, DEBUG_BUF_SIZE, format, argp);
    va_end(argp);

    if (ret < 0) {
        ret = 0;
    } else {
        if (ret >= DEBUG_BUF_SIZE - 1) {
            ret = DEBUG_BUF_SIZE - 2;
        }
    }
    str[ret]     = '\n';
    str[ret + 1] = '\0';

    debug_print_item(level, file, line, str);
}

