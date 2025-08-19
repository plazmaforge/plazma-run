#ifndef PLAZMA_LIB_SHA256_H
#define PLAZMA_LIB_SHA256_H

#include <stddef.h>
#include <stdint.h>

#define LIB_MD_SHA224_NAME "SHA224"
#define LIB_MD_SHA224_SIZE 28

#define LIB_MD_SHA256_NAME "SHA256"
#define LIB_MD_SHA256_SIZE 32

/** SHA-256 input data was malformed. */
#define LIB_ERR_SHA256_BAD_INPUT_DATA                 -0x0074

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lib_sha256_context_t {
    uint32_t total[2];          /*!< The number of Bytes processed.  */
    uint32_t state[8];          /*!< The intermediate digest state.  */
    unsigned char buffer[64];   /*!< The data block being processed. */
#if defined(LIB_SHA224_C)
    int is224;                  /*!< Determines which function to use:
                                                    0: Use SHA-256, or 1: Use SHA-224. */
#endif
} lib_sha256_context_t;


int lib_sha256_init(lib_sha256_context_t* ctx);

int lib_sha256_free(lib_sha256_context_t* ctx);

void lib_sha256_clone(lib_sha256_context_t* dst, const lib_sha256_context_t* src);

int lib_sha256_start(lib_sha256_context_t* ctx, int is224);

int lib_sha256_update(lib_sha256_context_t* ctx, const unsigned char* input, size_t ilen);
 
int lib_sha256_finish(lib_sha256_context_t* ctx, unsigned char* output);

int lib_internal_sha256_process(lib_sha256_context_t* ctx, const unsigned char data[64]);

int lib_sha256(const unsigned char* input, size_t ilen, unsigned char* output, int is224);

#if defined(LIB_SELF_TEST)

#if defined(LIB_SHA224_C)
int lib_sha224_self_test(int verbose);
#endif /* LIB_SHA224_C */

#if defined(LIB_SHA256_C)
int lib_sha256_self_test(int verbose);
#endif /* LIB_SHA256_C */

#endif /* LIB_SELF_TEST */

#ifdef __cplusplus
}
#endif

#endif /* PLAZMA_LIB_SHA256_H */
