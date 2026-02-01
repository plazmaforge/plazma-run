#include "run_doclib.h"
#include "run_htmllib.h"

#define LIB_SVG_CHARSET     LIB_DOC_CHARSET
#define LIB_SVG_TITLE       NULL
#define LIB_SVG_UNIT        "px"
#define LIB_SVG_MARGIN      "5px"
#define LIB_SVG_FONT_NAME   LIB_DOC_FONT_NAME
#define LIB_SVG_FONT_STYLE  LIB_DOC_FONT_STYLE
#define LIB_SVG_FONT_WEIGHT LIB_DOC_FONT_WEIGHT
#define LIB_SVG_FONT_SIZE   "12px"

/**
 * SVG Config
 */
typedef struct lib_svg_config_t {
    RUN_DOC_CONFIG

    const char* width;
    const char* height;
} lib_svg_config_t;

/**
 * SVG Context
 */
typedef struct lib_svg_context_t {
    RUN_DOC_CONTEXT

    const char* width;
    const char* width_unit;
    const char* height;
    const char* height_unit;

    bool use_width;
    bool use_height;
    bool use_size;

} lib_svg_context_t;

static const char* lib_svg_unitdef(const char* value) {
    return lib_is_digit(value) ? LIB_SVG_UNIT : "";
}

static int lib_svg_init(lib_svg_config_t* cnf) {
    if (!cnf) {
        return 1;
    }

    cnf->width       = NULL;
    cnf->height      = NULL;

    return lib_doc_config_init((lib_doc_config_t*) cnf);
}

static int lib_svg_prepare(lib_svg_context_t* ctx) {
    if (!ctx) {
        return 1;
    }

    ctx->margin_unit    = lib_svg_unitdef(ctx->margin);
    if (ctx->font) {
        ctx->font->unit = lib_svg_unitdef(ctx->font->size);
    }
    
    ctx->width_unit     = lib_svg_unitdef(ctx->width);
    ctx->height_unit    = lib_svg_unitdef(ctx->height);

    ctx->use_width          = ctx->width;
    ctx->use_height         = ctx->height;
    ctx->use_size           = ctx->use_width | ctx->use_height;

    return lib_doc_context_prepare((lib_doc_context_t*) ctx);
}

static int lib_svg_ctx_init(lib_svg_config_t* cnf, lib_svg_context_t* ctx) {
    if (!cnf || !ctx) {
        return 1;
    }

    // config -> context
    ctx->charset     = cnf->charset;
    ctx->title       = cnf->title;
    ctx->margin      = cnf->margin;
    ctx->font        = cnf->font;
    ctx->data        = NULL;
    ctx->size        = 0;

    ctx->width       = cnf->width;
    ctx->height      = cnf->height;

    return lib_svg_prepare(ctx);
}

////

static int lib_svg_margin(lib_html_context_t* ctx) {
    fprintf(stdout, "margin: %s%s;", ctx->margin, ctx->margin_unit);
    return 0;
}

static int lib_svg_size(lib_svg_context_t* ctx) {
    
    // SIZE
    if (ctx->use_width) {
        fprintf(stdout, " width=\"%s%s\"", ctx->width, ctx->width_unit);
    }
    if (ctx->use_height) {
        fprintf(stdout, " height=\"%s%s\"", ctx->height, ctx->height_unit);
    }

    return 0;
}

static int lib_svg_font(lib_svg_context_t* ctx) {

    if (ctx->use_font_name) {
        fprintf(stdout, " font-family=\"%s\"", ctx->font->name);
    }
    if (ctx->use_font_style) {
        fprintf(stdout, " font-style=\"%s\"", ctx->font->style);
    }
    if (ctx->use_font_weight) {
        fprintf(stdout, " font-weight=\"%s\"", ctx->font->weight);
    }
    if (ctx->use_font_size) {
        fprintf(stdout, " font-size=\"%s%s", ctx->font->size, ctx->font->unit);
    }
    
    return 0;
}

static int lib_svg_text(lib_svg_context_t* ctx) {
    fprintf(stdout, "  <text x=\"0\" y=\"20\"");

    if (ctx->use_style) {
        if (ctx->use_font) {
           lib_svg_font(ctx);
        }
    }
    fprintf(stdout, "\">\n");

    fprintf(stdout, "%s\n", ctx->data);
    fprintf(stdout, "</text>\n");
    return 0;
}

static int lib_svg_document(lib_svg_context_t* ctx) {
    
    // DOCUMENT
    fprintf(stdout, "<svg");

    // SIZE
    if (ctx->use_size) {
        lib_svg_size(ctx);

    }
    fprintf(stdout, " xmlns=\"http://www.w3.org/2000/svg\">\n");

    // TEXT
    lib_svg_text(ctx);
    
    fprintf(stdout, "</svg>\n");

    return 0;
}

static int run_svg(lib_svg_config_t* config, char* data, size_t size) {
    
    if (size == 0 || !data) {
        fprintf(stderr, "%s: No input data\n", prog_name);
        return 1;
    }

    lib_svg_context_t ctx;
    int error = lib_svg_ctx_init(config, &ctx);
    if (error != 0) {
        fprintf(stderr, "%s: Initialization error\n", prog_name);
        return error;
    }

    ctx.data = data;
    ctx.size = size;

    return lib_svg_document(&ctx);
}
