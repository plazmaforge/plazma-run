#ifndef PLAZMA_LIB_DEBUG_H
#define PLAZMA_LIB_DEBUG_H

#define LIB_DEBUG_C

#if defined(LIB_DEBUG_C)

#define LIB_DEBUG_STRIP_PARENS(...)   __VA_ARGS__

#define LIB_OBJ_DEBUG_MSG(obj, level, args)                     \
    lib_obj_debug_print_msg(obj, level, __FILE__, __LINE__,     \
                            LIB_DEBUG_STRIP_PARENS args)
#define LIB_OBJ_DEBUG_RET(obj, level, text, ret)                \
    lib_obj_debug_print_ret(obj, level, __FILE__, __LINE__, text, ret)

#define LIB_DEBUG_MSG(level, args)                              \
    lib_debug_print_msg(level, __FILE__, __LINE__,              \

#else /* LIB_DEBUG_C */

#define LIB_OBJ_DEBUG_MSG(obj, level, args)            do { } while (0)
#define LIB_OBJ_DEBUG_RET(obj, level, text, ret)       do { } while (0)
#define LIB_OBJ_DEBUG_BUF(obj, level, text, buf, len)  do { } while (0)
#define LIB_OBJ_DEBUG_MPI(obj, level, text, X)         do { } while (0)
#define LIB_OBJ_DEBUG_ECP(obj, level, text, X)         do { } while (0)
#define LIB_OBJ_DEBUG_CRT(obj, level, text, crt)       do { } while (0)
#define LIB_OBJ_DEBUG_ECDH(obj, level, ecdh, attr)     do { } while (0)

#endif

void lib_obj_debug_print_msg(const void* obj, int level,
                             const char* file, int line,
                             const char* format, ...);

void lib_obj_debug_print_ret(const void* obj, int level,
                             const char* file, int line,
                             const char* text, int ret);

#endif // PLAZMA_LIB_DEBUG_H