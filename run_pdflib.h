#include "iolib.h"
#include "run_doclib.h"

#define LIB_PDF_CHARSET     LIB_DOC_CHARSET
#define LIB_PDF_TITLE       NULL
#define LIB_PDF_UNIT        "px"
#define LIB_PDF_MARGIN      "5px"
#define LIB_PDF_FONT_NAME   LIB_DOC_FONT_NAME
#define LIB_PDF_FONT_STYLE  LIB_DOC_FONT_STYLE
#define LIB_PDF_FONT_WEIGHT LIB_DOC_FONT_WEIGHT
#define LIB_PDF_FONT_SIZE   "12px"

/**
 * PDF Config
 */
typedef struct lib_pdf_config_t {
    LIB_DOC_CONFIG_0
} lib_pdf_config_t;

/**
 * PDF Context
 */
typedef struct lib_pdf_context_t {
    LIB_DOC_CONTEXT_0

} lib_pdf_context_t;

static int lib_pdf_document(lib_pdf_context_t* ctx);

static int lib_pdf_head(lib_pdf_context_t* ctx);

static int lib_pdf_margin(lib_pdf_context_t* ctx);

static int lib_pdf_font(lib_pdf_context_t* ctx);

static int lib_pdf_body(lib_pdf_context_t* ctx);


static const char* lib_pdf_unitdef(const char* value) {
    return lib_is_digit(value) ? LIB_PDF_UNIT : "";
}

static int lib_pdf_init(lib_pdf_config_t* cnf) {
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

static int lib_pdf_prepare(lib_pdf_context_t* ctx) {
    if (!ctx) {
        return 1;
    }

    ctx->margin_unit = lib_pdf_unitdef(ctx->margin);
    ctx->font_unit   = lib_pdf_unitdef(ctx->font_size);

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

static int lib_pdf_ctx_init(lib_pdf_config_t* cnf, lib_pdf_context_t* ctx) {
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

    lib_pdf_prepare(ctx);
    
    ctx->data               = NULL;
    ctx->size               = 0;

    return 0;
}

static int lib_pdf_document(lib_pdf_context_t* ctx) {
    
    // DOCUMENT
    fprintf(stdout, "<!DOCTYPE html>\n");
    fprintf(stdout, "<html>\n");

    // HEAD
    if (ctx->use_head) {
        lib_pdf_head(ctx);
    }

    // BODY
    lib_pdf_body(ctx);
    
    fprintf(stdout, "</html>\n");

    return 0;
}

static int lib_pdf_head(lib_pdf_context_t* ctx) {

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

static int lib_pdf_margin(lib_pdf_context_t* ctx) {
    fprintf(stdout, "margin: %s%s;", ctx->margin, ctx->margin_unit);
    return 0;
}

static int lib_pdf_font(lib_pdf_context_t* ctx) {

    if (ctx->use_font_name) {
        fprintf(stdout, "font-family: %s;", ctx->font_name);
    }
    if (ctx->use_font_style) {
        fprintf(stdout, "font-style: %s;", ctx->font_style);
    }
    if (ctx->use_font_weight) {
        fprintf(stdout, "font-weight: %s;", ctx->font_weight);
    }
    if (ctx->use_font_size) {
        fprintf(stdout, "font-size: %s%s;", ctx->font_size, ctx->font_unit);
    }
    
    return 0;
}

static int lib_pdf_body(lib_pdf_context_t* ctx) {

    // if (ctx->use_style) {
    //     fprintf(stdout, "  <body style=\"");
    //     if (ctx->use_margin) {
    //        lib_pdf_margin(ctx);
    //     }
    //     if (ctx->use_font) {
    //        lib_pdf_font(ctx);
    //     }
    //     fprintf(stdout, "\">\n");
    // } else {
    //     fprintf(stdout, "  <body>\n");
    // }    

    // fprintf(stdout, "%s\n", ctx->data);
    // fprintf(stdout, "  </body>\n");

    fprintf(stdout, "%s\n", "%PDF-1.5");
    fprintf(stdout, "%s\n", "1 0 obj << /Pages 2 0 R /Type /Catalog >> endobj");
    fprintf(stdout, "%s\n", "2 0 obj << /Count 1 /Kids [3 0 R] /Type /Pages >> endobj");
    fprintf(stdout, "%s\n", "3 0 obj << /Contents 4 0 R /MediaBox [0 0 612 792] /Parent 2 0 R /Resources << /Font << /F1 5 0 R >> >> /Type /Page >> endobj");
    fprintf(stdout, "%s\n", "4 0 obj << /Length 44 >> stream");

    //fprintf(stdout, "%s\n", "BT /F1 24 Tf 72 720 Td (Hello ) Tj ET");

    fprintf(stdout, "%s", "BT /F1 24 Tf 72 720 Td (");
    fprintf(stdout, "%s", ctx->data);
    fprintf(stdout, "%s\n", ") Tj ET");

    fprintf(stdout, "%s\n", "endstream endobj");
    fprintf(stdout, "%s\n", "5 0 obj << /BaseFont /Helvetica /Encoding /WinAnsiEncoding /Subtype /Type1 /Type /Font >> endobj");
    fprintf(stdout, "%s\n", "xref");
    fprintf(stdout, "%s\n", "0 6");
    fprintf(stdout, "%s\n", "0000000000 65535 f ");
    fprintf(stdout, "%s\n", "0000000009 00000 n ");
    fprintf(stdout, "%s\n", "0000000062 00000 n ");
    fprintf(stdout, "%s\n", "0000000133 00000 n ");
    fprintf(stdout, "%s\n", "0000000277 00000 n ");
    fprintf(stdout, "%s\n", "0000000372 00000 n ");
    fprintf(stdout, "%s\n", "trailer << /Root 1 0 R /Size 6 >>");
    fprintf(stdout, "%s\n", "startxref");
    fprintf(stdout, "%s\n", "478");
    fprintf(stdout, "%s\n", "%%EOF");

    return 0;
}

static int run_pdf(lib_pdf_config_t* config, char* data, size_t size) {
    
    if (size == 0 || !data) {
        fprintf(stderr, "%s: No input data\n", prog_name);
        return 1;
    }

    lib_pdf_context_t ctx;
    lib_pdf_ctx_init(config, &ctx);
    ctx.data = data;
    ctx.size = size;

    return lib_pdf_document(&ctx);
}

