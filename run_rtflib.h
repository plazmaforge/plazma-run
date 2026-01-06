#include "iolib.h"
#include "run_doclib.h"

#define LIB_RTF_CHARSET     LIB_DOC_CHARSET
#define LIB_RTF_TITLE       NULL
#define LIB_RTF_UNIT        "px"
#define LIB_RTF_MARGIN      "5px"
#define LIB_RTF_FONT_NAME   LIB_DOC_FONT_NAME
#define LIB_RTF_FONT_STYLE  LIB_DOC_FONT_STYLE
#define LIB_RTF_FONT_WEIGHT LIB_DOC_FONT_WEIGHT
#define LIB_RTF_FONT_SIZE   "12px"

/**
 * RTF Config
 */
typedef struct lib_rtf_config_t {
    const char* charset;
    const char* title;
    const char* margin;
    const char* font_name;
    const char* font_style;
    const char* font_weight;
    const char* font_size;
} lib_rtf_config_t;

/**
 * RTF Context
 */
typedef struct lib_rtf_context_t {
    const char* charset;
    const char* title;
    const char* margin;
    const char* margin_unit; 
    const char* font_name;
    const char* font_style;
    const char* font_weight;
    const char* font_size;
    const char* font_unit;    

    bool use_charset;
    bool use_title;
    bool use_head;

    bool use_margin;
    bool use_font_name;
    bool use_font_style;
    bool use_font_weight;
    bool use_font_size;
    bool use_font;
    bool use_style;

    char* data;
    size_t size;

} lib_rtf_context_t;

static int lib_rtf_document(lib_rtf_context_t* ctx);

static int lib_rtf_head(lib_rtf_context_t* ctx);

static int lib_rtf_margin(lib_rtf_context_t* ctx);

static int lib_rtf_font(lib_rtf_context_t* ctx);

static int lib_rtf_body(lib_rtf_context_t* ctx);


static const char* lib_rtf_unitdef(const char* value) {
    return lib_is_digit(value) ? LIB_RTF_UNIT : "";
}

static int lib_rtf_init(lib_rtf_config_t* cnf) {
    if (!cnf) {
        return 1;
    }

    cnf->charset     = NULL;
    cnf->title       = NULL;
    cnf->margin      = NULL;
    cnf->font_name   = NULL;
    cnf->font_style  = NULL;
    cnf->font_weight = NULL;
    cnf->font_size   = NULL;
    return 0;
}

static int lib_rtf_prepare(lib_rtf_context_t* ctx) {
    if (!ctx) {
        return 1;
    }

    ctx->margin_unit = lib_rtf_unitdef(ctx->margin);
    ctx->font_unit   = lib_rtf_unitdef(ctx->font_size);

    ctx->use_charset        = ctx->charset;
    ctx->use_title          = ctx->title;
    ctx->use_head           = ctx->use_charset || ctx->use_title;

    ctx->use_margin         = ctx->margin;
    ctx->use_font_name      = ctx->font_name;
    ctx->use_font_style     = ctx->font_style;
    ctx->use_font_weight    = ctx->font_weight;
    ctx->use_font_size      = ctx->font_size;
    ctx->use_font           = ctx->use_font_name || ctx->use_font_style || ctx->use_font_weight || ctx->use_font_size;
    ctx->use_style          = ctx->use_margin || ctx->use_font;

    return 0;

}

static int lib_rtf_ctx_init(lib_rtf_config_t* cnf, lib_rtf_context_t* ctx) {
    if (!cnf || !ctx) {
        return 1;
    }

    // config -> context
    ctx->charset     = cnf->charset;
    ctx->title       = cnf->title;
    ctx->margin      = cnf->margin;
    ctx->font_name   = cnf->font_name;
    ctx->font_style  = cnf->font_style;
    ctx->font_weight = cnf->font_weight;
    ctx->font_size   = cnf->font_size;

    lib_rtf_prepare(ctx);
    
    ctx->data               = NULL;
    ctx->size               = 0;

    return 0;
}

static int lib_rtf_document(lib_rtf_context_t* ctx) {
    
    // DOCUMENT
    fprintf(stdout, "{\\rtf1\\ansi\\deff0\n");
    //fprintf(stdout, "\\fni1\\fcharset204\n");

    // HEAD
    //if (ctx->use_charset) {
    //    lib_rtf_charset(ctx);
    //}

    //if (ctx->use_font) {
    //    lib_rtf_font(ctx);
    //}

    // BODY
    lib_rtf_body(ctx);
    
    fprintf(stdout, "}\n");

    return 0;
}

static int lib_rtf_head(lib_rtf_context_t* ctx) {

    if (ctx->use_charset) {
        // TODO
    }
    if (ctx->use_title) {
        // TODO
    }

    return 0;
}

static int lib_rtf_margin(lib_rtf_context_t* ctx) {
    // TODO
    return 0;
}

static int lib_rtf_font(lib_rtf_context_t* ctx) {

    if (ctx->use_font_name) {
        // TODO
    }
    if (ctx->use_font_style) {
        // TODO
    }
    if (ctx->use_font_weight) {
        // TODO
    }
    if (ctx->use_font_size) {
        // TODO
    }
    
    return 0;
}

static int lib_rtf_body(lib_rtf_context_t* ctx) {
    fprintf(stdout, "%s", ctx->data);
    fprintf(stdout, "\\par\n");
    return 0;
}

static int run_rtf(lib_rtf_config_t* config, char* data, size_t size) {
    
    if (size == 0 || !data) {
        fprintf(stderr, "%s: No input data\n", prog_name);
        return 1;
    }

    lib_rtf_context_t ctx;
    lib_rtf_ctx_init(config, &ctx);
    ctx.data = data;
    ctx.size = size;

    return lib_rtf_document(&ctx);
}

