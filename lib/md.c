#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "error.h"
#include "memlib.h"

#include "md.h"

#if defined(LIB_MD_CAN_MD5)
#include "md5.h"
#endif

#if defined(LIB_MD_CAN_SHA1)
#include "sha1.h"
#endif

#if defined(LIB_MD_CAN_SHA256)
#include "sha256.h"
#endif

#if defined(LIB_MD_CAN_SHA512)
#include "sha512.h"
#endif

#if defined(LIB_MD_C)
#define MD_INFO(type, out_size, block_size) type, out_size, block_size,
#else
#define MD_INFO(type, out_size, block_size) type, out_size,
#endif

#if defined(LIB_MD_CAN_MD5)
static const lib_md_info_t lib_md5_info = {
    MD_INFO(LIB_MD_MD5, 16, 64)
};
#endif

#if defined(LIB_MD_CAN_SHA1)
static const lib_md_info_t lib_sha1_info = {
    MD_INFO(LIB_MD_SHA1, 20, 64)
};
#endif

#if defined(LIB_MD_CAN_SHA224)
static const lib_md_info_t lib_sha224_info = {
    MD_INFO(LIB_MD_SHA224, 28, 64)
};
#endif

#if defined(LIB_MD_CAN_SHA256)
 static const lib_md_info_t lib_sha256_info = {
     MD_INFO(LIB_MD_SHA256, 32, 64)
 };
#endif

#if defined(LIB_MD_CAN_SHA384)
static const lib_md_info_t lib_sha384_info = {
    MD_INFO(LIB_MD_SHA384, 48, 128)
};
#endif

#if defined(LIB_MD_CAN_SHA512)
static const lib_md_info_t lib_sha512_info = {
    MD_INFO(LIB_MD_SHA512, 64, 128)
};
#endif


const lib_md_info_t* lib_md_info_from_type(lib_md_type_t md_type) {
    switch (md_type) {

#if defined(LIB_MD_CAN_MD5)
        case LIB_MD_MD5:
            return &lib_md5_info;
#endif
#if defined(LIB_MD_CAN_SHA224)
        case LIB_MD_SHA224:
            return &lib_sha224_info;
#endif
#if defined(LIB_MD_CAN_SHA256)
        case LIB_MD_SHA256:
            return &lib_sha256_info;
#endif
#if defined(LIB_MD_CAN_SHA384)
        case LIB_MD_SHA384:
            return &lib_sha384_info;
#endif
#if defined(LIB_MD_CAN_SHA512)
        case LIB_MD_SHA512:
            return &lib_sha512_info;
#endif
        default:
            return NULL;
    }
}

unsigned char lib_md_get_size(const lib_md_info_t* md_info) {
    return md_info ? md_info->size : 0;
}

lib_md_type_t lib_md_get_type(const lib_md_info_t* md_info) {
    return md_info ? md_info->type : LIB_MD_NONE;
}

////

void lib_md_init(lib_md_context_t* ctx) {
    /* Note: this sets engine (if present) to lib_md_ENGINE_LEGACY */
    memset(ctx, 0, sizeof(lib_md_context_t));
}

void lib_md_free(lib_md_context_t* ctx) {
    if (ctx == NULL || ctx->md_info == NULL) {
        return;
    }

    if (ctx->md_ctx != NULL) {

        switch (ctx->md_info->type) {
#if defined(LIB_MD5_C)
        case LIB_MD_MD5:
            lib_md5_free((lib_md5_context_t*) ctx->md_ctx);
            break;
#endif
#if defined(LIB_SHA224_C)
        case LIB_MD_SHA224:
            lib_sha256_free((lib_sha256_context_t*) ctx->md_ctx);
            break;
#endif
#if defined(LIB_SHA256_C)
        case LIB_MD_SHA256:
            lib_sha256_free((lib_sha256_context_t*) ctx->md_ctx);
            break;
#endif
#if defined(LIB_SHA384_C)
            case LIB_MD_SHA384:
                lib_sha512_free((lib_sha512_context_t*) ctx->md_ctx);
                break;
#endif
#if defined(LIB_SHA512_C)
            case LIB_MD_SHA512:
                lib_sha512_free((lib_sha512_context_t*) ctx->md_ctx);
                break;
#endif

        default:
            /* Shouldn't happen */
            break;
        }
        free(ctx->md_ctx);
    }

    #if defined(LIB_MD_C)
        if (ctx->hmac_ctx != NULL) {
            lib_freez(ctx->hmac_ctx, 2 * ctx->md_info->block_size);
        }
    #endif

    lib_memsetz(ctx, sizeof(lib_md_context_t));
}

#define ALLOC(type)                                                        \
    do {                                                                   \
        ctx->md_ctx = /*lib*/ calloc(1, sizeof(lib_##type##_context_t));   \
        if (ctx->md_ctx == NULL)                                           \
        return -1;                                                         \
    }                                                                      \
    while (0)

// lib_##type##_init(((lib_##type##_context_t*)) ctx->md_ctx);                                \
// return LIB_ERR_MD_ALLOC_FAILED;

int lib_md_setup(lib_md_context_t* ctx, const lib_md_info_t* md_info, int hmac) {
    
#if defined(LIB_MD_C)
    if (ctx == NULL) {
        return LIB_ERR_MD_BAD_INPUT_DATA;
    }
#endif
    if (md_info == NULL) {
        return LIB_ERR_MD_BAD_INPUT_DATA;
    }

    ctx->md_info = md_info;
    ctx->md_ctx = NULL;
#if defined(LIB_MD_C)
    ctx->hmac_ctx = NULL;
#else
    if (hmac != 0) {
        return LIB_ERR_MD_BAD_INPUT_DATA;
    }
#endif

    switch (md_info->type) {
#if defined(LIB_MD5_C)
        case LIB_MD_MD5:
            ALLOC(md5);
            lib_md5_init((lib_md5_context_t*) ctx->md_ctx);
            break;
#endif
#if defined(LIB_SHA224_C)
        case LIB_MD_SHA224:
            ALLOC(sha256);
            lib_sha256_init((lib_sha256_context_t*) ctx->md_ctx);
            break;
#endif
#if defined(LIB_SHA256_C)
        case LIB_MD_SHA256:
            ALLOC(sha256);
            lib_sha256_init((lib_sha256_context_t*) ctx->md_ctx);
            break;
#endif
#if defined(LIB_SHA384_C)
        case LIB_MD_SHA384:
            ALLOC(sha512);
            lib_sha512_init((lib_sha512_context_t*) ctx->md_ctx);
            break;
#endif
#if defined(LIB_SHA512_C)
        case LIB_MD_SHA512:
            ALLOC(sha512);
            lib_sha512_init((lib_sha512_context_t*) ctx->md_ctx);
            break;
#endif

        default:
            return LIB_ERR_MD_BAD_INPUT_DATA;
    }

#if defined(LIB_MD_C)
    if (hmac != 0) {
        ctx->hmac_ctx = /*lib*/ calloc(2, md_info->block_size);
        if (ctx->hmac_ctx == NULL) {
            lib_md_free(ctx);
            return LIB_ERR_MD_ALLOC_FAILED;
        }
    }
#endif

    return 0;
}

#undef ALLOC

////

int lib_md_start(lib_md_context_t* ctx) {
#if defined(LIB_MD_C)
    if (ctx == NULL || ctx->md_info == NULL) {
        return LIB_ERR_MD_BAD_INPUT_DATA;
    }
#endif

    switch (ctx->md_info->type) {
#if defined(LIB_MD5_C)
        case LIB_MD_MD5:
            return lib_md5_start((lib_md5_context_t*) ctx->md_ctx);
#endif
#if defined(LIB_SHA224_C)
        case LIB_MD_SHA224:
            return lib_sha256_start((lib_sha256_context_t*) ctx->md_ctx, 1);
#endif
#if defined(LIB_SHA256_C)
        case LIB_MD_SHA256:
            return lib_sha256_start((lib_sha256_context_t*) ctx->md_ctx, 0);
#endif
#if defined(LIB_SHA384_C)
        case LIB_MD_SHA384:
            return lib_sha512_start((lib_sha512_context_t*) ctx->md_ctx, 1);
#endif
#if defined(LIB_SHA512_C)
        case LIB_MD_SHA512:
            return lib_sha512_start((lib_sha512_context_t*) ctx->md_ctx, 0);
#endif

        default:
            return LIB_ERR_MD_BAD_INPUT_DATA;
    }
}

int lib_md_update(lib_md_context_t* ctx, const unsigned char* idata, size_t isize)  {
#if defined(LIB_MD_C)
    if (ctx == NULL || ctx->md_info == NULL) {
        return LIB_ERR_MD_BAD_INPUT_DATA;
    }
#endif

    switch (ctx->md_info->type) {
#if defined(LIB_MD5_C)
        case LIB_MD_MD5:
            return lib_md5_update((lib_md5_context_t*) ctx->md_ctx, idata, isize);
#endif
#if defined(LIB_SHA224_C)
        case LIB_MD_SHA224:
            return lib_sha256_update((lib_sha256_context_t*) ctx->md_ctx, idata, isize);
#endif
#if defined(LIB_SHA256_C)
        case LIB_MD_SHA256:
            return lib_sha256_update((lib_sha256_context_t*) ctx->md_ctx, idata, isize);
#endif
#if defined(LIB_SHA384_C)
        case LIB_MD_SHA384:
            return lib_sha512_update((lib_sha512_context_t*) ctx->md_ctx, idata, isize);
#endif
#if defined(LIB_SHA512_C)
        case LIB_MD_SHA512:
            return lib_sha512_update((lib_sha512_context_t*) ctx->md_ctx, idata, isize);
#endif

        default:
            return LIB_ERR_MD_BAD_INPUT_DATA;
    }
}

int lib_md_finish(lib_md_context_t* ctx, unsigned char* odata) {
#if defined(LIB_MD_C)
    if (ctx == NULL || ctx->md_info == NULL) {
        return LIB_ERR_MD_BAD_INPUT_DATA;
    }
#endif

    switch (ctx->md_info->type) {
#if defined(LIB_MD5_C)
        case LIB_MD_MD5:
            return lib_md5_finish((lib_md5_context_t*) ctx->md_ctx, odata);
#endif
#if defined(LIB_SHA224_C)
        case LIB_MD_SHA224:
            return lib_sha256_finish((lib_sha256_context_t*) ctx->md_ctx, odata);
#endif
#if defined(LIB_SHA256_C)
        case LIB_MD_SHA256:
            return lib_sha256_finish((lib_sha256_context_t*) ctx->md_ctx, odata);
#endif
#if defined(LIB_SHA384_C)
        case LIB_MD_SHA384:
            return lib_sha512_finish((lib_sha512_context_t*) ctx->md_ctx, odata);
#endif
#if defined(LIB_SHA512_C)
        case LIB_MD_SHA512:
            return lib_sha512_finish((lib_sha512_context_t*) ctx->md_ctx, odata);
#endif

        default:
            return LIB_ERR_MD_BAD_INPUT_DATA;
    }
}

int lib_md(const lib_md_info_t* md_info, const unsigned char* idata, size_t isize, unsigned char* odata) {
    if (!md_info) {
        return LIB_ERR_MD_BAD_INPUT_DATA;
    }

    switch (md_info->type) {

#if defined(LIB_MD5_C)
        case LIB_MD_MD5:
            return lib_md5(idata, isize, odata);
#endif
#if defined(LIB_SHA224_C)
        case LIB_MD_SHA224:
            return lib_sha256(idata, isize, odata, 1);
#endif
#if defined(LIB_SHA256_C)
        case LIB_MD_SHA256:
            return lib_sha256(idata, isize, odata, 0);
#endif
#if defined(LIB_SHA384_C)
        case LIB_MD_SHA384:
            return lib_sha512(idata, isize, odata, 1);
#endif
#if defined(LIB_SHA512_C)
        case LIB_MD_SHA512:
            return lib_sha512(idata, isize, odata, 0);
#endif

        default:
            return LIB_ERR_MD_BAD_INPUT_DATA;
    }
}

/////////

typedef struct {
    const char *md_name;
    lib_md_type_t md_type;
} md_name_entry;

static const md_name_entry md_names[] = {
#if defined(LIB_MD_CAN_MD5)
    { "MD5", LIB_MD_MD5 },
#endif
#if defined(LIB_MD_CAN_RIPEMD160)
    { "RIPEMD160", LIB_MD_RIPEMD160 },
#endif
#if defined(LIB_MD_CAN_SHA1)
    { "SHA1", LIB_MD_SHA1 },
    { "SHA", LIB_MD_SHA1 }, // compatibility fallback
#endif
#if defined(LIB_MD_CAN_SHA224)
    { "SHA224", LIB_MD_SHA224 },
#endif
#if defined(LIB_MD_CAN_SHA256)
    { "SHA256", LIB_MD_SHA256 },
#endif
#if defined(LIB_MD_CAN_SHA384)
    { "SHA384", LIB_MD_SHA384 },
#endif
#if defined(LIB_MD_CAN_SHA512)
    { "SHA512", LIB_MD_SHA512 },
#endif
#if defined(LIB_MD_CAN_SHA3_224)
    { "SHA3-224", LIB_MD_SHA3_224 },
#endif
#if defined(LIB_MD_CAN_SHA3_256)
    { "SHA3-256", LIB_MD_SHA3_256 },
#endif
#if defined(LIB_MD_CAN_SHA3_384)
    { "SHA3-384", LIB_MD_SHA3_384 },
#endif
#if defined(LIB_MD_CAN_SHA3_512)
    { "SHA3-512", LIB_MD_SHA3_512 },
#endif
    { NULL, LIB_MD_NONE },
};

const lib_md_info_t* lib_md_info_from_string(const char* md_name) {
    if (!md_name) {
        return NULL;
    }

    const md_name_entry* entry = md_names;
    while (entry->md_name != NULL &&
           strcmp(entry->md_name, md_name) != 0) {
        ++entry;
    }

    return lib_md_info_from_type(entry->md_type);
}

const char* lib_md_get_name(const lib_md_info_t* md_info) {
    if (!md_info) {
        return NULL;
    }

    const md_name_entry* entry = md_names;
    while (entry->md_type != LIB_MD_NONE &&
           entry->md_type != md_info->type) {
        ++entry;
    }

    return entry->md_name;
}

const lib_md_info_t* lib_md_info_from_ctx(const lib_md_context_t* ctx) {
    if (!ctx) {
        return NULL;
    }
    return ctx->md_info;
}

//////

int lib_md_hmac_start(lib_md_context_t* ctx, const unsigned char* kdata, size_t ksize) {

    if (ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL) {
        return LIB_ERR_MD_BAD_INPUT_DATA;
    }

    int ret = LIB_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char sum[LIB_MD_MAX_SIZE];
    unsigned char *ipad, *opad;

    if (ksize > (size_t) ctx->md_info->block_size) {
        if ((ret = lib_md_start(ctx)) != 0) {
            goto cleanup;
        }
        if ((ret = lib_md_update(ctx, kdata, ksize)) != 0) {
            goto cleanup;
        }
        if ((ret = lib_md_finish(ctx, sum)) != 0) {
            goto cleanup;
        }

        ksize = ctx->md_info->size;
        kdata = sum;
    }

    ipad = (unsigned char*) ctx->hmac_ctx;
    opad = (unsigned char*) ctx->hmac_ctx + ctx->md_info->block_size;

    memset(ipad, 0x36, ctx->md_info->block_size);
    memset(opad, 0x5C, ctx->md_info->block_size);

    lib_xor(ipad, ipad, kdata, ksize);
    lib_xor(opad, opad, kdata, ksize);

    if ((ret = lib_md_start(ctx)) != 0) {
        goto cleanup;
    }
    if ((ret = lib_md_update(ctx, ipad, ctx->md_info->block_size)) != 0) {
        goto cleanup;
    }

cleanup:
    lib_memsetz(sum, sizeof(sum));
    return ret;
}

int lib_md_hmac_update(lib_md_context_t* ctx, const unsigned char* idata, size_t isize) {
    if (ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL) {
        return LIB_ERR_MD_BAD_INPUT_DATA;
    }
    return lib_md_update(ctx, idata, isize);
}

int lib_md_hmac_finish(lib_md_context_t* ctx, unsigned char* odata) {

    if (ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL) {
        return LIB_ERR_MD_BAD_INPUT_DATA;
    }

    int ret = LIB_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char tmp[LIB_MD_MAX_SIZE];
    unsigned char* opad;

    opad = (unsigned char*) ctx->hmac_ctx + ctx->md_info->block_size;

    if ((ret = lib_md_finish(ctx, tmp)) != 0) {
        return ret;
    }
    if ((ret = lib_md_start(ctx)) != 0) {
        return ret;
    }
    if ((ret = lib_md_update(ctx, opad, ctx->md_info->block_size)) != 0) {
        return ret;
    }
    if ((ret = lib_md_update(ctx, tmp, ctx->md_info->size)) != 0) {
        return ret;
    }
    return lib_md_finish(ctx, odata);
}

int lib_md_hmac_reset(lib_md_context_t* ctx) {

    if (ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL) {
        return LIB_ERR_MD_BAD_INPUT_DATA;
    }

    int ret = LIB_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char *ipad;

    ipad = (unsigned char*) ctx->hmac_ctx;

    if ((ret = lib_md_start(ctx)) != 0) {
        return ret;
    }
    return lib_md_update(ctx, ipad, ctx->md_info->block_size);
}

int lib_md_hmac(const lib_md_info_t* md_info,
                    const unsigned char* kdata, size_t ksize,
                    const unsigned char* idata, size_t isize,
                    unsigned char* odata) {

    if (!md_info) {
        return LIB_ERR_MD_BAD_INPUT_DATA;
    }

    lib_md_context_t ctx;
    int ret = LIB_ERR_ERROR_CORRUPTION_DETECTED;

    lib_md_init(&ctx);

    if ((ret = lib_md_setup(&ctx, md_info, 1)) != 0) {
        goto cleanup;
    }

    if ((ret = lib_md_hmac_start(&ctx, kdata, ksize)) != 0) {
        goto cleanup;
    }

    if ((ret = lib_md_hmac_update(&ctx, idata, isize)) != 0) {
        goto cleanup;
    }

    if ((ret = lib_md_hmac_finish(&ctx, odata)) != 0) {
        goto cleanup;
    }

cleanup:
    lib_md_free(&ctx);

    return ret;
}
