/*
 *  The SHA-512 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#include "common.h"
#include "memlib.h"

#include "md.h"

#if defined(LIB_SHA512_C) || defined(LIB_SHA384_C)

#define LIB_SELF_TEST

#include "sha512.h"
#include "error.h"

#if defined(LIB_SELF_TEST) 
#include <stdio.h>
#endif


#if defined(_MSC_VER) || defined(__WATCOMC__)
  #define UL64(x) x##ui64
#else
  #define UL64(x) x##ULL
#endif

#include <string.h>

#define SHA512_BLOCK_SIZE 128

#if defined(LIB_SHA512_SMALLER)
static void sha512_put_uint64_be(uint64_t n, unsigned char *b, uint8_t i) {
    LIB_PUT_UINT64_BE(n, b, i);
}
#else
#define sha512_put_uint64_be    LIB_PUT_UINT64_BE
#endif /* LIB_SHA512_SMALLER */

void lib_sha512_init(lib_sha512_context_t *ctx) {
    lib_memsetz(ctx, sizeof(lib_sha512_context_t));
}

void lib_sha512_free(lib_sha512_context_t *ctx) {
    lib_memsetz(ctx, sizeof(lib_sha512_context_t));
}

void lib_sha512_clone(lib_sha512_context_t* dst, const lib_sha512_context_t* src) {
    *dst = *src;
}

/*
 * SHA-512 context setup
 */
int lib_sha512_starts(lib_sha512_context_t *ctx, int is384) {
#if defined(LIB_SHA384_C) && defined(LIB_SHA512_C)
    if (is384 != 0 && is384 != 1) {
        return LIB_ERR_SHA512_BAD_INPUT_DATA;
    }
#elif defined(LIB_SHA512_C)
    if (is384 != 0) {
        return LIB_ERR_SHA512_BAD_INPUT_DATA;
    }
#else /* defined LIB_SHA384_C only */
    if (is384 == 0) {
        return LIB_ERR_SHA512_BAD_INPUT_DATA;
    }
#endif

    ctx->total[0] = 0;
    ctx->total[1] = 0;

    if (is384 == 0) {
#if defined(LIB_SHA512_C)
        ctx->state[0] = UL64(0x6A09E667F3BCC908);
        ctx->state[1] = UL64(0xBB67AE8584CAA73B);
        ctx->state[2] = UL64(0x3C6EF372FE94F82B);
        ctx->state[3] = UL64(0xA54FF53A5F1D36F1);
        ctx->state[4] = UL64(0x510E527FADE682D1);
        ctx->state[5] = UL64(0x9B05688C2B3E6C1F);
        ctx->state[6] = UL64(0x1F83D9ABFB41BD6B);
        ctx->state[7] = UL64(0x5BE0CD19137E2179);
#endif /* LIB_SHA512_C */
    } else {
#if defined(LIB_SHA384_C)
        ctx->state[0] = UL64(0xCBBB9D5DC1059ED8);
        ctx->state[1] = UL64(0x629A292A367CD507);
        ctx->state[2] = UL64(0x9159015A3070DD17);
        ctx->state[3] = UL64(0x152FECD8F70E5939);
        ctx->state[4] = UL64(0x67332667FFC00B31);
        ctx->state[5] = UL64(0x8EB44A8768581511);
        ctx->state[6] = UL64(0xDB0C2E0D64F98FA7);
        ctx->state[7] = UL64(0x47B5481DBEFA4FA4);
#endif /* LIB_SHA384_C */
    }

#if defined(LIB_SHA384_C)
    ctx->is384 = is384;
#endif

    return 0;
}

#if !defined(LIB_SHA512_PROCESS_ALT)

/*
 * Round constants
 */
static const uint64_t K[80] =
{
    UL64(0x428A2F98D728AE22),  UL64(0x7137449123EF65CD),
    UL64(0xB5C0FBCFEC4D3B2F),  UL64(0xE9B5DBA58189DBBC),
    UL64(0x3956C25BF348B538),  UL64(0x59F111F1B605D019),
    UL64(0x923F82A4AF194F9B),  UL64(0xAB1C5ED5DA6D8118),
    UL64(0xD807AA98A3030242),  UL64(0x12835B0145706FBE),
    UL64(0x243185BE4EE4B28C),  UL64(0x550C7DC3D5FFB4E2),
    UL64(0x72BE5D74F27B896F),  UL64(0x80DEB1FE3B1696B1),
    UL64(0x9BDC06A725C71235),  UL64(0xC19BF174CF692694),
    UL64(0xE49B69C19EF14AD2),  UL64(0xEFBE4786384F25E3),
    UL64(0x0FC19DC68B8CD5B5),  UL64(0x240CA1CC77AC9C65),
    UL64(0x2DE92C6F592B0275),  UL64(0x4A7484AA6EA6E483),
    UL64(0x5CB0A9DCBD41FBD4),  UL64(0x76F988DA831153B5),
    UL64(0x983E5152EE66DFAB),  UL64(0xA831C66D2DB43210),
    UL64(0xB00327C898FB213F),  UL64(0xBF597FC7BEEF0EE4),
    UL64(0xC6E00BF33DA88FC2),  UL64(0xD5A79147930AA725),
    UL64(0x06CA6351E003826F),  UL64(0x142929670A0E6E70),
    UL64(0x27B70A8546D22FFC),  UL64(0x2E1B21385C26C926),
    UL64(0x4D2C6DFC5AC42AED),  UL64(0x53380D139D95B3DF),
    UL64(0x650A73548BAF63DE),  UL64(0x766A0ABB3C77B2A8),
    UL64(0x81C2C92E47EDAEE6),  UL64(0x92722C851482353B),
    UL64(0xA2BFE8A14CF10364),  UL64(0xA81A664BBC423001),
    UL64(0xC24B8B70D0F89791),  UL64(0xC76C51A30654BE30),
    UL64(0xD192E819D6EF5218),  UL64(0xD69906245565A910),
    UL64(0xF40E35855771202A),  UL64(0x106AA07032BBD1B8),
    UL64(0x19A4C116B8D2D0C8),  UL64(0x1E376C085141AB53),
    UL64(0x2748774CDF8EEB99),  UL64(0x34B0BCB5E19B48A8),
    UL64(0x391C0CB3C5C95A63),  UL64(0x4ED8AA4AE3418ACB),
    UL64(0x5B9CCA4F7763E373),  UL64(0x682E6FF3D6B2B8A3),
    UL64(0x748F82EE5DEFB2FC),  UL64(0x78A5636F43172F60),
    UL64(0x84C87814A1F0AB72),  UL64(0x8CC702081A6439EC),
    UL64(0x90BEFFFA23631E28),  UL64(0xA4506CEBDE82BDE9),
    UL64(0xBEF9A3F7B2C67915),  UL64(0xC67178F2E372532B),
    UL64(0xCA273ECEEA26619C),  UL64(0xD186B8C721C0C207),
    UL64(0xEADA7DD6CDE0EB1E),  UL64(0xF57D4F7FEE6ED178),
    UL64(0x06F067AA72176FBA),  UL64(0x0A637DC5A2C898A6),
    UL64(0x113F9804BEF90DAE),  UL64(0x1B710B35131C471B),
    UL64(0x28DB77F523047D84),  UL64(0x32CAAB7B40C72493),
    UL64(0x3C9EBE0A15C9BEBC),  UL64(0x431D67C49C100D4C),
    UL64(0x4CC5D4BECB3E42B6),  UL64(0x597F299CFC657E2A),
    UL64(0x5FCB6FAB3AD6FAEC),  UL64(0x6C44198C4A475817)
};
#endif

#define lib_internal_sha512_process_many_c lib_internal_sha512_process_many
#define lib_internal_sha512_process_c      lib_internal_sha512_process

#if !defined(LIB_SHA512_PROCESS_ALT) && !defined(LIB_SHA512_USE_A64_CRYPTO_ONLY)


int lib_internal_sha512_process_c(lib_sha512_context_t *ctx, const unsigned char data[SHA512_BLOCK_SIZE]) {
    int i;
    struct {
        uint64_t temp1, temp2, W[80];
        uint64_t A[8];
    } local;

#define  SHR(x, n) ((x) >> (n))
#define ROTR(x, n) (SHR((x), (n)) | ((x) << (64 - (n))))

#define S0(x) (ROTR(x, 1) ^ ROTR(x, 8) ^  SHR(x, 7))
#define S1(x) (ROTR(x, 19) ^ ROTR(x, 61) ^  SHR(x, 6))

#define S2(x) (ROTR(x, 28) ^ ROTR(x, 34) ^ ROTR(x, 39))
#define S3(x) (ROTR(x, 14) ^ ROTR(x, 18) ^ ROTR(x, 41))

#define F0(x, y, z) (((x) & (y)) | ((z) & ((x) | (y))))
#define F1(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))

#define P(a, b, c, d, e, f, g, h, x, K)                                      \
    do                                                              \
    {                                                               \
        local.temp1 = (h) + S3(e) + F1((e), (f), (g)) + (K) + (x);    \
        local.temp2 = S2(a) + F0((a), (b), (c));                      \
        (d) += local.temp1; (h) = local.temp1 + local.temp2;        \
    } while (0)

    for (i = 0; i < 8; i++) {
        local.A[i] = ctx->state[i];
    }

#if defined(LIB_SHA512_SMALLER)
    for (i = 0; i < 80; i++) {
        if (i < 16) {
            local.W[i] = LIB_GET_UINT64_BE(data, i << 3);
        } else {
            local.W[i] = S1(local.W[i -  2]) + local.W[i -  7] +
                         S0(local.W[i - 15]) + local.W[i - 16];
        }

        P(local.A[0], local.A[1], local.A[2], local.A[3], local.A[4],
          local.A[5], local.A[6], local.A[7], local.W[i], K[i]);

        local.temp1 = local.A[7]; local.A[7] = local.A[6];
        local.A[6] = local.A[5]; local.A[5] = local.A[4];
        local.A[4] = local.A[3]; local.A[3] = local.A[2];
        local.A[2] = local.A[1]; local.A[1] = local.A[0];
        local.A[0] = local.temp1;
    }
#else /* LIB_SHA512_SMALLER */
    for (i = 0; i < 16; i++) {
        local.W[i] = LIB_GET_UINT64_BE(data, i << 3);
    }

    for (; i < 80; i++) {
        local.W[i] = S1(local.W[i -  2]) + local.W[i -  7] +
                     S0(local.W[i - 15]) + local.W[i - 16];
    }

    i = 0;
    do {
        P(local.A[0], local.A[1], local.A[2], local.A[3], local.A[4],
          local.A[5], local.A[6], local.A[7], local.W[i], K[i]); i++;
        P(local.A[7], local.A[0], local.A[1], local.A[2], local.A[3],
          local.A[4], local.A[5], local.A[6], local.W[i], K[i]); i++;
        P(local.A[6], local.A[7], local.A[0], local.A[1], local.A[2],
          local.A[3], local.A[4], local.A[5], local.W[i], K[i]); i++;
        P(local.A[5], local.A[6], local.A[7], local.A[0], local.A[1],
          local.A[2], local.A[3], local.A[4], local.W[i], K[i]); i++;
        P(local.A[4], local.A[5], local.A[6], local.A[7], local.A[0],
          local.A[1], local.A[2], local.A[3], local.W[i], K[i]); i++;
        P(local.A[3], local.A[4], local.A[5], local.A[6], local.A[7],
          local.A[0], local.A[1], local.A[2], local.W[i], K[i]); i++;
        P(local.A[2], local.A[3], local.A[4], local.A[5], local.A[6],
          local.A[7], local.A[0], local.A[1], local.W[i], K[i]); i++;
        P(local.A[1], local.A[2], local.A[3], local.A[4], local.A[5],
          local.A[6], local.A[7], local.A[0], local.W[i], K[i]); i++;
    } while (i < 80);
#endif /* LIB_SHA512_SMALLER */

    for (i = 0; i < 8; i++) {
        ctx->state[i] += local.A[i];
    }

    /* Zeroise buffers and variables to clear sensitive data from memory. */
    lib_memsetz(&local, sizeof(local));
    return 0;
}

#endif /* !LIB_SHA512_PROCESS_ALT && !LIB_SHA512_USE_A64_CRYPTO_ONLY */


#if !defined(LIB_SHA512_USE_A64_CRYPTO_ONLY)

static size_t lib_internal_sha512_process_many_c(lib_sha512_context_t *ctx, const uint8_t *data, size_t len) {
    size_t processed = 0;

    while (len >= SHA512_BLOCK_SIZE) {
        if (lib_internal_sha512_process_c(ctx, data) != 0) {
            return 0;
        }

        data += SHA512_BLOCK_SIZE;
        len  -= SHA512_BLOCK_SIZE;

        processed += SHA512_BLOCK_SIZE;
    }

    return processed;
}

#endif /* !LIB_SHA512_USE_A64_CRYPTO_ONLY */


/*
 * SHA-512 process buffer
 */
int lib_sha512_update(lib_sha512_context_t *ctx,
                          const unsigned char *input,
                          size_t ilen)
{
    int ret = LIB_ERR_ERROR_CORRUPTION_DETECTED;
    size_t fill;
    unsigned int left;

    if (ilen == 0) {
        return 0;
    }

    left = (unsigned int) (ctx->total[0] & 0x7F);
    fill = SHA512_BLOCK_SIZE - left;

    ctx->total[0] += (uint64_t) ilen;

    if (ctx->total[0] < (uint64_t) ilen) {
        ctx->total[1]++;
    }

    if (left && ilen >= fill) {
        memcpy((void *) (ctx->buffer + left), input, fill);

        if ((ret = lib_internal_sha512_process(ctx, ctx->buffer)) != 0) {
            return ret;
        }

        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while (ilen >= SHA512_BLOCK_SIZE) {
        size_t processed =
            lib_internal_sha512_process_many(ctx, input, ilen);
        if (processed < SHA512_BLOCK_SIZE) {
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
 * SHA-512 final digest
 */
int lib_sha512_finish(lib_sha512_context_t *ctx, unsigned char *output) {
    int ret = LIB_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned used;
    uint64_t high, low;
    int truncated = 0;

    /*
     * Add padding: 0x80 then 0x00 until 16 bytes remain for the length
     */
    used = ctx->total[0] & 0x7F;

    ctx->buffer[used++] = 0x80;

    if (used <= 112) {
        /* Enough room for padding + length in current block */
        memset(ctx->buffer + used, 0, 112 - used);
    } else {
        /* We'll need an extra block */
        memset(ctx->buffer + used, 0, SHA512_BLOCK_SIZE - used);

        if ((ret = lib_internal_sha512_process(ctx, ctx->buffer)) != 0) {
            goto exit;
        }

        memset(ctx->buffer, 0, 112);
    }

    /*
     * Add message length
     */
    high = (ctx->total[0] >> 61)
           | (ctx->total[1] <<  3);
    low  = (ctx->total[0] <<  3);

    sha512_put_uint64_be(high, ctx->buffer, 112);
    sha512_put_uint64_be(low,  ctx->buffer, 120);

    if ((ret = lib_internal_sha512_process(ctx, ctx->buffer)) != 0) {
        goto exit;
    }

    /*
     * Output final state
     */
    sha512_put_uint64_be(ctx->state[0], output,  0);
    sha512_put_uint64_be(ctx->state[1], output,  8);
    sha512_put_uint64_be(ctx->state[2], output, 16);
    sha512_put_uint64_be(ctx->state[3], output, 24);
    sha512_put_uint64_be(ctx->state[4], output, 32);
    sha512_put_uint64_be(ctx->state[5], output, 40);

#if defined(LIB_SHA384_C)
    truncated = ctx->is384;
#endif
    if (!truncated) {
        sha512_put_uint64_be(ctx->state[6], output, 48);
        sha512_put_uint64_be(ctx->state[7], output, 56);
    }

    ret = 0;

exit:
    lib_sha512_free(ctx);
    return ret;
}

//#endif /* !LIB_SHA512_ALT */

/*
 * output = SHA-512( input buffer )
 */
int lib_sha512(const unsigned char *input, size_t ilen, unsigned char *output, int is384) {
    int ret = LIB_ERR_ERROR_CORRUPTION_DETECTED;
    lib_sha512_context_t ctx;

#if defined(LIB_SHA384_C) && defined(LIB_SHA512_C)
    if (is384 != 0 && is384 != 1) {
        return LIB_ERR_SHA512_BAD_INPUT_DATA;
    }
#elif defined(LIB_SHA512_C)
    if (is384 != 0) {
        return LIB_ERR_SHA512_BAD_INPUT_DATA;
    }
#else /* defined LIB_SHA384_C only */
    if (is384 == 0) {
        return LIB_ERR_SHA512_BAD_INPUT_DATA;
    }
#endif

    lib_sha512_init(&ctx);

    if ((ret = lib_sha512_starts(&ctx, is384)) != 0) {
        goto exit;
    }

    if ((ret = lib_sha512_update(&ctx, input, ilen)) != 0) {
        goto exit;
    }

    if ((ret = lib_sha512_finish(&ctx, output)) != 0) {
        goto exit;
    }

exit:
    lib_sha512_free(&ctx);

    return ret;
}

#if defined(LIB_SELF_TEST)

/*
 * FIPS-180-2 test vectors
 */
static const unsigned char sha_test_buf[3][113] =
{
    { "abc" },
    {
        "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"
    },
    { "" }
};

static const size_t sha_test_buflen[3] =
{
    3, 112, 1000
};

typedef const unsigned char (sha_test_sum_t)[64];

/*
 * SHA-384 test vectors
 */
#if defined(LIB_SHA384_C)
static sha_test_sum_t sha384_test_sum[] =
{
    { 0xCB, 0x00, 0x75, 0x3F, 0x45, 0xA3, 0x5E, 0x8B,
      0xB5, 0xA0, 0x3D, 0x69, 0x9A, 0xC6, 0x50, 0x07,
      0x27, 0x2C, 0x32, 0xAB, 0x0E, 0xDE, 0xD1, 0x63,
      0x1A, 0x8B, 0x60, 0x5A, 0x43, 0xFF, 0x5B, 0xED,
      0x80, 0x86, 0x07, 0x2B, 0xA1, 0xE7, 0xCC, 0x23,
      0x58, 0xBA, 0xEC, 0xA1, 0x34, 0xC8, 0x25, 0xA7 },
    { 0x09, 0x33, 0x0C, 0x33, 0xF7, 0x11, 0x47, 0xE8,
      0x3D, 0x19, 0x2F, 0xC7, 0x82, 0xCD, 0x1B, 0x47,
      0x53, 0x11, 0x1B, 0x17, 0x3B, 0x3B, 0x05, 0xD2,
      0x2F, 0xA0, 0x80, 0x86, 0xE3, 0xB0, 0xF7, 0x12,
      0xFC, 0xC7, 0xC7, 0x1A, 0x55, 0x7E, 0x2D, 0xB9,
      0x66, 0xC3, 0xE9, 0xFA, 0x91, 0x74, 0x60, 0x39 },
    { 0x9D, 0x0E, 0x18, 0x09, 0x71, 0x64, 0x74, 0xCB,
      0x08, 0x6E, 0x83, 0x4E, 0x31, 0x0A, 0x4A, 0x1C,
      0xED, 0x14, 0x9E, 0x9C, 0x00, 0xF2, 0x48, 0x52,
      0x79, 0x72, 0xCE, 0xC5, 0x70, 0x4C, 0x2A, 0x5B,
      0x07, 0xB8, 0xB3, 0xDC, 0x38, 0xEC, 0xC4, 0xEB,
      0xAE, 0x97, 0xDD, 0xD8, 0x7F, 0x3D, 0x89, 0x85 }
};
#endif /* LIB_SHA384_C */

/*
 * SHA-512 test vectors
 */
#if defined(LIB_SHA512_C)
static sha_test_sum_t sha512_test_sum[] =
{
    { 0xDD, 0xAF, 0x35, 0xA1, 0x93, 0x61, 0x7A, 0xBA,
      0xCC, 0x41, 0x73, 0x49, 0xAE, 0x20, 0x41, 0x31,
      0x12, 0xE6, 0xFA, 0x4E, 0x89, 0xA9, 0x7E, 0xA2,
      0x0A, 0x9E, 0xEE, 0xE6, 0x4B, 0x55, 0xD3, 0x9A,
      0x21, 0x92, 0x99, 0x2A, 0x27, 0x4F, 0xC1, 0xA8,
      0x36, 0xBA, 0x3C, 0x23, 0xA3, 0xFE, 0xEB, 0xBD,
      0x45, 0x4D, 0x44, 0x23, 0x64, 0x3C, 0xE8, 0x0E,
      0x2A, 0x9A, 0xC9, 0x4F, 0xA5, 0x4C, 0xA4, 0x9F },
    { 0x8E, 0x95, 0x9B, 0x75, 0xDA, 0xE3, 0x13, 0xDA,
      0x8C, 0xF4, 0xF7, 0x28, 0x14, 0xFC, 0x14, 0x3F,
      0x8F, 0x77, 0x79, 0xC6, 0xEB, 0x9F, 0x7F, 0xA1,
      0x72, 0x99, 0xAE, 0xAD, 0xB6, 0x88, 0x90, 0x18,
      0x50, 0x1D, 0x28, 0x9E, 0x49, 0x00, 0xF7, 0xE4,
      0x33, 0x1B, 0x99, 0xDE, 0xC4, 0xB5, 0x43, 0x3A,
      0xC7, 0xD3, 0x29, 0xEE, 0xB6, 0xDD, 0x26, 0x54,
      0x5E, 0x96, 0xE5, 0x5B, 0x87, 0x4B, 0xE9, 0x09 },
    { 0xE7, 0x18, 0x48, 0x3D, 0x0C, 0xE7, 0x69, 0x64,
      0x4E, 0x2E, 0x42, 0xC7, 0xBC, 0x15, 0xB4, 0x63,
      0x8E, 0x1F, 0x98, 0xB1, 0x3B, 0x20, 0x44, 0x28,
      0x56, 0x32, 0xA8, 0x03, 0xAF, 0xA9, 0x73, 0xEB,
      0xDE, 0x0F, 0xF2, 0x44, 0x87, 0x7E, 0xA6, 0x0A,
      0x4C, 0xB0, 0x43, 0x2C, 0xE5, 0x77, 0xC3, 0x1B,
      0xEB, 0x00, 0x9C, 0x5C, 0x2C, 0x49, 0xAA, 0x2E,
      0x4E, 0xAD, 0xB2, 0x17, 0xAD, 0x8C, 0xC0, 0x9B }
};
#endif /* LIB_SHA512_C */

static int lib_sha512_common_self_test(int verbose, int is384) {
    int i, buflen, ret = 0;
    unsigned char *buf;
    unsigned char sha512sum[64];
    lib_sha512_context_t ctx;

#if defined(LIB_SHA384_C) && defined(LIB_SHA512_C)
    sha_test_sum_t *sha_test_sum = (is384) ? sha384_test_sum : sha512_test_sum;
#elif defined(LIB_SHA512_C)
    sha_test_sum_t *sha_test_sum = sha512_test_sum;
#else
    sha_test_sum_t *sha_test_sum = sha384_test_sum;
#endif

    buf = (unsigned char *) calloc(1024, sizeof(unsigned char));
    if (NULL == buf) {
        if (verbose != 0) {
            printf("Buffer allocation failed\n");
        }

        return 1;
    }

    lib_sha512_init(&ctx);

    for (i = 0; i < 3; i++) {
        if (verbose != 0) {
            printf("  SHA-%d test #%d: ", 512 - is384 * 128, i + 1);
        }

        if ((ret = lib_sha512_starts(&ctx, is384)) != 0) {
            goto fail;
        }

        if (i == 2) {
            memset(buf, 'a', buflen = 1000);

            for (int j = 0; j < 1000; j++) {
                ret = lib_sha512_update(&ctx, buf, buflen);
                if (ret != 0) {
                    goto fail;
                }
            }
        } else {
            ret = lib_sha512_update(&ctx, sha_test_buf[i],
                                        sha_test_buflen[i]);
            if (ret != 0) {
                goto fail;
            }
        }

        if ((ret = lib_sha512_finish(&ctx, sha512sum)) != 0) {
            goto fail;
        }

        if (memcmp(sha512sum, sha_test_sum[i], 64 - is384 * 16) != 0) {
            ret = 1;
            goto fail;
        }

        if (verbose != 0) {
            printf("passed\n");
        }
    }

    if (verbose != 0) {
        printf("\n");
    }

    goto exit;

fail:
    if (verbose != 0) {
        printf("failed\n");
    }

exit:
    lib_sha512_free(&ctx);
    free(buf);

    return ret;
}

#if defined(LIB_SHA512_C)
int lib_sha512_self_test(int verbose) {
    return lib_sha512_common_self_test(verbose, 0);
}
#endif /* LIB_SHA512_C */

#if defined(LIB_SHA384_C)
int lib_sha384_self_test(int verbose) {
    return lib_sha512_common_self_test(verbose, 1);
}
#endif /* LIB_SHA384_C */

#undef ARRAY_LENGTH

// int main(int argc, char *argv[]) {
//     lib_sha384_self_test(1);
//     lib_sha512_self_test(1);
// }

#endif /* LIB_SELF_TEST */

#endif /* LIB_SHA512_C || LIB_SHA384_C */
