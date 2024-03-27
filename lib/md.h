#ifndef PLAZMA_LIB_MD_H
#define PLAZMA_LIB_MD_H

#include <stdlib.h>

#define LIB_MD_C

/* MD5     */
#define LIB_MD_CAN_MD5
#define LIB_CAN_MD5
#define LIB_MD5_C

/* SHA-224 */
#define LIB_MD_CAN_SHA224
#define LIB_CAN_SHA224
#define LIB_SHA224_C

/* SHA-256 */
#define LIB_MD_CAN_SHA256
#define LIB_CAN_SHA256
#define LIB_SHA256_C

#define LIB_ERR_MD_FEATURE_UNAVAILABLE                -0x5080
#define LIB_ERR_MD_BAD_INPUT_DATA                     -0x5100
#define LIB_ERR_MD_ALLOC_FAILED                       -0x5180
#define LIB_ERR_MD_FILE_IO_ERROR                      -0x5200

typedef enum {
    LIB_MD_NONE      = 0,     /**< None.                          */
    LIB_MD_MD5       = 0x03,  /**< The MD5 message digest.        */
    LIB_MD_RIPEMD160 = 0x04,  /**< The RIPEMD-160 message digest. */
    LIB_MD_SHA1      = 0x05,  /**< The SHA-1 message digest.      */
    LIB_MD_SHA224    = 0x08,  /**< The SHA-224 message digest.    */
    LIB_MD_SHA256    = 0x09,  /**< The SHA-256 message digest.    */
    LIB_MD_SHA384    = 0x0a,  /**< The SHA-384 message digest.    */
    LIB_MD_SHA512    = 0x0b,  /**< The SHA-512 message digest.    */
    LIB_MD_SHA3_224  = 0x10,  /**< The SHA3-224 message digest.   */
    LIB_MD_SHA3_256  = 0x11,  /**< The SHA3-256 message digest.   */
    LIB_MD_SHA3_384  = 0x12,  /**< The SHA3-384 message digest.   */
    LIB_MD_SHA3_512  = 0x13,  /**< The SHA3-512 message digest.   */
} lib_md_type_t;

#if defined(LIB_MD_CAN_SHA512) || defined(LIB_MD_CAN_SHA3_512)
#define LIB_MD_MAX_SIZE         64  /* longest known is SHA512 */
#elif defined(LIB_MD_CAN_SHA384) || defined(LIB_MD_CAN_SHA3_384)
#define LIB_MD_MAX_SIZE         48  /* longest known is SHA384 */
#elif defined(LIB_MD_CAN_SHA256) || defined(LIB_MD_CAN_SHA3_256)
#define LIB_MD_MAX_SIZE         32  /* longest known is SHA256 */
#elif defined(LIB_MD_CAN_SHA224) || defined(LIB_MD_CAN_SHA3_224)
#define LIB_MD_MAX_SIZE         28  /* longest known is SHA224 */
#else
#define LIB_MD_MAX_SIZE         20  /* longest known is SHA1 or RIPE MD-160
                                           or smaller (MD5 and earlier) */
#endif

#if defined(LIB_MD_CAN_SHA3_224)
#define LIB_MD_MAX_BLOCK_SIZE         144 /* the longest known is SHA3-224 */
#elif defined(LIB_MD_CAN_SHA3_256)
#define LIB_MD_MAX_BLOCK_SIZE         136
#elif defined(LIB_MD_CAN_SHA512) || defined(LIB_MD_CAN_SHA384)
#define LIB_MD_MAX_BLOCK_SIZE         128
#elif defined(LIB_MD_CAN_SHA3_384)
#define LIB_MD_MAX_BLOCK_SIZE         104
#elif defined(LIB_MD_CAN_SHA3_512)
#define LIB_MD_MAX_BLOCK_SIZE         72
#else
#define LIB_MD_MAX_BLOCK_SIZE         64
#endif

/**
 * Used internally to indicate whether a context uses legacy or PSA.
 *
 * Internal use only.
 */
typedef enum {
    LIB_MD_ENGINE_LEGACY = 0,
    LIB_MD_ENGINE_PSA,
} lib_md_engine_t;

/**
 * Message digest information.
 * Allows message digest functions to be called in a generic way.
 */
struct lib_md_info_t {

    /** Digest identifier */
    lib_md_type_t type;

    /** Output length of the digest function in bytes */
    unsigned char size;

    /** Block length of the digest function in bytes */
    unsigned char block_size;
};


typedef struct lib_md_context_t {

    /** Information about the associated message digest. */
    const lib_md_info_t* md_info;

    /** Are hash operations dispatched to PSA or legacy? */
    lib_md_engine_t engine;

    /** The digest-specific context (MD/SHA/PSA). */
    void* md_ctx;

    /** The HMAC part of the context. */
    void* hmac_ctx;

} lib_md_context_t;

const lib_md_info_t* lib_md_info_from_type(lib_md_type_t md_type);

unsigned char lib_md_get_size(const lib_md_info_t* md_info);

lib_md_type_t lib_md_get_type(const lib_md_info_t* md_info);

void lib_md_init(lib_md_context_t* ctx);

void lib_md_free(lib_md_context_t* ctx);

int lib_md_setup(lib_md_context_t* ctx, const lib_md_info_t* md_info, int hmac);

int lib_md_starts(lib_md_context_t* ctx);

int lib_md_update(lib_md_context_t* ctx, const unsigned char* input, size_t ilen);

int lib_md_finish(lib_md_context_t *ctx, unsigned char* output);

int lib_md(const lib_md_info_t* md_info, const unsigned char* input, size_t ilen, unsigned char *output);

//////

const lib_md_info_t* lib_md_info_from_string(const char* md_name);

const char* lib_md_get_name(const lib_md_info_t* md_info);

const lib_md_info_t* lib_md_info_from_ctx(const lib_md_context_t *ctx);

/////

int lib_md_hmac_starts(lib_md_context_t* ctx, const unsigned char* key, size_t keylen);

int lib_md_hmac_update(lib_md_context_t* ctx, const unsigned char* input, size_t ilen);

int lib_md_hmac_finish(lib_md_context_t* ctx, unsigned char* output);

int lib_md_hmac_reset(lib_md_context_t* ctx);

int lib_md_hmac(const lib_md_info_t* md_info, const unsigned char* key, size_t keylen,
                    const unsigned char* input, size_t ilen,
                    unsigned char* output);

#endif /* PLAZMA_LIB_MD_H */
