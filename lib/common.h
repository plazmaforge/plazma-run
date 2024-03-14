#ifndef PLAZMA_LIB_COMMON_H
#define PLAZMA_LIB_COMMON_H

#include <string.h>
#include "alignment.h"
#include "error.h"

void lib_platform_zeroize(void *buf, size_t len) {
    if (len > 0) {
        // TODO
        memset(buf, 0, len);
    }
}

/**
 * Perform a fast block XOR operation, such that
 * r[i] = a[i] ^ b[i] where 0 <= i < n
 *
 * \param   r Pointer to result (buffer of at least \p n bytes). \p r
 *            may be equal to either \p a or \p b, but behaviour when
 *            it overlaps in other ways is undefined.
 * \param   a Pointer to input (buffer of at least \p n bytes)
 * \param   b Pointer to input (buffer of at least \p n bytes)
 * \param   n Number of bytes to process.
 *
 * \note      Depending on the situation, it may be faster to use either lib_xor() or
 *            lib_xor_no_simd() (these are functionally equivalent).
 *            If the result is used immediately after the xor operation in non-SIMD code (e.g, in
 *            AES-CBC), there may be additional latency to transfer the data from SIMD to scalar
 *            registers, and in this case, lib_xor_no_simd() may be faster. In other cases where
 *            the result is not used immediately (e.g., in AES-CTR), lib_xor() may be faster.
 *            For targets without SIMD support, they will behave the same.
 */
static inline void lib_xor(unsigned char *r,
                               const unsigned char *a,
                               const unsigned char *b,
                               size_t n)
{
    size_t i = 0;
#if defined(LIB_EFFICIENT_UNALIGNED_ACCESS)
#if defined(LIB_HAVE_NEON_INTRINSICS) && \
    (!(defined(LIB_COMPILER_IS_GCC) && LIB_GCC_VERSION < 70300))
    /* Old GCC versions generate a warning here, so disable the NEON path for these compilers */
    for (; (i + 16) <= n; i += 16) {
        uint8x16_t v1 = vld1q_u8(a + i);
        uint8x16_t v2 = vld1q_u8(b + i);
        uint8x16_t x = veorq_u8(v1, v2);
        vst1q_u8(r + i, x);
    }
#if defined(__IAR_SYSTEMS_ICC__)
    /* This if statement helps some compilers (e.g., IAR) optimise out the byte-by-byte tail case
     * where n is a constant multiple of 16.
     * For other compilers (e.g. recent gcc and clang) it makes no difference if n is a compile-time
     * constant, and is a very small perf regression if n is not a compile-time constant. */
    if (n % 16 == 0) {
        return;
    }
#endif
#elif defined(LIB_ARCH_IS_X64) || defined(LIB_ARCH_IS_ARM64)
    /* This codepath probably only makes sense on architectures with 64-bit registers */
    for (; (i + 8) <= n; i += 8) {
        uint64_t x = lib_get_unaligned_uint64(a + i) ^ lib_get_unaligned_uint64(b + i);
        lib_put_unaligned_uint64(r + i, x);
    }
#if defined(__IAR_SYSTEMS_ICC__)
    if (n % 8 == 0) {
        return;
    }
#endif
#else
    for (; (i + 4) <= n; i += 4) {
        uint32_t x = lib_get_unaligned_uint32(a + i) ^ lib_get_unaligned_uint32(b + i);
        lib_put_unaligned_uint32(r + i, x);
    }
#if defined(__IAR_SYSTEMS_ICC__)
    if (n % 4 == 0) {
        return;
    }
#endif
#endif
#endif
    for (; i < n; i++) {
        r[i] = a[i] ^ b[i];
    }
}



#endif // PLAZMA_LIB_COMMON_H