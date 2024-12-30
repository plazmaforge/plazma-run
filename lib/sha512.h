#ifndef PLAZMA_SHA512_H
#define PLAZMA_SHA512_H

#include <stddef.h>
#include <stdint.h>

/** SHA-512 input data was malformed. */
#define LIB_ERR_SHA512_BAD_INPUT_DATA                 -0x0075

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lib_sha512_context_t {
    uint64_t total[2];          /*!< The number of Bytes processed. */
    uint64_t state[8];          /*!< The intermediate digest state. */
    unsigned char buffer[128];  /*!< The data block being processed. */
#if defined(LIB_SHA384_C)
    int is384;                  /*!< Determines which function to use:                                                      0: Use SHA-512, or 1: Use SHA-384. */
#endif
} lib_sha512_context_t;

void lib_sha512_init(lib_sha512_context_t *ctx);

void lib_sha512_free(lib_sha512_context_t *ctx);

void lib_sha512_clone(lib_sha512_context_t *dst, const lib_sha512_context_t *src);

int lib_sha512_starts(lib_sha512_context_t *ctx, int is384);

int lib_sha512_update(lib_sha512_context_t *ctx, const unsigned char *input, size_t ilen);

int lib_sha512_finish(lib_sha512_context_t *ctx, unsigned char *output);

int lib_internal_sha512_process(lib_sha512_context_t *ctx, const unsigned char data[128]);

int lib_sha512(const unsigned char *input, size_t ilen, unsigned char *output, int is384);

#if defined(LIB_SELF_TEST)

#if defined(LIB_SHA384_C)
int lib_sha384_self_test(int verbose);
#endif /* LIB_SHA384_C */

#if defined(LIB_SHA512_C)
int lib_sha512_self_test(int verbose);
#endif /* LIB_SHA512_C */

#endif /* LIB_SELF_TEST */

#ifdef __cplusplus
}
#endif

#endif /* PLAZMA_SHA512_H */
