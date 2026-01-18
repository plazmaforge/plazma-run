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
    fprintf(stdout, " margin: %s%s;", ctx->margin, ctx->margin_unit);
    return 0;
}

static int lib_html_font(lib_html_context_t* ctx) {

    bool use_css       = false;
    bool use_bold      = false;
    bool use_underline = false;
    bool use_overline  = false;
    bool use_strike    = false;

    if (ctx->use_font_name) {
        fprintf(stdout, " font-family: %s;", ctx->font->name);
    }

    if (ctx->use_font_style) {
        if (use_css) {
            fprintf(stdout, " font-style: %s;", ctx->font->style);
        } else {

            // ITALIC
            if (lib_doc_has_italic(ctx->font->style)) {
                fprintf(stdout, " font-style: %s;", "italic");
            }

            // BOLD: has bold (!)
            if (lib_doc_has_bold(ctx->font->style)) {
                use_bold = true;
                fprintf(stdout, " font-weight: %s;", "bold");
            }

            // UNDERLINE
            if (lib_doc_has_underline(ctx->font->style)) {
                use_underline = true;
            }

            // OVERLINE
            if (lib_doc_has_overline(ctx->font->style)) {
                use_overline = true;
            }

            // STRIKE
            if (lib_doc_has_strike(ctx->font->style)) {
                use_strike = true;
            }

            if (use_underline || use_overline || use_strike) {

                // DECORATION-LINE
                fprintf(stdout, " text-decoration-line:");
                if (use_underline) {
                    fprintf(stdout, " underline");
                }
                if (use_overline) {
                    fprintf(stdout, " overline");
                }
                if (use_strike) {
                    fprintf(stdout, " line-through");
                }
                fprintf(stdout, ";");

                // DECORATION-STYLE
                // double | dotted | dashed | wavy: Only one style (?)
                // word (?)
                if (lib_doc_has_double(ctx->font->style)) {
                    fprintf(stdout, " text-decoration-style: %s;", "double");
                } else if (lib_doc_has_dotted(ctx->font->style)) {
                    fprintf(stdout, " text-decoration-style: %s;", "dotted");
                } else if (lib_doc_has_dashed(ctx->font->style)) {
                    fprintf(stdout, " text-decoration-style: %s;", "dashed");
                } else if (lib_doc_has_wave(ctx->font->style)) {
                    fprintf(stdout, " text-decoration-style: %s;", "wavy");
                }

            }

        }
    }

    if (ctx->use_font_weight) {
        if (use_css) {
            fprintf(stdout, " font-weight: %s;", ctx->font->weight);
        } else {
            // BOLD: is bold (!)            
            if (lib_doc_is_bold(ctx->font->weight)) {
                if (!use_bold) {
                    fprintf(stdout, " font-weight: %s;", "bold");
                }
            } else {
                // 100..900
                fprintf(stdout, " font-weight: %s;", ctx->font->weight);
            }
        }
    }

    // FONT SIZE
    if (ctx->use_font_size) {
        fprintf(stdout, " font-size: %s%s;", ctx->font->size, ctx->font->unit);
    }

    // FONT COLOR
    if (ctx->font->color) {
        fprintf(stdout, " color: %s;", ctx->font->color);
    }

    // FONT BACKGROUND
    if (ctx->font->background) {
        fprintf(stdout, " background: %s;", ctx->font->background);
    }
    
    return 0;
}

static int lib_html_body(lib_html_context_t* ctx) {

    bool use_div = true;

    fprintf(stdout, "  <body");

    // MARGIN
    if (ctx->use_style && ctx->use_margin) {
        lib_html_margin(ctx);
    }

    if (use_div) {
        fprintf(stdout, ">\n");
        fprintf(stdout, "    <div");
    }

    // STYLE
    if (ctx->use_style) {
        fprintf(stdout, " style=\"");
        if (ctx->use_font) {
           lib_html_font(ctx);
        }
        fprintf(stdout, "\">\n");
    }

    // CONTENT
    fprintf(stdout, "%s\n", ctx->data);

    if (use_div) {        
        fprintf(stdout, "  </div>\n");
    }
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

