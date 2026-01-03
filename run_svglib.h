#include "run_doclib.h"
#include "run_htmllib.h"
// #include "run_svglib.h"

#define LIB_SVG_CHARSET     LIB_DOC_CHARSET
#define LIB_SVG_TITLE       NULL
#define LIB_SVGL_UNIT        "px"
#define LIB_SVG_MARGIN      "5px"
#define LIB_SVG_FONT_NAME   LIB_DOC_FONT_NAME
#define LIB_SVG_FONT_STYLE  LIB_DOC_FONT_STYLE
#define LIB_SVG_FONT_WEIGHT LIB_DOC_FONT_WEIGHT
#define LIB_SVG_FONT_SIZE   "12px"

/**
 * HTML Context
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

static int lib_svg_init(lib_svg_context_t* ctx) {
    if (!ctx) {
        return 1;
    }

    ctx->charset     = NULL;
    ctx->title       = NULL;
    ctx->margin      = NULL;
    ctx->font_name   = NULL;
    ctx->font_style  = NULL;
    ctx->font_weight = NULL;
    ctx->font_size   = NULL;

    return 0;
}

static int lib_html_init2(lib_html_context_t* ctx2, lib_svg_context_t* ctx) {
    if (!ctx) {
        return 1;
    }

    ctx2->charset     = ctx->charset;
    ctx2->title       = ctx->title;
    ctx2->margin      = ctx->margin;
    ctx2->font_name   = ctx->font_name;
    ctx2->font_style  = ctx->font_style;
    ctx2->font_weight = ctx->font_weight;
    ctx2->font_size   = ctx->font_size;

    return 0;
}

static int run_svg(lib_svg_context_t* ctx, char* data, size_t size) {
    
    if (size == 0 || !data) {
        fprintf(stderr, "%s: No input data\n", prog_name);
        return 1;
    }

    lib_html_context_t ctx2;
    lib_html_init2(&ctx2, ctx);
    lib_html_prepare(&ctx2);

    ctx2.data = data;
    ctx2.size = size;

    return lib_html_document(&ctx2);
}
