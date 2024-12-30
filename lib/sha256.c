/*
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

// 1

#include "common.h"
#include "memlib.h"
#include "md.h"

#if defined(LIB_SHA256_C) || defined(LIB_SHA224_C)

#define LIB_SELF_TEST

#include "sha256.h"
#include <string.h>

// 2

#if defined(LIB_SELF_TEST) 
#include <stdio.h>
#endif

#if !defined(LIB_SHA256_ALT)

#define SHA256_BLOCK_SIZE 64

void lib_sha256_init(lib_sha256_context_t* ctx) {
    //memset(ctx, 0, sizeof(lib_sha256_context_t));
    lib_memset0(ctx, sizeof(lib_sha256_context_t));
}

void lib_sha256_free(lib_sha256_context_t *ctx) {
    //if (ctx == NULL) {
    //    return;
    //}
    //lib_platform_zeroize(ctx, sizeof(lib_sha256_context_t));
    lib_memset0(ctx, sizeof(lib_sha256_context_t)); /* WHY memset0 */
}

void lib_sha256_clone(lib_sha256_context_t* dst, const lib_sha256_context_t* src) {
    *dst = *src;
}

/*
 * SHA-256 context setup
 */
int lib_sha256_starts(lib_sha256_context_t* ctx, int is224) {
#if defined(LIB_SHA224_C) && defined(LIB_SHA256_C)
    if (is224 != 0 && is224 != 1) {
        return LIB_ERR_SHA256_BAD_INPUT_DATA;
    }
#elif defined(LIB_SHA256_C)
    if (is224 != 0) {
        return LIB_ERR_SHA256_BAD_INPUT_DATA;
    }
#else /* defined LIB_SHA224_C only */
    if (is224 == 0) {
        return LIB_ERR_SHA256_BAD_INPUT_DATA;
    }
#endif

    ctx->total[0] = 0;
    ctx->total[1] = 0;

    if (is224 == 0) {
#if defined(LIB_SHA256_C)
        ctx->state[0] = 0x6A09E667;
        ctx->state[1] = 0xBB67AE85;
        ctx->state[2] = 0x3C6EF372;
        ctx->state[3] = 0xA54FF53A;
        ctx->state[4] = 0x510E527F;
        ctx->state[5] = 0x9B05688C;
        ctx->state[6] = 0x1F83D9AB;
        ctx->state[7] = 0x5BE0CD19;
#endif
    } else {
#if defined(LIB_SHA224_C)
        ctx->state[0] = 0xC1059ED8;
        ctx->state[1] = 0x367CD507;
        ctx->state[2] = 0x3070DD17;
        ctx->state[3] = 0xF70E5939;
        ctx->state[4] = 0xFFC00B31;
        ctx->state[5] = 0x68581511;
        ctx->state[6] = 0x64F98FA7;
        ctx->state[7] = 0xBEFA4FA4;
#endif
    }

#if defined(LIB_SHA224_C)
    ctx->is224 = is224;
#endif

    return 0;
}

#if !defined(LIB_SHA256_PROCESS_ALT)
static const uint32_t K[] =
{
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
    0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
    0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
    0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
    0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
    0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
    0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
    0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
    0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
    0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
    0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
    0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
    0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
    0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2,
};

#endif

// SOCKET-3: CRYPTO_IF_PRESENT

//#if !defined(LIB_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT)

#define lib_internal_sha256_process_many_c lib_internal_sha256_process_many
#define lib_internal_sha256_process_c      lib_internal_sha256_process

//#endif

#if !defined(LIB_SHA256_PROCESS_ALT) && \
    !defined(LIB_SHA256_USE_ARMV8_A_CRYPTO_ONLY)

#define  SHR(x, n) (((x) & 0xFFFFFFFF) >> (n))
#define ROTR(x, n) (SHR(x, n) | ((x) << (32 - (n))))

#define S0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^  SHR(x, 3))
#define S1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^  SHR(x, 10))

#define S2(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define S3(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))

#define F0(x, y, z) (((x) & (y)) | ((z) & ((x) | (y))))
#define F1(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))

#define R(t)                                                        \
    (                                                               \
        local.W[t] = S1(local.W[(t) -  2]) + local.W[(t) -  7] +    \
                     S0(local.W[(t) - 15]) + local.W[(t) - 16]      \
    )

#define P(a, b, c, d, e, f, g, h, x, K)                                      \
    do                                                              \
    {                                                               \
        local.temp1 = (h) + S3(e) + F1((e), (f), (g)) + (K) + (x);    \
        local.temp2 = S2(a) + F0((a), (b), (c));                      \
        (d) += local.temp1; (h) = local.temp1 + local.temp2;        \
    } while (0)



int lib_internal_sha256_process_c(lib_sha256_context_t* ctx,
                                      const unsigned char data[SHA256_BLOCK_SIZE]) {
    struct {
        uint32_t temp1, temp2, W[64];
        uint32_t A[8];
    } local;

    unsigned int i;

    for (i = 0; i < 8; i++) {
        local.A[i] = ctx->state[i];
    }

#if defined(LIB_SHA256_SMALLER)
    for (i = 0; i < 64; i++) {
        if (i < 16) {
            local.W[i] = LIB_GET_UINT32_BE(data, 4 * i);
        } else {
            R(i);
        }

        P(local.A[0], local.A[1], local.A[2], local.A[3], local.A[4],
          local.A[5], local.A[6], local.A[7], local.W[i], K[i]);

        local.temp1 = local.A[7]; local.A[7] = local.A[6];
        local.A[6] = local.A[5]; local.A[5] = local.A[4];
        local.A[4] = local.A[3]; local.A[3] = local.A[2];
        local.A[2] = local.A[1]; local.A[1] = local.A[0];
        local.A[0] = local.temp1;
    }
#else /* LIB_SHA256_SMALLER */
    for (i = 0; i < 16; i++) {
        local.W[i] = LIB_GET_UINT32_BE(data, 4 * i);
    }

    for (i = 0; i < 16; i += 8) {
        P(local.A[0], local.A[1], local.A[2], local.A[3], local.A[4],
          local.A[5], local.A[6], local.A[7], local.W[i+0], K[i+0]);
        P(local.A[7], local.A[0], local.A[1], local.A[2], local.A[3],
          local.A[4], local.A[5], local.A[6], local.W[i+1], K[i+1]);
        P(local.A[6], local.A[7], local.A[0], local.A[1], local.A[2],
          local.A[3], local.A[4], local.A[5], local.W[i+2], K[i+2]);
        P(local.A[5], local.A[6], local.A[7], local.A[0], local.A[1],
          local.A[2], local.A[3], local.A[4], local.W[i+3], K[i+3]);
        P(local.A[4], local.A[5], local.A[6], local.A[7], local.A[0],
          local.A[1], local.A[2], local.A[3], local.W[i+4], K[i+4]);
        P(local.A[3], local.A[4], local.A[5], local.A[6], local.A[7],
          local.A[0], local.A[1], local.A[2], local.W[i+5], K[i+5]);
        P(local.A[2], local.A[3], local.A[4], local.A[5], local.A[6],
          local.A[7], local.A[0], local.A[1], local.W[i+6], K[i+6]);
        P(local.A[1], local.A[2], local.A[3], local.A[4], local.A[5],
          local.A[6], local.A[7], local.A[0], local.W[i+7], K[i+7]);
    }

    for (i = 16; i < 64; i += 8) {
        P(local.A[0], local.A[1], local.A[2], local.A[3], local.A[4],
          local.A[5], local.A[6], local.A[7], R(i+0), K[i+0]);
        P(local.A[7], local.A[0], local.A[1], local.A[2], local.A[3],
          local.A[4], local.A[5], local.A[6], R(i+1), K[i+1]);
        P(local.A[6], local.A[7], local.A[0], local.A[1], local.A[2],
          local.A[3], local.A[4], local.A[5], R(i+2), K[i+2]);
        P(local.A[5], local.A[6], local.A[7], local.A[0], local.A[1],
          local.A[2], local.A[3], local.A[4], R(i+3), K[i+3]);
        P(local.A[4], local.A[5], local.A[6], local.A[7], local.A[0],
          local.A[1], local.A[2], local.A[3], R(i+4), K[i+4]);
        P(local.A[3], local.A[4], local.A[5], local.A[6], local.A[7],
          local.A[0], local.A[1], local.A[2], R(i+5), K[i+5]);
        P(local.A[2], local.A[3], local.A[4], local.A[5], local.A[6],
          local.A[7], local.A[0], local.A[1], R(i+6), K[i+6]);
        P(local.A[1], local.A[2], local.A[3], local.A[4], local.A[5],
          local.A[6], local.A[7], local.A[0], R(i+7), K[i+7]);
    }
#endif /* LIB_SHA256_SMALLER */

    for (i = 0; i < 8; i++) {
        ctx->state[i] += local.A[i];
    }

    /* Zeroise buffers and variables to clear sensitive data from memory. */
    //lib_platform_zeroize(&local, sizeof(local));
    lib_memset0(&local, sizeof(local));

    return 0;
}

#endif /* !LIB_SHA256_PROCESS_ALT && !LIB_SHA256_USE_ARMV8_A_CRYPTO_ONLY */


#if !defined(LIB_SHA256_USE_ARMV8_A_CRYPTO_ONLY)

static size_t lib_internal_sha256_process_many_c(lib_sha256_context_t* ctx, const uint8_t *data, size_t len) {
    size_t processed = 0;

    while (len >= SHA256_BLOCK_SIZE) {
        if (lib_internal_sha256_process_c(ctx, data) != 0) {
            return 0;
        }

        data += SHA256_BLOCK_SIZE;
        len  -= SHA256_BLOCK_SIZE;

        processed += SHA256_BLOCK_SIZE;
    }

    return processed;
}

#endif /* !LIB_SHA256_USE_ARMV8_A_CRYPTO_ONLY */


// SOCKET-4: A_CRYPTO_IF_PRESENT

/*
 * SHA-256 process buffer
 */
int lib_sha256_update(lib_sha256_context_t* ctx,
                          const unsigned char* input,
                          size_t ilen) {
    int ret = LIB_ERR_ERROR_CORRUPTION_DETECTED;
    size_t fill;
    uint32_t left;

    if (ilen == 0) {
        return 0;
    }

    left = ctx->total[0] & 0x3F;
    fill = SHA256_BLOCK_SIZE - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < (uint32_t) ilen) {
        ctx->total[1]++;
    }

    if (left && ilen >= fill) {
        memcpy((void *) (ctx->buffer + left), input, fill);

        if ((ret = lib_internal_sha256_process(ctx, ctx->buffer)) != 0) {
            return ret;
        }

        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while (ilen >= SHA256_BLOCK_SIZE) {
        size_t processed =
            lib_internal_sha256_process_many(ctx, input, ilen);
        if (processed < SHA256_BLOCK_SIZE) {
            return LIB_ERR_ERROR_GENERIC_ERROR;
        }

        input += processed;
        ilen  -= processed;
    }

    if (ilen > 0) {
        memcpy((void *) (ctx->buffer + left), input, ilen);
    }

    return 0;
}

/*
 * SHA-256 final digest
 */
int lib_sha256_finish(lib_sha256_context_t* ctx,
                          unsigned char* output) {
    int ret = LIB_ERR_ERROR_CORRUPTION_DETECTED;
    uint32_t used;
    uint32_t high, low;
    int truncated = 0;

    /*
     * Add padding: 0x80 then 0x00 until 8 bytes remain for the length
     */
    used = ctx->total[0] & 0x3F;

    ctx->buffer[used++] = 0x80;

    if (used <= 56) {
        /* Enough room for padding + length in current block */
        memset(ctx->buffer + used, 0, 56 - used);
    } else {
        /* We'll need an extra block */
        memset(ctx->buffer + used, 0, SHA256_BLOCK_SIZE - used);

        if ((ret = lib_internal_sha256_process(ctx, ctx->buffer)) != 0) {
            goto exit;
        }

        memset(ctx->buffer, 0, 56);
    }

    /*
     * Add message length
     */
    high = (ctx->total[0] >> 29)
           | (ctx->total[1] <<  3);
    low  = (ctx->total[0] <<  3);

    LIB_PUT_UINT32_BE(high, ctx->buffer, 56);
    LIB_PUT_UINT32_BE(low,  ctx->buffer, 60);

    if ((ret = lib_internal_sha256_process(ctx, ctx->buffer)) != 0) {
        goto exit;
    }

    /*
     * Output final state
     */
    LIB_PUT_UINT32_BE(ctx->state[0], output,  0);
    LIB_PUT_UINT32_BE(ctx->state[1], output,  4);
    LIB_PUT_UINT32_BE(ctx->state[2], output,  8);
    LIB_PUT_UINT32_BE(ctx->state[3], output, 12);
    LIB_PUT_UINT32_BE(ctx->state[4], output, 16);
    LIB_PUT_UINT32_BE(ctx->state[5], output, 20);
    LIB_PUT_UINT32_BE(ctx->state[6], output, 24);

#if defined(LIB_SHA224_C)
    truncated = ctx->is224;
#endif
    if (!truncated) {
        LIB_PUT_UINT32_BE(ctx->state[7], output, 28);
    }

    ret = 0;

exit:
    lib_sha256_free(ctx);
    return ret;
}

#endif /* !LIB_SHA256_ALT */

/*
 * output = SHA-256( input buffer )
 */
int lib_sha256(const unsigned char *input, size_t ilen, unsigned char *output, int is224) {
    int ret = LIB_ERR_ERROR_CORRUPTION_DETECTED;
    lib_sha256_context_t ctx;

#if defined(LIB_SHA224_C) && defined(LIB_SHA256_C)
    if (is224 != 0 && is224 != 1) {
        return LIB_ERR_SHA256_BAD_INPUT_DATA;
    }
#elif defined(LIB_SHA256_C)
    if (is224 != 0) {
        return LIB_ERR_SHA256_BAD_INPUT_DATA;
    }
#else /* defined LIB_SHA224_C only */
    if (is224 == 0) {
        return LIB_ERR_SHA256_BAD_INPUT_DATA;
    }
#endif

    lib_sha256_init(&ctx);

    if ((ret = lib_sha256_starts(&ctx, is224)) != 0) {
        goto exit;
    }

    if ((ret = lib_sha256_update(&ctx, input, ilen)) != 0) {
        goto exit;
    }

    if ((ret = lib_sha256_finish(&ctx, output)) != 0) {
        goto exit;
    }

exit:
    lib_sha256_free(&ctx);

    return ret;
}

#if defined(LIB_SELF_TEST)
/*
 * FIPS-180-2 test vectors
 */
static const unsigned char sha_test_buf[3][57] =
{
    { "abc" },
    { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
    { "" }
};

static const size_t sha_test_buflen[3] =
{
    3, 56, 1000
};

typedef const unsigned char (sha_test_sum_t)[32];

/*
 * SHA-224 test vectors
 */
#if defined(LIB_SHA224_C)
static sha_test_sum_t sha224_test_sum[] =
{
    { 0x23, 0x09, 0x7D, 0x22, 0x34, 0x05, 0xD8, 0x22,
      0x86, 0x42, 0xA4, 0x77, 0xBD, 0xA2, 0x55, 0xB3,
      0x2A, 0xAD, 0xBC, 0xE4, 0xBD, 0xA0, 0xB3, 0xF7,
      0xE3, 0x6C, 0x9D, 0xA7 },
    { 0x75, 0x38, 0x8B, 0x16, 0x51, 0x27, 0x76, 0xCC,
      0x5D, 0xBA, 0x5D, 0xA1, 0xFD, 0x89, 0x01, 0x50,
      0xB0, 0xC6, 0x45, 0x5C, 0xB4, 0xF5, 0x8B, 0x19,
      0x52, 0x52, 0x25, 0x25 },
    { 0x20, 0x79, 0x46, 0x55, 0x98, 0x0C, 0x91, 0xD8,
      0xBB, 0xB4, 0xC1, 0xEA, 0x97, 0x61, 0x8A, 0x4B,
      0xF0, 0x3F, 0x42, 0x58, 0x19, 0x48, 0xB2, 0xEE,
      0x4E, 0xE7, 0xAD, 0x67 }
};
#endif

/*
 * SHA-256 test vectors
 */
#if defined(LIB_SHA256_C)
static sha_test_sum_t sha256_test_sum[] =
{
    { 0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA,
      0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
      0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C,
      0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD },
    { 0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8,
      0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39,
      0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67,
      0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1 },
    { 0xCD, 0xC7, 0x6E, 0x5C, 0x99, 0x14, 0xFB, 0x92,
      0x81, 0xA1, 0xC7, 0xE2, 0x84, 0xD7, 0x3E, 0x67,
      0xF1, 0x80, 0x9A, 0x48, 0xA4, 0x97, 0x20, 0x0E,
      0x04, 0x6D, 0x39, 0xCC, 0xC7, 0x11, 0x2C, 0xD0 }
};
#endif

/*
 * Checkup routine
 */
static int lib_sha256_common_self_test(int verbose, int is224)
{
    int i, buflen, ret = 0;
    unsigned char *buf;
    unsigned char sha256sum[32];
    lib_sha256_context_t ctx;

#if defined(LIB_SHA224_C) && defined(LIB_SHA256_C)
    sha_test_sum_t *sha_test_sum = (is224) ? sha224_test_sum : sha256_test_sum;
#elif defined(LIB_SHA256_C)
    sha_test_sum_t *sha_test_sum = sha256_test_sum;
#else
    sha_test_sum_t *sha_test_sum = sha224_test_sum;
#endif

    buf = /*lib*/ (unsigned char*) calloc(1024, sizeof(unsigned char));
    if (NULL == buf) {
        if (verbose != 0) {
            /*lib*/ printf("Buffer allocation failed\n");
        }
        return 1;
    }

    lib_sha256_init(&ctx);

    for (i = 0; i < 3; i++) {
        if (verbose != 0) {
            /*lib*/ printf("  SHA-%d test #%d: ", 256 - is224 * 32, i + 1);
        }

        if ((ret = lib_sha256_starts(&ctx, is224)) != 0) {
            goto fail;
        }

        if (i == 2) {
            memset(buf, 'a', buflen = 1000);

            for (int j = 0; j < 1000; j++) {
                ret = lib_sha256_update(&ctx, buf, buflen);
                if (ret != 0) {
                    goto fail;
                }
            }

        } else {
            ret = lib_sha256_update(&ctx, sha_test_buf[i], sha_test_buflen[i]);
            if (ret != 0) {
                goto fail;
            }
        }

        if ((ret = lib_sha256_finish(&ctx, sha256sum)) != 0) {
            goto fail;
        }


        if (memcmp(sha256sum, sha_test_sum[i], 32 - is224 * 4) != 0) {
            ret = 1;
            goto fail;
        }

        if (verbose != 0) {
            /*lib*/ printf("passed\n");
        }
    }

    if (verbose != 0) {
        /*lib*/ printf("\n");
    }

    goto exit;

fail:
    if (verbose != 0) {
        /*lib*/ printf("failed\n");
    }

exit:
    lib_sha256_free(&ctx);
    /*lib*/ free(buf);

    return ret;
}

#if defined(LIB_SHA256_C)
int lib_sha256_self_test(int verbose) {
    return lib_sha256_common_self_test(verbose, 0);
}
#endif /* LIB_SHA256_C */

#if defined(LIB_SHA224_C)
int lib_sha224_self_test(int verbose) {
    return lib_sha256_common_self_test(verbose, 1);
}
#endif /* LIB_SHA224_C */

// int main(int argc, char *argv[]) {
//     lib_sha256_common_self_test(1, 0);
// }

#endif /* LIB_SELF_TEST */

#endif /* LIB_SHA256_C || LIB_SHA224_C */
