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
    RUN_DOC_CONFIG
} lib_rtf_config_t;

/**
 * RTF Context
 */
typedef struct lib_rtf_context_t {
    RUN_DOC_CONTEXT
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

    return lib_doc_config_init((run_doc_config_t*) cnf);    
}

static int lib_rtf_prepare(lib_rtf_context_t* ctx) {
    if (!ctx) {
        return 1;
    }

    int error = lib_doc_context_prepare((run_doc_context_t*) ctx);
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

    ctx->out_file_name = cnf->out_file_name;
    ctx->out = cnf->out;

    return lib_rtf_prepare(ctx);
}

static int lib_rtf_document(lib_rtf_context_t* ctx) {
    
    // DOCUMENT
    fprintf(ctx->out, "{\\rtf1\\ansi\\deff0\n");

    // HEAD
    if (ctx->use_head) {
       lib_rtf_head(ctx);
    }

    // BODY
    lib_rtf_body(ctx);
    
    fprintf(ctx->out, "}\n");

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
    //fprintf(ctx->out, "\\fni1\\fcharset204\n");
    return 0;
}

static int lib_rtf_margin(lib_rtf_context_t* ctx) {
    // TODO
    return 0;
}

static int lib_rtf_font(lib_rtf_context_t* ctx) {
    
    if (!ctx->font) {
        return 0;
    }

    bool use_font_name      = false;
    bool use_fix_background = true; /* Special fix for Windows: add 'highlight' */

    int cf = 0;
    int cb = 0;

    // FONT TABLE
    if (ctx->use_font_name) {
        use_font_name = true;
        fprintf(ctx->out, "{\\fonttbl {\\f0 %s;}}\n", ctx->font->name);
    }

    // COLOR TABLE
    if (ctx->font->color || ctx->font->background) {
        fprintf(ctx->out, "{\\colortbl;");

        int rgb[3];

        // FONT COLOR DEFINE
        if (ctx->font->color) {
            lib_doc_rgb_parse(ctx->font->color, rgb);
            fprintf(ctx->out, "\\red%d\\green%d\\blue%d;", rgb[0], rgb[1], rgb[2]);
            cf = 1;
        }

        // FONT BACKGROUND DEFINE
        if (ctx->font->background) {
            lib_doc_rgb_parse(ctx->font->background, rgb);
            fprintf(ctx->out, "\\red%d\\green%d\\blue%d;", rgb[0], rgb[1], rgb[2]);
            cb = cf + 1;
        }

        fprintf(ctx->out, "}\n");

    }

    // FONT NAME
    if (use_font_name) {
        fprintf(ctx->out, "\\f0\n");
    }

    // FONT COLOR
    if (cf > 0) {
        fprintf(ctx->out, "\\cf%d\n", cf);
    }

    // FONT BACKGROUND
    if (cb > 0) {        
        if (use_fix_background) {
            fprintf(ctx->out, "\\highlight%d\n", cb);
        }
        fprintf(ctx->out, "\\cb%d\n", cb);
    }

    bool use_bold      = false;

    // STYLE
    if (ctx->use_font_style) {

        // ITALIC
        if (lib_doc_has_italic(ctx->font->style)) {
            fprintf(ctx->out, "\\i\n");
        }

        // BOLD: has bold (!)
        if (lib_doc_has_bold(ctx->font->style)) {
            use_bold = true;
            fprintf(ctx->out, "\\b\n");
        }

        // UNDERLINE
        if (lib_doc_has_underline(ctx->font->style)) {

            // double | word | wave: Only one style (?)
            if (lib_doc_has_double(ctx->font->style)) {
                fprintf(ctx->out, "\\uldb\n");
            } else if (lib_doc_has_word(ctx->font->style)) {
                fprintf(ctx->out, "\\ulw\n");
            } else if (lib_doc_has_wave(ctx->font->style)) {
                fprintf(ctx->out, "\\ulwave\n");

            } else if (lib_doc_has_thdashdd(ctx->font->style)) {
                fprintf(ctx->out, "\\ulthdashdd\n");
            } else if (lib_doc_has_thdashd(ctx->font->style)) {
                fprintf(ctx->out, "\\ulthdashd\n");
            } else if (lib_doc_has_thdash(ctx->font->style)) {
                fprintf(ctx->out, "\\ulthdash\n");
            } else if (lib_doc_has_thdot(ctx->font->style)) {    
                fprintf(ctx->out, "\\ulthd\n");
            } else if (lib_doc_has_thick(ctx->font->style)) {
                fprintf(ctx->out, "\\ulth\n");

            } else if (lib_doc_has_dashdd(ctx->font->style)) {
                fprintf(ctx->out, "\\uldashdd\n");
            } else if (lib_doc_has_dashd(ctx->font->style)) {
                fprintf(ctx->out, "\\uldashd\n");
            } else if (lib_doc_has_dash(ctx->font->style)) {
                fprintf(ctx->out, "\\uldash\n");
            } else if (lib_doc_has_dot(ctx->font->style)) {    
                fprintf(ctx->out, "\\uld\n");
            } else {
                fprintf(ctx->out, "\\ul\n");
            }

        }

        // STRIKE
        if (lib_doc_has_strike(ctx->font->style)) {
            fprintf(ctx->out, "\\strike\n");
        }
    }

    // WEIGHT
    if (ctx->use_font_weight) {
        // TODO: bold = nnn
        if (lib_doc_has_bold(ctx->font->style)) {
            if (!use_bold) {
                fprintf(ctx->out, "\\b\n");
            }
        }
    }

    // SIZE
    if (ctx->use_font_size) {
        // TODO
        if (lib_is_digit(ctx->font->size)) {
            int size = atoi(ctx->font->size);
            int size_pt = size;
            int fs = size_pt * 2;
            fprintf(ctx->out, "\\fs%d\n", fs);
        }
    }

    /*

    - Font Underline:

    \ul         - Underline
    \uld        - Dot underline
    \uldash     - Dash underline
    \uldashd    - Dot dash underline
    \uldashdd   - Dot dot dash underline

    \ulth       - Thick underline
    \ulthd      - Thick dot underline
    \ulthdash   - Thick dash underline
    \ulthdashd  - Thick dot dash underline
    \ulthdashdd - Thick dot dot dash underline

    \uldb       - Double underline
    \ulw        - Word underline
    \ulword     - Wave underline

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

static int lib_rtf_content(lib_rtf_context_t* ctx) {
    if (!(ctx->data)) {
        return 0;
    }

    bool use_raw = false;

    if (use_raw) {
        fprintf(ctx->out, "%s", ctx->data);
        return 0;
    }

    char c;
    bool break_line;
    bool new_line = true;
    for (size_t i = 0; i < ctx->size; i++) {

        break_line = false;
        c = ctx->data[i];
        
        switch (c) {
        case '\\':
            fprintf(ctx->out, "\\\\");
            break;
        case '{':
            fprintf(ctx->out, "\\{");
            break;
        case '}':
            fprintf(ctx->out, "\\}");
            break;
        case '\n':
            break_line = true;
            break;
        case '\r':
            if (i + 1 < ctx->size && ctx->data[i + 1] == '\n') {
                i++;
            }
            break_line = true;
        default:
            fprintf(ctx->out, "%c", c);
            break;
        }

        new_line = break_line;
        if (break_line) {
            fprintf(ctx->out, "\n\\line\n");
        }

    }

    return 0;
}

static int lib_rtf_body(lib_rtf_context_t* ctx) {
    lib_rtf_content(ctx);
    fprintf(ctx->out, "\\par\n");
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

