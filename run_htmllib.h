#include "iolib.h"
#include "run_doclib.h"

#define LIB_HTML_CHARSET     LIB_DOC_CHARSET
#define LIB_HTML_TITLE       NULL
#define LIB_HTML_UNIT        "px"
#define LIB_HTML_MARGIN      "5px"
#define LIB_HTML_FONT_NAME   LIB_DOC_FONT_NAME
#define LIB_HTML_FONT_STYLE  LIB_DOC_FONT_STYLE
#define LIB_HTML_FONT_WEIGHT LIB_DOC_FONT_WEIGHT
#define LIB_HTML_FONT_SIZE   "12px"

/**
 * HTML Config (temp solution: config -> context)
 */
typedef struct lib_html_config_t {
    LIB_DOC_CONFIG
} lib_html_config_t;

/**
 * HTML Context
 */
typedef struct lib_html_context_t {
    LIB_DOC_CONTEXT
} lib_html_context_t;

static int lib_html_document(lib_html_context_t* ctx);

static int lib_html_head(lib_html_context_t* ctx);

static int lib_html_margin(lib_html_context_t* ctx);

static int lib_html_font(lib_html_context_t* ctx);

static int lib_html_body(lib_html_context_t* ctx);


static const char* lib_html_unitdef(const char* value) {
    return lib_is_digit(value) ? LIB_HTML_UNIT : "";
}

static int lib_html_init(lib_html_config_t* cnf) {
    if (!cnf) {
        return 1;
    }

    return lib_doc_config_init((lib_doc_config_t*) cnf);
}

static int lib_html_prepare(lib_html_context_t* ctx) {
    if (!ctx) {
        return 1;
    }
    
    int error = lib_doc_context_prepare((lib_doc_context_t*) ctx);
    if (error != 0) {
        return error;
    }

    ctx->margin_unit = lib_html_unitdef(ctx->margin);
    if (ctx->font) {
        ctx->font->unit   = lib_html_unitdef(ctx->font->size);
    }

    return 0;
}

static int lib_html_ctx_init(lib_html_config_t* cnf, lib_html_context_t* ctx) {
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

    return lib_html_prepare(ctx);
}

static int lib_html_document(lib_html_context_t* ctx) {
    
    // DOCUMENT
    fprintf(stdout, "<!DOCTYPE html>\n");
    fprintf(stdout, "<html>\n");

    // HEAD
    if (ctx->use_head) {
        lib_html_head(ctx);
    }

    // BODY
    lib_html_body(ctx);
    
    fprintf(stdout, "</html>\n");

    return 0;
}

static int lib_html_head(lib_html_context_t* ctx) {

    fprintf(stdout, "  <head>\n");
    if (ctx->use_charset) {
        fprintf(stdout, "    <meta charset=\"%s\">\n", ctx->charset);
    }
    if (ctx->use_title) {
        fprintf(stdout, "    <title>%s</title>\n", ctx->title);
    }
    fprintf(stdout, "  </head>\n");

    return 0;
}

static int lib_html_margin(lib_html_context_t* ctx) {
    fprintf(stdout, "margin: %s%s;", ctx->margin, ctx->margin_unit);
    return 0;
}

static int lib_html_font(lib_html_context_t* ctx) {

    if (ctx->use_font_name) {
        fprintf(stdout, "font-family: %s;", ctx->font->name);
    }
    if (ctx->use_font_style) {
        fprintf(stdout, "font-style: %s;", ctx->font->style);
    }
    if (ctx->use_font_weight) {
        fprintf(stdout, "font-weight: %s;", ctx->font->weight);
    }
    if (ctx->use_font_size) {
        fprintf(stdout, "font-size: %s%s;", ctx->font->size, ctx->font->unit);
    }
    
    return 0;
}

static int lib_html_body(lib_html_context_t* ctx) {

    if (ctx->use_style) {
        fprintf(stdout, "  <body style=\"");
        if (ctx->use_margin) {
           lib_html_margin(ctx);
        }
        if (ctx->use_font) {
           lib_html_font(ctx);
        }
        fprintf(stdout, "\">\n");
    } else {
        fprintf(stdout, "  <body>\n");
    }    

    fprintf(stdout, "%s\n", ctx->data);
    fprintf(stdout, "  </body>\n");

    return 0;
}

static int run_html(lib_html_config_t* config, char* data, size_t size) {
    
    if (size == 0 || !data) {
        fprintf(stderr, "%s: No input data\n", prog_name);
        return 1;
    }

    lib_html_context_t ctx;
    int error = lib_html_ctx_init(config, &ctx);
    if (error != 0) {
        fprintf(stderr, "%s: Initialization error\n", prog_name);
        return error;
    }

    ctx.data = data;
    ctx.size = size;

    return lib_html_document(&ctx);
}

