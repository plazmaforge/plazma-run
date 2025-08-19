#ifndef PLAZMA_LIB_MD5_H
#define PLAZMA_LIB_MD5_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lib_md5_context_t {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[4];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
}
lib_md5_context_t;

int lib_md5_init(lib_md5_context_t* ctx);

int lib_md5_free(lib_md5_context_t* ctx);

void lib_md5_clone(lib_md5_context_t* dst, const lib_md5_context_t* src);

int lib_md5_start(lib_md5_context_t* ctx);

int lib_md5_update(lib_md5_context_t* ctx, const unsigned char *input, size_t ilen);

int lib_md5_finish(lib_md5_context_t* ctx, unsigned char output[16]);

int lib_internal_md5_process(lib_md5_context_t* ctx, const unsigned char data[64]);

int lib_md5(const unsigned char* input, size_t ilen, unsigned char output[16]);

#if defined(LIB_SELF_TEST)

int lib_md5_self_test(int verbose);

#endif /* LIB_SELF_TEST */

#ifdef __cplusplus
}
#endif

#endif /* PLAZMA_LIB_MD5_H */
