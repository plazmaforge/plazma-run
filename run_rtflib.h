#include "iolib.h"
#include "run_doclib.h"

#define LIB_RTF_CHARSET     LIB_DOC_CHARSET
#define LIB_RTF_TITLE       NULL
#define LIB_RTF_UNIT        "px"
#define LIB_RTF_MARGIN      "5px"
#define LIB_RTF_FONT_NAME   "Arial"
#define LIB_RTF_FONT_STYLE  LIB_DOC_FONT_STYLE
#define LIB_RTF_FONT_WEIGHT LIB_DOC_FONT_WEIGHT
#define LIB_RTF_FONT_SIZE   "12px"

/**
 * RTF Config
 */
typedef struct lib_rtf_config_t {
    LIB_DOC_CONFIG
} lib_rtf_config_t;

/**
 * RTF Context
 */
typedef struct lib_rtf_context_t {
    LIB_DOC_CONTEXT
} lib_rtf_context_t;

static int lib_rtf_document(lib_rtf_context_t* ctx);

static int lib_rtf_head(lib_rtf_context_t* ctx);

static int lib_rtf_charset(lib_rtf_context_t* ctx);

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

    return lib_doc_config_init((lib_doc_config_t*) cnf);    
}

static int lib_rtf_prepare(lib_rtf_context_t* ctx) {
    if (!ctx) {
        return 1;
    }

    int error = lib_doc_context_prepare((lib_doc_context_t*) ctx);
    if (error != 0) {
        return error;
    }

    ctx->margin_unit = lib_rtf_unitdef(ctx->margin);
    if (ctx->font) {
        ctx->font->unit   = lib_rtf_unitdef(ctx->font->size);
    }
    
    return 0;
}

static int lib_rtf_ctx_init(lib_rtf_config_t* cnf, lib_rtf_context_t* ctx) {
    if (!cnf || !ctx) {
        return 1;
    }

    // config -> context
    ctx->charset = cnf->charset;
    ctx->title   = cnf->title;
    ctx->margin  = cnf->margin;
    ctx->font    = cnf->font;
    ctx->data    = NULL;
    ctx->size    = 0;

    return lib_rtf_prepare(ctx);
}

static int lib_rtf_document(lib_rtf_context_t* ctx) {
    
    // DOCUMENT
    fprintf(stdout, "{\\rtf1\\ansi\\deff0\n");

    // HEAD
    if (ctx->use_head) {
       lib_rtf_head(ctx);
    }

    // BODY
    lib_rtf_body(ctx);
    
    fprintf(stdout, "}\n");

    return 0;
}

static int lib_rtf_head(lib_rtf_context_t* ctx) {

    // CHARSET    
    if (ctx->use_charset) {
        lib_rtf_charset(ctx);
    }

    //if (ctx->use_title) {
    //}

    // FONT
    if (ctx->use_font) {
        lib_rtf_font(ctx);
    }

    return 0;
}

static int lib_rtf_charset(lib_rtf_context_t* ctx) {
    // TODO
    //fprintf(stdout, "\\fni1\\fcharset204\n");
    return 0;
}

static int lib_rtf_margin(lib_rtf_context_t* ctx) {
    // TODO
    return 0;
}

static int lib_rtf_font(lib_rtf_context_t* ctx) {

    // FONT TABLE
    if (ctx->use_font_name) {
        fprintf(stdout, "{\\fonttbl {\\f0 %s;}}\n", ctx->font->name);
        fprintf(stdout, "\\f0\n");
    }

    // STYLE
    if (ctx->use_font_style) {

        // ITALIC
        if (lib_has_value(ctx->font->style, "italic")) {    
            fprintf(stdout, "\\i\n");
        }

        // UNDERLINE
        if (lib_has_value(ctx->font->style, "underline")) {
            if (lib_has_value(ctx->font->style, "solid")) {
                fprintf(stdout, "\\ulth\n");
            } else if (lib_has_value(ctx->font->style, "double")) {
                fprintf(stdout, "\\uldb\n");
            } else if (lib_has_value(ctx->font->style, "dash")) {
                fprintf(stdout, "\\ulthdash\n");
            } else if (lib_has_value(ctx->font->style, "dott")) {
                fprintf(stdout, "\\uld\n");
            } else {
                fprintf(stdout, "\\ul\n");
            }
        }

        // STRIKE
        if (lib_has_value(ctx->font->style, "strike")) {
            fprintf(stdout, "\\strike\n");
        }
    }

    // WEIGHT
    if (ctx->use_font_weight) {
        // TODO
        if (lib_stricmp(ctx->font->weight, "bold") == 0) {
            fprintf(stdout, "\\b\n");
        }
    }

    // SIZE
    if (ctx->use_font_size) {
        // TODO
        if (lib_is_digit(ctx->font->size)) {
            int size = atoi(ctx->font->size);
            int size_pt = size;
            int fs = size_pt * 2;
            fprintf(stdout, "\\fs%d\n", fs);
        }
    }

    /*

    - Font Underline:
    \ul       - solid
    \ulth     - solid?
    \uldb     - double
    \ulthdash - dash
    \uld      - dott

    - Font line-throught
    \strike

    - Font Color
    {\colortbl;red0\gren0\blue0;red0\green0\blue255;}
    \cf1

    - Font Unicode
    \u<dec><c>
    \198?

    */
    
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
    int error = lib_rtf_ctx_init(config, &ctx);
    if (error != 0) {
        fprintf(stderr, "%s: Initialization error\n", prog_name);
        return error;
    }

    ctx.data = data;
    ctx.size = size;

    return lib_rtf_document(&ctx);
}

