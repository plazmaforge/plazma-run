/*
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */

#include "common.h"
#include "memlib.h"
#include "md.h"

#if defined(LIB_SHA1_C)

#include "sha1.h"
#include <string.h>


#if !defined(LIB_SHA1_ALT)

void lib_sha1_init(lib_sha1_context_t *ctx) {
    memset(ctx, 0, sizeof(lib_sha1_context_t));
}

void lib_sha1_free(lib_sha1_context_t *ctx) {
    if (ctx == NULL) {
        return;
    }
    lib_memsetz(ctx, sizeof(lib_sha1_context_t));
}

void lib_sha1_clone(lib_sha1_context_t *dst, const lib_sha1_context_t *src) {
    *dst = *src;
}

/*
 * SHA-1 context setup
 */
int lib_sha1_starts(lib_sha1_context_t *ctx) {
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;

    return 0;
}

#if !defined(LIB_SHA1_PROCESS_ALT)
int lib_internal_sha1_process(lib_sha1_context_t *ctx, const unsigned char data[64]) {
    struct {
        uint32_t temp, W[16], A, B, C, D, E;
    } local;

    local.W[0] = LIB_GET_UINT32_BE(data,  0);
    local.W[1] = LIB_GET_UINT32_BE(data,  4);
    local.W[2] = LIB_GET_UINT32_BE(data,  8);
    local.W[3] = LIB_GET_UINT32_BE(data, 12);
    local.W[4] = LIB_GET_UINT32_BE(data, 16);
    local.W[5] = LIB_GET_UINT32_BE(data, 20);
    local.W[6] = LIB_GET_UINT32_BE(data, 24);
    local.W[7] = LIB_GET_UINT32_BE(data, 28);
    local.W[8] = LIB_GET_UINT32_BE(data, 32);
    local.W[9] = LIB_GET_UINT32_BE(data, 36);
    local.W[10] = LIB_GET_UINT32_BE(data, 40);
    local.W[11] = LIB_GET_UINT32_BE(data, 44);
    local.W[12] = LIB_GET_UINT32_BE(data, 48);
    local.W[13] = LIB_GET_UINT32_BE(data, 52);
    local.W[14] = LIB_GET_UINT32_BE(data, 56);
    local.W[15] = LIB_GET_UINT32_BE(data, 60);

#define S(x, n) (((x) << (n)) | (((x) & 0xFFFFFFFF) >> (32 - (n))))

#define R(t)                                                    \
    (                                                           \
        local.temp = local.W[((t) -  3) & 0x0F] ^             \
                     local.W[((t) -  8) & 0x0F] ^             \
                     local.W[((t) - 14) & 0x0F] ^             \
                     local.W[(t)        & 0x0F],              \
        (local.W[(t) & 0x0F] = S(local.temp, 1))               \
    )

#define P(a, b, c, d, e, x)                                          \
    do                                                          \
    {                                                           \
        (e) += S((a), 5) + F((b), (c), (d)) + K + (x);             \
        (b) = S((b), 30);                                        \
    } while (0)

    local.A = ctx->state[0];
    local.B = ctx->state[1];
    local.C = ctx->state[2];
    local.D = ctx->state[3];
    local.E = ctx->state[4];

#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define K 0x5A827999

    P(local.A, local.B, local.C, local.D, local.E, local.W[0]);
    P(local.E, local.A, local.B, local.C, local.D, local.W[1]);
    P(local.D, local.E, local.A, local.B, local.C, local.W[2]);
    P(local.C, local.D, local.E, local.A, local.B, local.W[3]);
    P(local.B, local.C, local.D, local.E, local.A, local.W[4]);
    P(local.A, local.B, local.C, local.D, local.E, local.W[5]);
    P(local.E, local.A, local.B, local.C, local.D, local.W[6]);
    P(local.D, local.E, local.A, local.B, local.C, local.W[7]);
    P(local.C, local.D, local.E, local.A, local.B, local.W[8]);
    P(local.B, local.C, local.D, local.E, local.A, local.W[9]);
    P(local.A, local.B, local.C, local.D, local.E, local.W[10]);
    P(local.E, local.A, local.B, local.C, local.D, local.W[11]);
    P(local.D, local.E, local.A, local.B, local.C, local.W[12]);
    P(local.C, local.D, local.E, local.A, local.B, local.W[13]);
    P(local.B, local.C, local.D, local.E, local.A, local.W[14]);
    P(local.A, local.B, local.C, local.D, local.E, local.W[15]);
    P(local.E, local.A, local.B, local.C, local.D, R(16));
    P(local.D, local.E, local.A, local.B, local.C, R(17));
    P(local.C, local.D, local.E, local.A, local.B, R(18));
    P(local.B, local.C, local.D, local.E, local.A, R(19));

#undef K
#undef F

#define F(x, y, z) ((x) ^ (y) ^ (z))
#define K 0x6ED9EBA1

    P(local.A, local.B, local.C, local.D, local.E, R(20));
    P(local.E, local.A, local.B, local.C, local.D, R(21));
    P(local.D, local.E, local.A, local.B, local.C, R(22));
    P(local.C, local.D, local.E, local.A, local.B, R(23));
    P(local.B, local.C, local.D, local.E, local.A, R(24));
    P(local.A, local.B, local.C, local.D, local.E, R(25));
    P(local.E, local.A, local.B, local.C, local.D, R(26));
    P(local.D, local.E, local.A, local.B, local.C, R(27));
    P(local.C, local.D, local.E, local.A, local.B, R(28));
    P(local.B, local.C, local.D, local.E, local.A, R(29));
    P(local.A, local.B, local.C, local.D, local.E, R(30));
    P(local.E, local.A, local.B, local.C, local.D, R(31));
    P(local.D, local.E, local.A, local.B, local.C, R(32));
    P(local.C, local.D, local.E, local.A, local.B, R(33));
    P(local.B, local.C, local.D, local.E, local.A, R(34));
    P(local.A, local.B, local.C, local.D, local.E, R(35));
    P(local.E, local.A, local.B, local.C, local.D, R(36));
    P(local.D, local.E, local.A, local.B, local.C, R(37));
    P(local.C, local.D, local.E, local.A, local.B, R(38));
    P(local.B, local.C, local.D, local.E, local.A, R(39));

#undef K
#undef F

#define F(x, y, z) (((x) & (y)) | ((z) & ((x) | (y))))
#define K 0x8F1BBCDC

    P(local.A, local.B, local.C, local.D, local.E, R(40));
    P(local.E, local.A, local.B, local.C, local.D, R(41));
    P(local.D, local.E, local.A, local.B, local.C, R(42));
    P(local.C, local.D, local.E, local.A, local.B, R(43));
    P(local.B, local.C, local.D, local.E, local.A, R(44));
    P(local.A, local.B, local.C, local.D, local.E, R(45));
    P(local.E, local.A, local.B, local.C, local.D, R(46));
    P(local.D, local.E, local.A, local.B, local.C, R(47));
    P(local.C, local.D, local.E, local.A, local.B, R(48));
    P(local.B, local.C, local.D, local.E, local.A, R(49));
    P(local.A, local.B, local.C, local.D, local.E, R(50));
    P(local.E, local.A, local.B, local.C, local.D, R(51));
    P(local.D, local.E, local.A, local.B, local.C, R(52));
    P(local.C, local.D, local.E, local.A, local.B, R(53));
    P(local.B, local.C, local.D, local.E, local.A, R(54));
    P(local.A, local.B, local.C, local.D, local.E, R(55));
    P(local.E, local.A, local.B, local.C, local.D, R(56));
    P(local.D, local.E, local.A, local.B, local.C, R(57));
    P(local.C, local.D, local.E, local.A, local.B, R(58));
    P(local.B, local.C, local.D, local.E, local.A, R(59));

#undef K
#undef F

#define F(x, y, z) ((x) ^ (y) ^ (z))
#define K 0xCA62C1D6

    P(local.A, local.B, local.C, local.D, local.E, R(60));
    P(local.E, local.A, local.B, local.C, local.D, R(61));
    P(local.D, local.E, local.A, local.B, local.C, R(62));
    P(local.C, local.D, local.E, local.A, local.B, R(63));
    P(local.B, local.C, local.D, local.E, local.A, R(64));
    P(local.A, local.B, local.C, local.D, local.E, R(65));
    P(local.E, local.A, local.B, local.C, local.D, R(66));
    P(local.D, local.E, local.A, local.B, local.C, R(67));
    P(local.C, local.D, local.E, local.A, local.B, R(68));
    P(local.B, local.C, local.D, local.E, local.A, R(69));
    P(local.A, local.B, local.C, local.D, local.E, R(70));
    P(local.E, local.A, local.B, local.C, local.D, R(71));
    P(local.D, local.E, local.A, local.B, local.C, R(72));
    P(local.C, local.D, local.E, local.A, local.B, R(73));
    P(local.B, local.C, local.D, local.E, local.A, R(74));
    P(local.A, local.B, local.C, local.D, local.E, R(75));
    P(local.E, local.A, local.B, local.C, local.D, R(76));
    P(local.D, local.E, local.A, local.B, local.C, R(77));
    P(local.C, local.D, local.E, local.A, local.B, R(78));
    P(local.B, local.C, local.D, local.E, local.A, R(79));

#undef K
#undef F

    ctx->state[0] += local.A;
    ctx->state[1] += local.B;
    ctx->state[2] += local.C;
    ctx->state[3] += local.D;
    ctx->state[4] += local.E;

    /* Zeroise buffers and variables to clear sensitive data from memory. */    
    lib_memsetz(&local, sizeof(local));

    return 0;
}

#endif /* !LIB_SHA1_PROCESS_ALT */

/*
 * SHA-1 process buffer
 */
int lib_sha1_update(lib_sha1_context_t *ctx, const unsigned char *input, size_t ilen) {
    int ret = LIB_ERR_ERROR_CORRUPTION_DETECTED;
    size_t fill;
    uint32_t left;

    if (ilen == 0) {
        return 0;
    }

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < (uint32_t) ilen) {
        ctx->total[1]++;
    }

    if (left && ilen >= fill) {
        memcpy((void *) (ctx->buffer + left), input, fill);

        if ((ret = lib_internal_sha1_process(ctx, ctx->buffer)) != 0) {
            return ret;
        }

        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while (ilen >= 64) {
        if ((ret = lib_internal_sha1_process(ctx, input)) != 0) {
            return ret;
        }

        input += 64;
        ilen  -= 64;
    }

    if (ilen > 0) {
        memcpy((void *) (ctx->buffer + left), input, ilen);
    }

    return 0;
}

/*
 * SHA-1 final digest
 */
int lib_sha1_finish(lib_sha1_context_t *ctx, unsigned char output[20]) {
    int ret = LIB_ERR_ERROR_CORRUPTION_DETECTED;
    uint32_t used;
    uint32_t high, low;

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
        memset(ctx->buffer + used, 0, 64 - used);

        if ((ret = lib_internal_sha1_process(ctx, ctx->buffer)) != 0) {
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

    if ((ret = lib_internal_sha1_process(ctx, ctx->buffer)) != 0) {
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

    ret = 0;

exit:
    lib_sha1_free(ctx);
    return ret;
}

#endif /* !LIB_SHA1_ALT */

/*
 * output = SHA-1( input buffer )
 */
int lib_sha1(const unsigned char *input, size_t ilen, unsigned char output[20]) {
    int ret = LIB_ERR_ERROR_CORRUPTION_DETECTED;
    lib_sha1_context_t ctx;

    lib_sha1_init(&ctx);

    if ((ret = lib_sha1_starts(&ctx)) != 0) {
        goto exit;
    }

    if ((ret = lib_sha1_update(&ctx, input, ilen)) != 0) {
        goto exit;
    }

    if ((ret = lib_sha1_finish(&ctx, output)) != 0) {
        goto exit;
    }

exit:
    lib_sha1_free(&ctx);
    return ret;
}

#if defined(LIB_SELF_TEST)
/*
 * FIPS-180-1 test vectors
 */
static const unsigned char sha1_test_buf[3][57] =
{
    { "abc" },
    { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
    { "" }
};

static const size_t sha1_test_buflen[3] =
{
    3, 56, 1000
};

static const unsigned char sha1_test_sum[3][20] =
{
    { 0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
      0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D },
    { 0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
      0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1 },
    { 0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
      0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F }
};

/*
 * Checkup routine
 */
int lib_sha1_self_test(int verbose) {
    int i, j, buflen, ret = 0;
    unsigned char buf[1024];
    unsigned char sha1sum[20];
    lib_sha1_context_t ctx;

    lib_sha1_init(&ctx);

    /*
     * SHA-1
     */
    for (i = 0; i < 3; i++) {
        if (verbose != 0) {
            lib_printf("  SHA-1 test #%d: ", i + 1);
        }

        if ((ret = lib_sha1_starts(&ctx)) != 0) {
            goto fail;
        }

        if (i == 2) {
            memset(buf, 'a', buflen = 1000);

            for (j = 0; j < 1000; j++) {
                ret = lib_sha1_update(&ctx, buf, buflen);
                if (ret != 0) {
                    goto fail;
                }
            }
        } else {
            ret = lib_sha1_update(&ctx, sha1_test_buf[i],
                                      sha1_test_buflen[i]);
            if (ret != 0) {
                goto fail;
            }
        }

        if ((ret = lib_sha1_finish(&ctx, sha1sum)) != 0) {
            goto fail;
        }

        if (memcmp(sha1sum, sha1_test_sum[i], 20) != 0) {
            ret = 1;
            goto fail;
        }

        if (verbose != 0) {
            lib_printf("passed\n");
        }
    }

    if (verbose != 0) {
        lib_printf("\n");
    }

    goto exit;

fail:
    if (verbose != 0) {
        lib_printf("failed\n");
    }

exit:
    lib_sha1_free(&ctx);

    return ret;
}

#endif /* LIB_SELF_TEST */

#endif /* LIB_SHA1_C */
