
#ifndef PLAZMA_LIB_ALIGNMENT_H
#define PLAZMA_LIB_ALIGNMENT_H

#include <string.h>
#include <stdint.h>

/** Byte Reading Macros
 *
 * Given a multi-byte integer \p x, LIB_BYTE_n retrieves the n-th
 * byte from x, where byte 0 is the least significant byte.
 */
#define LIB_BYTE_0(x) ((uint8_t) ((x)         & 0xff))
#define LIB_BYTE_1(x) ((uint8_t) (((x) >>  8) & 0xff))
#define LIB_BYTE_2(x) ((uint8_t) (((x) >> 16) & 0xff))
#define LIB_BYTE_3(x) ((uint8_t) (((x) >> 24) & 0xff))
#define LIB_BYTE_4(x) ((uint8_t) (((x) >> 32) & 0xff))
#define LIB_BYTE_5(x) ((uint8_t) (((x) >> 40) & 0xff))
#define LIB_BYTE_6(x) ((uint8_t) (((x) >> 48) & 0xff))
#define LIB_BYTE_7(x) ((uint8_t) (((x) >> 56) & 0xff))

/*
 * Detect GCC built-in byteswap routines
 */
#if defined(__GNUC__) && defined(__GNUC_PREREQ)
#if __GNUC_PREREQ(4, 8)
#define LIB_BSWAP16 __builtin_bswap16
#endif /* __GNUC_PREREQ(4,8) */
#if __GNUC_PREREQ(4, 3)
#define LIB_BSWAP32 __builtin_bswap32
#define LIB_BSWAP64 __builtin_bswap64
#endif /* __GNUC_PREREQ(4,3) */
#endif /* defined(__GNUC__) && defined(__GNUC_PREREQ) */

/*
 * Detect Clang built-in byteswap routines
 */
#if defined(__clang__) && defined(__has_builtin)
#if __has_builtin(__builtin_bswap16) && !defined(LIB_BSWAP16)
#define LIB_BSWAP16 __builtin_bswap16
#endif /* __has_builtin(__builtin_bswap16) */
#if __has_builtin(__builtin_bswap32) && !defined(LIB_BSWAP32)
#define LIB_BSWAP32 __builtin_bswap32
#endif /* __has_builtin(__builtin_bswap32) */
#if __has_builtin(__builtin_bswap64) && !defined(LIB_BSWAP64)
#define LIB_BSWAP64 __builtin_bswap64
#endif /* __has_builtin(__builtin_bswap64) */
#endif /* defined(__clang__) && defined(__has_builtin) */

/*
 * Detect MSVC built-in byteswap routines
 */
#if defined(_MSC_VER)
#if !defined(LIB_BSWAP16)
#define LIB_BSWAP16 _byteswap_ushort
#endif
#if !defined(LIB_BSWAP32)
#define LIB_BSWAP32 _byteswap_ulong
#endif
#if !defined(LIB_BSWAP64)
#define LIB_BSWAP64 _byteswap_uint64
#endif
#endif /* defined(_MSC_VER) */

/* Detect armcc built-in byteswap routine */
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 410000) && !defined(LIB_BSWAP32)
#if defined(__ARM_ACLE)  /* ARM Compiler 6 - earlier versions don't need a header */
#include <arm_acle.h>
#endif
#define LIB_BSWAP32 __rev
#endif

/* Detect IAR built-in byteswap routine */
#if defined(__IAR_SYSTEMS_ICC__)
#if defined(__ARM_ACLE)
#include <arm_acle.h>
#define LIB_BSWAP16(x) ((uint16_t) __rev16((uint32_t) (x)))
#define LIB_BSWAP32 __rev
#define LIB_BSWAP64 __revll
#endif
#endif

/*
 * Where compiler built-ins are not present, fall back to C code that the
 * compiler may be able to detect and transform into the relevant bswap or
 * similar instruction.
 */
#if !defined(LIB_BSWAP16)
static inline uint16_t lib_bswap16(uint16_t x)
{
    return
        (x & 0x00ff) << 8 |
        (x & 0xff00) >> 8;
}
#define LIB_BSWAP16 lib_bswap16
#endif /* !defined(LIB_BSWAP16) */

#if !defined(LIB_BSWAP32)
static inline uint32_t lib_bswap32(uint32_t x)
{
    return
        (x & 0x000000ff) << 24 |
        (x & 0x0000ff00) <<  8 |
        (x & 0x00ff0000) >>  8 |
        (x & 0xff000000) >> 24;
}
#define LIB_BSWAP32 lib_bswap32
#endif /* !defined(LIB_BSWAP32) */

#if !defined(LIB_BSWAP64)
static inline uint64_t lib_bswap64(uint64_t x)
{
    return
        (x & 0x00000000000000ffULL) << 56 |
        (x & 0x000000000000ff00ULL) << 40 |
        (x & 0x0000000000ff0000ULL) << 24 |
        (x & 0x00000000ff000000ULL) <<  8 |
        (x & 0x000000ff00000000ULL) >>  8 |
        (x & 0x0000ff0000000000ULL) >> 24 |
        (x & 0x00ff000000000000ULL) >> 40 |
        (x & 0xff00000000000000ULL) >> 56;
}
#define LIB_BSWAP64 lib_bswap64
#endif /* !defined(LIB_BSWAP64) */

////

#if !defined(__BYTE_ORDER__)

#if defined(__LITTLE_ENDIAN__)
/* IAR defines __xxx_ENDIAN__, but not __BYTE_ORDER__ */
#define LIB_IS_BIG_ENDIAN 0
#elif defined(__BIG_ENDIAN__)
#define LIB_IS_BIG_ENDIAN 1
#else
static const uint16_t lib_byte_order_detector = { 0x100 };
#define LIB_IS_BIG_ENDIAN (*((unsigned char *) (&lib_byte_order_detector)) == 0x01)
#endif

#else

#if (__BYTE_ORDER__) == (__ORDER_BIG_ENDIAN__)
#define LIB_IS_BIG_ENDIAN 1
#else
#define LIB_IS_BIG_ENDIAN 0
#endif

#endif /* !defined(__BYTE_ORDER__) */

////

static inline uint16_t lib_get_unaligned_uint16(const void *p) {
    uint16_t r;
    memcpy(&r, p, sizeof(r));
    return r;
}

static inline void lib_put_unaligned_uint16(void *p, uint16_t x) {
    memcpy(p, &x, sizeof(x));
}

static inline uint32_t lib_get_unaligned_uint32(const void *p) {
    uint32_t r;
    memcpy(&r, p, sizeof(r));
    return r;
}

static inline uint64_t lib_get_unaligned_uint64(const void *p) {
    uint64_t r;
    memcpy(&r, p, sizeof(r));
    return r;
}

static inline void lib_put_unaligned_uint32(void *p, uint32_t x) {
    memcpy(p, &x, sizeof(x));
}

static inline void lib_put_unaligned_uint64(void *p, uint64_t x) {
    memcpy(p, &x, sizeof(x));
}

////

#define LIB_GET_UINT16_LE(data, offset)                                \
    ((LIB_IS_BIG_ENDIAN)                                               \
        ? LIB_BSWAP16(lib_get_unaligned_uint16((data) + (offset)))     \
        : lib_get_unaligned_uint16((data) + (offset))                  \
    )

#define LIB_PUT_UINT16_LE(n, data, offset)                                            \
    {                                                                                 \
        if (LIB_IS_BIG_ENDIAN)                                                        \
        {                                                                             \
            lib_put_unaligned_uint16((data) + (offset), LIB_BSWAP16((uint16_t) (n))); \
        }                                                                             \
        else                                                                          \
        {                                                                             \
            lib_put_unaligned_uint16((data) + (offset), (uint16_t) (n));              \
        }                                                                             \
    }

#define LIB_GET_UINT16_BE(data, offset)                                \
    ((LIB_IS_BIG_ENDIAN)                                               \
        ? lib_get_unaligned_uint16((data) + (offset))                  \
        : LIB_BSWAP16(lib_get_unaligned_uint16((data) + (offset)))     \
    )

#define LIB_PUT_UINT16_BE(n, data, offset)                                            \
    {                                                                                 \
        if (LIB_IS_BIG_ENDIAN)                                                        \
        {                                                                             \
            lib_put_unaligned_uint16((data) + (offset), (uint16_t) (n));              \
        }                                                                             \
        else                                                                          \
        {                                                                             \
            lib_put_unaligned_uint16((data) + (offset), LIB_BSWAP16((uint16_t) (n))); \
        }                                                                             \
    }

////

#define LIB_GET_UINT32_BE(data, offset)                                \
    ((LIB_IS_BIG_ENDIAN)                                               \
        ? lib_get_unaligned_uint32((data) + (offset))                  \
        : LIB_BSWAP32(lib_get_unaligned_uint32((data) + (offset)))     \
    )

#define LIB_PUT_UINT32_BE(n, data, offset)                                            \
    {                                                                                 \
        if (LIB_IS_BIG_ENDIAN)                                                        \
        {                                                                             \
            lib_put_unaligned_uint32((data) + (offset), (uint32_t) (n));              \
        }                                                                             \
        else                                                                          \
        {                                                                             \
            lib_put_unaligned_uint32((data) + (offset), LIB_BSWAP32((uint32_t) (n))); \
        }                                                                             \
    }

#define LIB_GET_UINT32_LE(data, offset)                                \
    ((LIB_IS_BIG_ENDIAN)                                               \
        ? LIB_BSWAP32(lib_get_unaligned_uint32((data) + (offset)))     \
        : lib_get_unaligned_uint32((data) + (offset))                  \
    )

#define LIB_PUT_UINT32_LE(n, data, offset)                                            \
    {                                                                                 \
        if (LIB_IS_BIG_ENDIAN)                                                        \
        {                                                                             \
            lib_put_unaligned_uint32((data) + (offset), LIB_BSWAP32((uint32_t) (n))); \
        }                                                                             \
        else                                                                          \
        {                                                                             \
            lib_put_unaligned_uint32((data) + (offset), ((uint32_t) (n)));            \
        }                                                                             \
    }

#endif /* PLAZMA_LIB_ALIGNMENT_H */
