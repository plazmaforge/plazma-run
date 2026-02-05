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
    RUN_DOC_CONFIG
} lib_pdf_config_t;

/**
 * PDF Context
 */
typedef struct lib_pdf_context_t {
    RUN_DOC_CONTEXT

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

    return lib_doc_config_init((run_doc_config_t*) cnf);
}

static int lib_pdf_prepare(lib_pdf_context_t* ctx) {
    if (!ctx) {
        return 1;
    }

    ctx->margin_unit    = lib_pdf_unitdef(ctx->margin);
    if (ctx->font) {
        ctx->font->unit = lib_pdf_unitdef(ctx->font->size);
    }

    return lib_doc_context_prepare((run_doc_context_t*) ctx);
}

static int lib_pdf_ctx_init(lib_pdf_config_t* cnf, lib_pdf_context_t* ctx) {
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

static int lib_pdf_document(lib_pdf_context_t* ctx) {
    
    // DOCUMENT

    // HEAD
    if (ctx->use_head) {
        lib_pdf_head(ctx);
    }

    // BODY
    lib_pdf_body(ctx);
    
    return 0;
}

static int lib_pdf_head(lib_pdf_context_t* ctx) {

    // if (ctx->use_charset) {
    // }
    // if (ctx->use_title) {
    // }

    return 0;
}

static int lib_pdf_margin(lib_pdf_context_t* ctx) {
    // fprintf(ctx->out, "margin: %s%s;", ctx->margin, ctx->margin_unit);
    return 0;
}

static int lib_pdf_font(lib_pdf_context_t* ctx) {

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

static int lib_pdf_body(lib_pdf_context_t* ctx) {
    
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

    // HEADER
    len = fprintf(ctx->out, "%%PDF-1.5\n"); // '%PDF-1.5': first '%' for fprint only (!)
    offset  += len;
    offset_1 = offset;

    len = fprintf(ctx->out, "1 0 obj << /Pages 2 0 R /Type /Catalog >> endobj\n");
    offset  += len;
    offset_2 = offset;

    len = fprintf(ctx->out, "2 0 obj << /Count 1 /Kids [3 0 R] /Type /Pages >> endobj\n");
    offset  += len;
    offset_3 = offset;

    len = fprintf(ctx->out, "3 0 obj << /Contents 4 0 R /MediaBox [0 0 612 792] /Parent 2 0 R /Resources << /Font << /F1 5 0 R >> >> /Type /Page >> endobj\n");
    offset  += len;
    offset_4 = offset;

    // STREAM

    const char* BUF_BT = "BT\n";                    // BEGIN
    const char* BUF_HD = "/F1 12 Tf 72 720 Td\n";   // HEADER
    const char* BUF_LF = "() Tj\n";                 // LINE FIRST
    const char* BUF_LN = "0 -18 Td\n() Tj\n";       // LINE NEXT
    const char* BUF_ET = "ET\n";                    // END

    //const char* data_end   = ") Tj ET\n";
    //const char* data_begin = "BT /F1 24 Tf 72 720 Td (";
    //const char* data_end   = ") Tj ET\n";

    //const char* data_end   = "0 -18 Td\n (...) Tj\n";

    //int data_begin_len = strlen(data_begin);
    //int data_end_len   = strlen(data_end);
    //int data_len       = ctx->size;
    //int stream_len     = data_begin_len + data_len + data_end_len;

    // fprintf(ctx->out, "4 0 obj << /Length 44 >> stream\n");
    // fprintf(ctx->out, "BT /F1 24 Tf 72 720 Td (%s) Tj ET\n", ctx->data);
    // fprintf(ctx->out, "endstream endobj\n");

    int stream_len = 0;
    stream_len  += strlen(BUF_BT);
    stream_len  += strlen(BUF_HD);
    stream_len  += strlen(BUF_LF);
    stream_len  += strlen(BUF_ET);

    int BUF_LN_LEN = strlen(BUF_LN);

    char c;
    bool break_line;
    bool new_line;

    break_line = false;
    new_line = true;
    len = 0;

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
        }
    }

    //stream_len  += ctx->size;
    stream_len  += len;

    len = fprintf(ctx->out, "4 0 obj << /Length %d >> stream\n", stream_len);
    offset  += len;

    //len = fprintf(ctx->out, "%s", data_begin);
    //offset  += len;

    len = fprintf(ctx->out, "%s", BUF_BT);
    offset  += len;
    len = fprintf(ctx->out, "%s", BUF_HD);
    offset  += len;

    len = fprintf(ctx->out, "(");
    offset  += len;

    // "() Tj\n"
    //len = fprintf(ctx->out, "(%s) Tj\n", ctx->data);
    //offset  += len;

    break_line = false;
    new_line = true;
    len = 0;

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

    offset  += len;
    len = fprintf(ctx->out, ") Tj\n");
    offset  += len;

    // // >>> CONTENT: BEGIN
    // len = fprintf(ctx->out, "%s", ctx->data);
    // offset  += len;
    // // >>> CONTENT: END

    len = fprintf(ctx->out, "%s", BUF_ET);
    offset  += len;

    //len = fprintf(ctx->out, "%s", data_end);
    //offset  += len;

    len = fprintf(ctx->out, "endstream endobj\n");
    offset  += len;
    offset_5 = offset;

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

static int run_pdf(lib_pdf_config_t* config, char* data, size_t size) {
    
    if (size == 0 || !data) {
        fprintf(stderr, "%s: No input data\n", prog_name);
        return 1;
    }

    lib_pdf_context_t ctx;
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

