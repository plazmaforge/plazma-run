#include "iolib.h"
#include "run_doclib.h"

#define LIB_PDF_CHARSET     LIB_DOC_CHARSET
#define LIB_PDF_TITLE       NULL
#define LIB_PDF_UNIT        "pt"
#define LIB_PDF_MARGIN      "72"
#define LIB_PDF_FONT_NAME   "Helvetica"
#define LIB_PDF_FONT_STYLE  LIB_DOC_FONT_STYLE
#define LIB_PDF_FONT_WEIGHT LIB_DOC_FONT_WEIGHT
#define LIB_PDF_FONT_SIZE   "12"

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

typedef struct lib_font_info_t {
    const char* family;
    const char* name;
    bool bold;
    bool italic;
} lib_font_info_t;

// The 14 Standard PDF Fonts:
//
// - Courier (Standard, Bold, Oblique, BoldOblique)
// - Helvetica (Standard, Bold, Oblique, BoldOblique)
// - Times (Roman, Bold, Italic, BoldItalic)
// - Symbol
// - ZapfDingbats 

static const lib_font_info_t lib_fonts[] = {
    {"Courier", "Courier", false, false},
    {"Courier", "Courier-Bold", true, false},
    {"Courier", "Courier-Oblique", false, true},
    {"Courier", "Courier-BoldOblique", true, true},

    {"Helvetica", "Helvetica", false, false},
    {"Helvetica", "Helvetica-Bold", true, false},
    {"Helvetica", "Helvetica-Oblique", false, true},
    {"Helvetica", "Helvetica-BoldOblique", true, true},

    {"Times", "Times", false, false},
    {"Times", "Times-Bold", true, false},
    {"Times", "Times-Italic", false, true},
    {"Times", "Times-BoldItalic", true, true},

    {"Symbol", "Symbol", false, false},
    {"ZapfDingbats", "ZapfDingbats", false, false}

};

static const char* lib_get_font_family(const char* name);

static size_t lib_get_fonts_size();

static lib_font_info_t lib_get_font_info(lib_font_t* font);

////

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

static int lib_to_pt(const char* value) {
    // TODO: px/mm/cm -> pt
    return atoi(value);
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

    // if (ctx->font) {
    //     fprintf(stderr, ">> font->name  : %s\n", ctx->font->name);
    //     fprintf(stderr, ">> font->style : %s\n", ctx->font->style);
    //     fprintf(stderr, ">> font->bold  : %s\n", ctx->font->style ? (lib_doc_has_bold(ctx->font->style)   ? "true" : "false") : "false");
    //     fprintf(stderr, ">> font->italic: %s\n", ctx->font->style ? (lib_doc_has_italic(ctx->font->style) ? "true" : "false") : "false");
    // } else {
    //     fprintf(stderr, ">> font        : null\n");
    // }

    const char* pdf_version  = "1.5";
    const char* font_name    = "Helvetica";
    const char* font_subtype = "Type1";
    const char* encoding     = "WinAnsiEncoding";
    int font_size            = lib_to_pt(LIB_PDF_FONT_SIZE);

    // lib_font_info_t font     = lib_get_font_info(ctx->font);

    // // fprintf(stderr, "\n");
    // // fprintf(stderr, ">>>font->name  : %s\n", font.name);
    // // fprintf(stderr, ">>>font->bold  : %s\n", font.bold   ? "true" : "false");
    // // fprintf(stderr, ">>>font->italic: %s\n", font.italic ? "true" : "false");

    // if (font.name) {
    //     font_name = font.name;
    // }

    int len            = 0;
    int offset         = 0;
    int xref_size      = 0;
    int xref_offset    = 0;
    int ref            = 0;
    int page           = 0;
    int line           = 0;
    int page_count     = 0;

    int page_width     = 612;
    int page_height    = 792;

    int margin = lib_to_pt(LIB_PDF_MARGIN);
    int margin_left    = margin; // 72
    int margin_right   = margin; // 72
    int margin_top     = margin; // 72
    int margin_bottom  = margin; // 72
    int line_offset    = 18;

    if (ctx->use_style) {
        if (ctx->use_margin) {
            margin = lib_to_pt(ctx->margin);
            margin_left    = margin; // 72
            margin_right   = margin; // 72
            margin_top     = margin; // 72
            margin_bottom  = margin; // 72
        }
        if (ctx->use_font) {
            lib_font_info_t font     = lib_get_font_info(ctx->font);

            // fprintf(stderr, "\n");
            // fprintf(stderr, ">>>font->name  : %s\n", font.name);
            // fprintf(stderr, ">>>font->bold  : %s\n", font.bold   ? "true" : "false");
            // fprintf(stderr, ">>>font->italic: %s\n", font.italic ? "true" : "false");

            if (font.name) {
                font_name = font.name;
            }

            if (ctx->font->size > 0) {
                font_size = lib_to_pt(ctx->font->size);
                line_offset = font_size * 1.5;
            }
        }
    }

    int content_width  = page_width - margin_left - margin_right;
    int content_height = page_height - margin_top - margin_bottom;
    int line_page      = content_height / line_offset + 1;

    char* BUF_BT = "BT\n";                    // BEGIN
    char  BUF_HD[256];                        // HEADER
    char* BUF_LF = "() Tj\n";                 // LINE FIRST
    char  BUF_LN[256];                        // LINE NEXT
    char* BUF_ET = "ET\n";                    // END

    //const char* BUF_HD = "/F1 12 Tf 72 720 Td\n"; // HEADER
    //const char* BUF_LN = "0 -18 Td\n() Tj\n";      // LINE NEXT
    //const char* BUF_LN = "() Tj\n";                // LINE NEXT

    int BUF_LEN    = 0;
    int BUF_LN_LEN = 0;

    BUF_LEN += strlen(BUF_BT);
    BUF_LEN += sprintf(BUF_HD, "/F1 %d Tf 72 720 Td\n", font_size); // strlen(BUF_HD);
    BUF_LEN += strlen(BUF_LF);
    BUF_LEN += strlen(BUF_ET);

    BUF_LN_LEN = sprintf(BUF_LN, "0 -%d Td\n() Tj\n", line_offset); // strlen(BUF_LN);

    int MAX_PAGE = 8192;
    int MAX_XREF = 8192;

    int pages[MAX_PAGE];
    int xrefs[MAX_XREF];

    // Streams
    int stream_len;
    bool break_line;
    bool new_line;
    bool new_page;
    char c;

    // Calculate
    stream_len = 0;
    page       = 1;
    line       = 1;

    break_line = false;
    new_line   = true;
    new_page   = true;
    len        = 0;

    //int max_page = 1;
    stream_len = BUF_LEN;

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
        new_page = false;
        if (break_line) {
            len += BUF_LN_LEN;
            if (line > line_page) {
                //if (page + 1 > max_page) {
                //    break;
                //}
                stream_len += len;
                pages[page - 1] = stream_len;

                new_page = true;
                page++;
                line = 1;
                stream_len = BUF_LEN;
            } else {
                line++;
            }
        }
    }

    //fprintf(stderr, ">> page_count: %d\n", page);

    stream_len += len;
    pages[page - 1] += stream_len;

    // TODO: TEST
    //stream_len = pages[0];

    // Output

    int root_ref     = 0;
    int catalog_ref  = 0;
    int pages_ref    = 0;
    int page_ref     = 0; // foreach
    int contents_ref = 0; // foreach 
    int font_ref     = 0;

    // HEADER
    len = fprintf(ctx->out, "%%PDF-%s\n", pdf_version); // '%PDF-': first '%' for fprint only (!)
    offset  += len;
    xrefs[ref] = offset;

    // Type: Catalog
    ref++; // 1-Catalog/Root
    catalog_ref = ref;
    root_ref = catalog_ref;
    len = fprintf(ctx->out, "%d 0 obj << /Type /Catalog /Pages %d 0 R >> endobj\n", ref, ref + 1);
    offset  += len;
    xrefs[ref] = offset;

    page_count = page;

    // Type: Pages
    ref++; // 2-Pages
    pages_ref = ref;
    len = fprintf(ctx->out, "%d 0 obj << /Type /Pages /Count %d /Kids [", ref, page_count);
    offset  += len;

    // Pages: foreach
    page_ref  = ref;
    for (size_t i = 0; i < page_count; i++) {
        page_ref++;
        if (page_count > 1) {
           len = fprintf(ctx->out, "\n ");
           offset += len;
        }
        len = fprintf(ctx->out, "%d 0 R", page_ref);
        offset += len;
    }

    if (page_count > 1) {
        len = fprintf(ctx->out, "\n");
        offset += len;
    }
    len = fprintf(ctx->out, "] >> endobj\n");
    offset  += len;
    xrefs[ref] = offset;

    font_ref = pages_ref + (page * 2) + 1;

    // Pages
    contents_ref = page_ref;

    // Pages: foreach
    for (size_t i = 0; i < page_count; i++) {
        // Type: Page
        ref++; // 3-Page<N>
        contents_ref++;
        len = fprintf(ctx->out, "%d 0 obj << /Type /Page /Contents %d 0 R /MediaBox [0 0 %d %d] /Parent %d 0 R /Resources << /Font << /F1 %d 0 R >> >> >> endobj\n", ref, contents_ref, page_width, page_height, pages_ref, font_ref);
        offset += len;
        xrefs[ref] = offset;
    }

    // Contents

    // Output
    page       = 1;
    line       = 1;
    
    break_line = false;
    new_line   = true;
    new_page   = true;
    len        = 0;

    // >>> Stream: start
    ref++; // 4-Stream<N>
    len = fprintf(ctx->out, "%d 0 obj << /Length %d >> stream\n", ref, pages[page - 1]);
    offset  += len;
    len = fprintf(ctx->out, "%s", BUF_BT);
    offset  += len;
    len = fprintf(ctx->out, "%s", BUF_HD);
    offset  += len;
    len = fprintf(ctx->out, "(");
    offset  += len;
    // >>>

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
        new_page = false;
        if (break_line) {
            if (line > line_page) {
                //if (page + 1 > max_page) {
                //    break;
                //}

    // >>> Stream: end
    offset  += len;
    len = fprintf(ctx->out, ") Tj\n");
    offset  += len;
    len = fprintf(ctx->out, "%s", BUF_ET);
    offset  += len;
    len = fprintf(ctx->out, "endstream endobj\n");
    offset  += len;
    xrefs[ref] = offset;
    // >>>

                new_page = true;
                page++;
                line = 1;
                len  = 0;

    // >>> Stream: start
    ref++; // 4-Stream<N>
    len = fprintf(ctx->out, "%d 0 obj << /Length %d >> stream\n", ref, pages[page - 1]);
    offset  += len;
    len = fprintf(ctx->out, "%s", BUF_BT);
    offset  += len;
    len = fprintf(ctx->out, "%s", BUF_HD);
    offset  += len;
    len = fprintf(ctx->out, "(");
    offset  += len;
    // >>>

            } else {
                line++;
            }
            
            len += fprintf(ctx->out, ") Tj\n");                   // BUF_LN_END
            len += fprintf(ctx->out, "0 -%d Td\n(", line_offset); // BUF_LN_BEGIN
            //len += fprintf(ctx->out, "0 -18 Td\n(");
            //len += fprintf(ctx->out, "(");
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
    xrefs[ref] = offset;
    // >>>

    // Type: Font
    ref++; // 5-Font
    len = fprintf(ctx->out, "%d 0 obj << /Type /Font /Subtype /%s /BaseFont /%s /Encoding /%s >> endobj\n", ref, font_subtype, font_name, encoding);
    offset  += len;
    xrefs[ref] = offset;

    xref_size   = ref + 1;
    xref_offset = offset;

    // XREF
    fprintf(ctx->out, "%s\n", "xref");
    fprintf(ctx->out, "0 %d\n", xref_size);
    fprintf(ctx->out, "%s\n", "0000000000 65535 f ");

    for (size_t i = 0; i < ref; i++) {
        fprintf(ctx->out, "%010d 00000 n \n", xrefs[i]);
    }

    // TRAILER
    fprintf(ctx->out, "trailer << /Root %d 0 R /Size %d >>\n", root_ref, xref_size);
    fprintf(ctx->out, "startxref\n");
    fprintf(ctx->out, "%d\n", xref_offset);

    // EOF
    fprintf(ctx->out, "%%%%EOF\n");       // '%%EOF': first '%' for fprint only (!)

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

static const char* lib_get_font_family(const char* name) {
    if (!name) {
        return NULL;
    }
    if (lib_stricmp(name, "Times New Roman") == 0) {
        return "Times";
    }
    return name;
}

static size_t lib_get_fonts_size() {
  return sizeof(lib_fonts) / sizeof(lib_font_info_t);
}

static lib_font_info_t lib_get_font_info(lib_font_t* font) {
    
    lib_font_info_t res;
    res.family = NULL;
    res.name   = NULL;
    res.bold   = false;
    res.italic = false;

    if (!font || !(font->name)) {
        return res;
    }

    lib_font_info_t cur;
    const char* family = lib_get_font_family(font->name);
    bool bold   = lib_doc_has_bold(font->style);
    bool italic = lib_doc_has_italic(font->style);
    size_t size = lib_get_fonts_size();

    // fprintf(stderr, "\n");
    // fprintf(stderr, ">>>      family: %s\n", family);
    // fprintf(stderr, ">>>      bold  : %s\n", bold   ? "true" : "false");
    // fprintf(stderr, ">>>      italic: %s\n", italic ? "true" : "false");
    // fprintf(stderr, ">>>        size: %d\n", size);

    for (size_t i = 0; i < size; i++) {
        cur = lib_fonts[i];

        // fprintf(stderr, "\n");
        // fprintf(stderr, ">>>cur.family  : %s\n", cur.family);
        // fprintf(stderr, ">>>cur.name    : %s\n", cur.name);
        // fprintf(stderr, ">>>cur.bold    : %s\n", cur.bold   ? "true" : "false");
        // fprintf(stderr, ">>>cur.italic  : %s\n", cur.italic ? "true" : "false");

        if (lib_stricmp(cur.family, family) == 0) {
            // fprintf(stderr, ">>>        find: %s\n", family);
            if (cur.bold == bold && cur.italic == italic) {
                // fprintf(stderr, ">>>        fcur: %s\n", family);
                return cur;
            } else if (!cur.bold && !cur.italic) {
                res = cur; 
            }
        }
    }
    // fprintf(stderr, ">>>        fres: %s\n", family);
    return res;
}

// PDF Format structure
// https://medium.com/@jberkenbilt/the-structure-of-a-pdf-file-6f08114a58f6

// Multi line
//
// 4 0 obj<</Length 84>>stream
// BT 150 700 Td /F0 32 Tf 30 TL(FIRST LINE)' T*(Second line)' (Third line)' ET
// endstream endobj
//
// https://stackoverflow.com/questions/17279712/what-is-the-smallest-possible-valid-pdf
