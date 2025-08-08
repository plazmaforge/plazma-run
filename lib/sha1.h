#ifndef PLAZMA_SHA1_H
#define PLAZMA_SHA1_H

#include <stddef.h>
#include <stdint.h>

/** SHA-1 input data was malformed. */
#define LIB_ERR_SHA1_BAD_INPUT_DATA                   -0x0073

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lib_sha1_context_t {
    uint32_t total[2];          /*!< The number of Bytes processed.  */
    uint32_t state[5];          /*!< The intermediate digest state.  */
    unsigned char buffer[64];   /*!< The data block being processed. */
} lib_sha1_context_t;

void lib_sha1_init(lib_sha1_context_t *ctx);

void lib_sha1_free(lib_sha1_context_t *ctx);

void lib_sha1_clone(lib_sha1_context_t *dst, const lib_sha1_context_t *src);

int lib_sha1_starts(lib_sha1_context_t *ctx);

int lib_sha1_update(lib_sha1_context_t *ctx, const unsigned char *input, size_t ilen);

int lib_internal_sha1_process(lib_sha1_context_t *ctx, const unsigned char data[64]);

int lib_sha1(const unsigned char *input, size_t ilen, unsigned char output[20]);

int lib_sha1_finish(lib_sha1_context_t *ctx, unsigned char output[20]);

#if defined(LIB_SELF_TEST)

int lib_sha1_self_test(int verbose);

#endif /* LIB_SELF_TEST */

#ifdef __cplusplus
}
#endif

#endif /* PLAZMA_SHA1_H */
