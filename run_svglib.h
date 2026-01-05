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
    const char* charset;
    const char* title;
    const char* margin;
    const char* font_name;
    const char* font_style;
    const char* font_weight;
    const char* font_size;
} lib_svg_config_t;

/**
 * SVG Context
 */
typedef struct lib_svg_context_t {
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

} lib_svg_context_t;

static const char* lib_svg_unitdef(const char* value) {
    return lib_is_digit(value) ? LIB_SVG_UNIT : "";
}

static int lib_svg_init(lib_svg_config_t* config) {
    if (!config) {
        return 1;
    }

    config->charset     = NULL;
    config->title       = NULL;
    config->margin      = NULL;
    config->font_name   = NULL;
    config->font_style  = NULL;
    config->font_weight = NULL;
    config->font_size   = NULL;

    return 0;
}

static int lib_svg_prepare(lib_svg_context_t* ctx) {
    if (!ctx) {
        return 1;
    }

    ctx->margin_unit = lib_svg_unitdef(ctx->margin);
    ctx->font_unit   = lib_svg_unitdef(ctx->font_size);

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

static int lib_svg_ctx_init(lib_svg_config_t* cnf, lib_svg_context_t* ctx) {
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

    lib_svg_prepare(ctx);
    
    ctx->data               = NULL;
    ctx->size               = 0;

    return 0;
}

// static int lib_html_init2(lib_html_context_t* ctx2, lib_svg_context_t* ctx) {
//     if (!ctx) {
//         return 1;
//     }

//     ctx2->charset     = ctx->charset;
//     ctx2->title       = ctx->title;
//     ctx2->margin      = ctx->margin;
//     ctx2->font_name   = ctx->font_name;
//     ctx2->font_style  = ctx->font_style;
//     ctx2->font_weight = ctx->font_weight;
//     ctx2->font_size   = ctx->font_size;

//     return 0;
// }

static int lib_svg_margin(lib_html_context_t* ctx) {
    fprintf(stdout, "margin: %s%s;", ctx->margin, ctx->margin_unit);
    return 0;
}

static int lib_svg_font(lib_svg_context_t* ctx) {

    if (ctx->use_font_name) {
        fprintf(stdout, " font-family=\"%s\"", ctx->font_name);
    }
    if (ctx->use_font_style) {
        fprintf(stdout, " font-style=\"%s\"", ctx->font_style);
    }
    if (ctx->use_font_weight) {
        fprintf(stdout, " font-weight=\"%s\"", ctx->font_weight);
    }
    if (ctx->use_font_size) {
        fprintf(stdout, " font-size=\"%s%s", ctx->font_size, ctx->font_unit);
    }
    
    return 0;
}

static int lib_svg_text(lib_svg_context_t* ctx) {
    fprintf(stdout, "  <text x=\"0\" y=\"20\" ");

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
    //fprintf(stdout, " width=\"600\" height=\"300\"");
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
    lib_svg_ctx_init(config, &ctx);
    ctx.data = data;
    ctx.size = size;

    return lib_svg_document(&ctx);
}
