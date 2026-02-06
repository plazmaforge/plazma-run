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
typedef struct run_pdf_config_t {
    RUN_DOC_CONFIG
} run_pdf_config_t;

/**
 * PDF Context
 */
typedef struct run_pdf_context_t {
    RUN_DOC_CONTEXT

} run_pdf_context_t;

static int lib_pdf_document(run_pdf_context_t* ctx);

static int lib_pdf_head(run_pdf_context_t* ctx);

static int lib_pdf_margin(run_pdf_context_t* ctx);

static int lib_pdf_font(run_pdf_context_t* ctx);

static int lib_pdf_body(run_pdf_context_t* ctx);


static const char* lib_pdf_unitdef(const char* value) {
    return lib_is_digit(value) ? LIB_PDF_UNIT : "";
}

static int lib_pdf_init(run_pdf_config_t* cnf) {
    if (!cnf) {
        return 1;
    }

    return lib_doc_config_init((run_doc_config_t*) cnf);
}

static int lib_pdf_prepare(run_pdf_context_t* ctx) {
    if (!ctx) {
        return 1;
    }

    ctx->margin_unit    = lib_pdf_unitdef(ctx->margin);
    if (ctx->font) {
        ctx->font->unit = lib_pdf_unitdef(ctx->font->size);
    }

    return lib_doc_context_prepare((run_doc_context_t*) ctx);
}

static int lib_pdf_ctx_init(run_pdf_config_t* cnf, run_pdf_context_t* ctx) {
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

    return lib_pdf_prepare(ctx);
}

static int lib_pdf_document(run_pdf_context_t* ctx) {
    
    // DOCUMENT

    // HEAD
    if (ctx->use_head) {
        lib_pdf_head(ctx);
    }

    // BODY
    lib_pdf_body(ctx);
    
    return 0;
}

static int lib_pdf_head(run_pdf_context_t* ctx) {

    // if (ctx->use_charset) {
    // }
    // if (ctx->use_title) {
    // }

    return 0;
}

static int lib_pdf_margin(run_pdf_context_t* ctx) {
    // fprintf(ctx->out, "margin: %s%s;", ctx->margin, ctx->margin_unit);
    return 0;
}

static int lib_pdf_font(run_pdf_context_t* ctx) {

    // if (ctx->use_font_name) {
    //     fprintf(ctx->out, "font-family: %s;", ctx->font->name);
    // }
    // if (ctx->use_font_style) {
    //     fprintf(ctx->out, "font-style: %s;", ctx->font->style);
    // }
    // if (ctx->use_font_weight) {
    //     fprintf(ctx->out, "font-weight: %s;", ctx->font->weight);
    // }
    // if (ctx->use_font_size) {
    //     fprintf(ctx->out, "font-size: %s%s;", ctx->font->size, ctx->font->unit);
    // }
    
    return 0;
}

static int lib_pdf_body(run_pdf_context_t* ctx) {
    
    // if (ctx->use_style) {
    //     if (ctx->use_margin) {
    //        lib_pdf_margin(ctx);
    //     }
    //     if (ctx->use_font) {
    //        lib_pdf_font(ctx);
    //     }
    //     fprintf(ctx->out, "\">\n");
    // }

    int len      = 0;
    int offset   = 0;
    int offset_1 = 0;
    int offset_2 = 0;
    int offset_3 = 0;
    int offset_4 = 0;
    int offset_5 = 0;
    int offset_x = 0; // xref
    int page     = 0;
    int line     = 0;

    int page_width     = 612;
    int page_height    = 792;
    int margin_left    = 72;
    int margin_right   = 72;
    int margin_top     = 72;
    int margin_bottom  = 72;
    int line_offset    = 18;

    int content_width  = page_width - margin_left - margin_right;
    int content_height = page_height - margin_top - margin_bottom;
    int line_page      = content_width / line_offset + 1;

    const char* BUF_BT = "BT\n";                    // BEGIN
    const char* BUF_HD = "/F1 12 Tf 72 720 Td\n";   // HEADER
    const char* BUF_LF = "() Tj\n";                 // LINE FIRST
    const char* BUF_LN = "0 -18 Td\n() Tj\n";       // LINE NEXT
    const char* BUF_ET = "ET\n";                    // END

    int BUF_LEN    = 0;
    int BUF_LN_LEN = 0;

    BUF_LEN += strlen(BUF_BT);
    BUF_LEN += strlen(BUF_HD);
    BUF_LEN += strlen(BUF_LF);
    BUF_LEN += strlen(BUF_ET);

    BUF_LN_LEN = strlen(BUF_LN);

    // HEADER
    len = fprintf(ctx->out, "%%PDF-1.5\n"); // '%PDF-1.5': first '%' for fprint only (!)
    offset  += len;
    offset_1 = offset;

    // Catalog
    len = fprintf(ctx->out, "1 0 obj << /Pages 2 0 R /Type /Catalog >> endobj\n");
    offset  += len;
    offset_2 = offset;

    // Pages
    len = fprintf(ctx->out, "2 0 obj << /Count 1 /Kids [3 0 R] /Type /Pages >> endobj\n");
    offset  += len;
    offset_3 = offset;

    // Contents
    len = fprintf(ctx->out, "3 0 obj << /Contents 4 0 R /MediaBox [0 0 612 792] /Parent 2 0 R /Resources << /Font << /F1 5 0 R >> >> /Type /Page >> endobj\n");
    offset  += len;
    offset_4 = offset;

    // Streams
    int stream_len;
    char c;
    bool break_line;
    bool new_line;

    // Calculate
    stream_len = 0;
    page       = 1;
    line       = 1;

    break_line = false;
    new_line   = true;
    len        = 0;

    for (size_t i = 0; i < ctx->size; i++) {

        break_line = false;
        c = ctx->data[i];

        switch (c) {
        case '\n':
            break_line = true;
            break;
        case '\r':
            if (i + 1 < ctx->size && ctx->data[i + 1] == '\n') {
                i++;
            }
            break_line = true;
            break;
        default:
            len++;
            break;
        }

        new_line = break_line;
        if (break_line) {
            len += BUF_LN_LEN;
            if (line > line_page) {
                page++;
                line = 1;
            } else {
                line++;
            }            
        }
    }

    fprintf(stderr, ">> page_count: %d\n", page);

    stream_len  += len;

    // >>> Stream: start
    len = fprintf(ctx->out, "4 0 obj << /Length %d >> stream\n", stream_len);
    offset  += len;

    len = fprintf(ctx->out, "%s", BUF_BT);
    offset  += len;
    len = fprintf(ctx->out, "%s", BUF_HD);
    offset  += len;

    len = fprintf(ctx->out, "(");
    offset  += len;
    // >>>

    // Output
    break_line = false;
    new_line   = true;
    len        = 0;

    for (size_t i = 0; i < ctx->size; i++) {

        break_line = false;
        c = ctx->data[i];

        switch (c) {
        case '\n':
            break_line = true;
            break;
        case '\r':
            if (i + 1 < ctx->size && ctx->data[i + 1] == '\n') {
                i++;
            }
            break_line = true;
            break;
        default:
            len++;
            fprintf(ctx->out, "%c", c);
            break;
        }

        new_line = break_line;
        if (break_line) {
            len += fprintf(ctx->out, ") Tj\n");
            len += fprintf(ctx->out, "0 -18 Td\n(");
        }
    }


    // >>> Stream: end
    offset  += len;
    len = fprintf(ctx->out, ") Tj\n");
    offset  += len;

    len = fprintf(ctx->out, "%s", BUF_ET);
    offset  += len;

    len = fprintf(ctx->out, "endstream endobj\n");
    offset  += len;
    offset_5 = offset;
    // >>>

    len = fprintf(ctx->out, "5 0 obj << /BaseFont /Helvetica /Encoding /WinAnsiEncoding /Subtype /Type1 /Type /Font >> endobj\n");
    offset  += len;
    offset_x = offset;

    // XREF
    fprintf(ctx->out, "%s\n", "xref");
    fprintf(ctx->out, "%s\n", "0 6");
    fprintf(ctx->out, "%s\n", "0000000000 65535 f ");

    fprintf(ctx->out, "%010d 00000 n \n", offset_1);
    fprintf(ctx->out, "%010d 00000 n \n", offset_2);
    fprintf(ctx->out, "%010d 00000 n \n", offset_3);
    fprintf(ctx->out, "%010d 00000 n \n", offset_4);
    fprintf(ctx->out, "%010d 00000 n \n", offset_5);

    // TRAILER
    fprintf(ctx->out, "trailer << /Root 1 0 R /Size 6 >>\n");
    fprintf(ctx->out, "startxref\n");
    fprintf(ctx->out, "%d\n", offset_x);  // offset xref
    fprintf(ctx->out, "%%%%EOF\n");       // '%%EOF': first '%' for fprint only (!)

    // fprintf(stderr, "data_begin_len = %d\n", data_begin_len);
    // fprintf(stderr, "data_end_len   = %d\n", data_end_len);
    // fprintf(stderr, "data_len       = %d\n", data_len);
    // fprintf(stderr, "stream_len     = %d\n", stream_len);
    // fprintf(stderr, "offset_1       = %010d\n", offset_1);
    // fprintf(stderr, "offset_2       = %010d\n", offset_2);
    // fprintf(stderr, "offset_3       = %010d\n", offset_3);
    // fprintf(stderr, "offset_4       = %010d\n", offset_4);
    // fprintf(stderr, "offset_5       = %010d\n", offset_5);
    // fprintf(stderr, "offset_x       = %010d\n", offset_x);

    return 0;
}

static int run_pdf(run_pdf_config_t* config, char* data, size_t size) {
    
    if (size == 0 || !data) {
        fprintf(stderr, "%s: No input data\n", prog_name);
        return 1;
    }

    run_pdf_context_t ctx;
    int error = lib_pdf_ctx_init(config, &ctx);
    if (error != 0) {
        fprintf(stderr, "%s: Initialization error\n", prog_name);
        return error;
    }

    ctx.data = data;
    ctx.size = size;

    return lib_pdf_document(&ctx);
}

// https://medium.com/@jberkenbilt/the-structure-of-a-pdf-file-6f08114a58f6

