#include "iolib.h"
#include "run_doclib.h"

#include "intlib.h"
#include "unimap.h"
#include "enclib.h"

#define LIB_PDF_ENCODING    NULL
#define LIB_PDF_TITLE       NULL
#define LIB_PDF_UNIT        "pt"
#define LIB_PDF_MARGIN      "72"                 // 72 pt = 1 inch
#define LIB_PDF_FONT_NAME   "Helvetica"
#define LIB_PDF_FONT_STYLE  LIB_DOC_FONT_STYLE
#define LIB_PDF_FONT_WEIGHT LIB_DOC_FONT_WEIGHT
#define LIB_PDF_FONT_SIZE   "12"                 // 12 pt

/**
 * PDF Config
 */
typedef struct run_pdf_config_t {
    RUN_DOC_CONFIG
    bool use_unicode;
} run_pdf_config_t;

/**
 * PDF Context
 */
typedef struct run_pdf_context_t {
    RUN_DOC_CONTEXT
    bool use_unicode;
} run_pdf_context_t;

typedef struct lib_font_info_t {
    const char* family;
    const char* name;
    bool bold;
    bool italic;
} lib_font_info_t;

typedef struct lib_pdf_cmap_t {
    int icode; // inp code
    int ucode; // uni code
    int idx;   // index
    int width; // char width
    bool is_predef;
} lib_pdf_cmap_t;

typedef struct lib_pdf_char_t {
    uint32_t ucode;
    int idx;
    int width;
} lib_pdf_char_t;

typedef struct lib_pdf_line_t {
    lib_pdf_char_t buf[65536];
    int width;
    int len;
    int pos2;
    int len2;
    bool flush;

} lib_pdf_line_t;

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
    cnf->use_unicode = false;
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
    ctx->charset     = cnf->charset;
    ctx->encoding    = cnf->encoding;
    ctx->encoding_id = cnf->encoding_id;
    ctx->title       = cnf->title;
    ctx->margin      = cnf->margin;
    ctx->font        = cnf->font;
    ctx->data        = NULL;
    ctx->size        = 0;

    ctx->use_unicode   = cnf->use_unicode;
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

/**
 * Create predefined CMap [0..255]
 * By default use ASCII code page.
 * Bu we can use other code pages.
 * For example: CP1251.
 */
static int _cmap_init(lib_pdf_cmap_t* cmap) {
    
    int size = 256;
    lib_pdf_cmap_t e;
    int cmap_idx  = 0;

    for (int i = 0; i < size; i++) {

        cmap_idx = i;

        e.idx   = cmap_idx;
        e.icode = cmap_idx;
        e.ucode = cmap_idx;
        e.width = 700;     // TODO: Use font info to get width of char

        bool block_a =  (cmap_idx >= 32 && cmap_idx <= 127);  // [032..127] ASCII
        bool block_b =  (cmap_idx >= 160 && cmap_idx <= 255); // [160..255] NLS

        e.is_predef = block_a || block_b;

        cmap[i] = e;
    }
    return size;
}

/**
 * Find icode in CMap
 */
static lib_pdf_cmap_t* _cmap_find_by_icode(lib_pdf_cmap_t* cmap, int size, int icode) {
    lib_pdf_cmap_t* p = cmap;
    for (int i = 0; i < size; i++) {
        if (icode == p->icode) {
            return p;
        }
        p++;
    }
    return NULL;
}

/**
 * Find ucode in CMap
 */
static lib_pdf_cmap_t* _cmap_find_by_ucode(lib_pdf_cmap_t* cmap, int size, int ucode) {
    lib_pdf_cmap_t* p = cmap;
    for (int i = 0; i < size; i++) {
        if (ucode == p->ucode) {
            return p;
        }
        p++;
    }
    return NULL;
}

static bool _cmap_has_icode(lib_pdf_cmap_t* cmap, int size, int icode) {
    return _cmap_find_by_icode(cmap, size, icode) != NULL;
}

static bool _cmap_has_ucode(lib_pdf_cmap_t* cmap, int size, int ucode) {
    return _cmap_find_by_ucode(cmap, size, ucode) != NULL;
}

/**
 * Find and return CMap index of first not predefined element
 */
static int _cmap_find_gap(lib_pdf_cmap_t* cmap, int size, int cur) {
    lib_pdf_cmap_t* p = cmap;
    int start = cur + 1;
    p += start;
    for (int i = start; i < size; i++) {
        if (!(p->is_predef)) {
            return i;
        }
        p++;
    }
    return -1;
}

/**
 * Dump CMap in DEBUG mode
 */
static void _cmap_dump(lib_pdf_cmap_t* cmap, int size) {
    lib_pdf_cmap_t* p = cmap;
    for (int i = 0; i < size; i++) {
        fprintf(stderr, "idx: %d, icode: %d, ucode: %d, width: %d, [%s]\n", p->idx, p->icode, p->ucode, p->width, (p->is_predef ? "+" : " "));
        p++;
    }
}

static bool _is_br_sep(uint32_t ucode) {
    return ucode == ' ';
}

static bool _is_nonbr_sep(uint32_t ucode) {
    return ucode == ',' || ucode == ';';
}

static int _find_br_sep(uint32_t* buf, int len) {
    for (int k = len - 1; k--; k <= 0) {
        if (_is_br_sep(buf[k])) {
            return k;
        }
    }
    return -1;
}

int _line_init(lib_pdf_line_t* line) {
    if (!line) {
        return 1;
    }
    line->width  = 0;
    //int idx    = -1;
    line->len    = 0;
    line->pos2   = -1;
    line->len2   = 0;
    line->flush = false;
    return 0;
}

static int _line_find_br_sep(lib_pdf_char_t* buf, int len) {
    for (int k = len - 1; k--; k <= 0) {
        if (_is_br_sep(buf[k].ucode)) {
            return k;
        }
    }
    return -1;
}

static int _line_flush_1(lib_pdf_line_t* line, bool use_unicode) {
    if (!line) {
        return 0;
    }
    int len = 0;
    // FLUSH-1
    if (use_unicode) {
        for (int k = 0; k < line->len; k++) {
            len += 2; // <xx>
        }
    } else {
        for (int k = 0; k < line->len; k++) {
            len++;
        }
    }

    line->width = 0;
    //line->idx   = -1;
    line->len   = 0;
}

static int _line_flush_2(lib_pdf_line_t* line) {
    if (!line) {
        return 1;
    }

    int shift = line->pos2 == 0 ? 1 : line->pos2; 
    for (int k = line->len2; k < line->len2; k++) {
        line->buf[k] = line->buf[k + shift];
    }
    line->len  = line->len2;
    line->pos2 = -1;
    line->len2 = 0;
}

static int _line_proc(lib_pdf_line_t* line) {
    if (!line) {
        return 1;
    }

                        //>>
    line->pos2 = _line_find_br_sep(line->buf, line->len);
    line->len2 = 0;
    if (line->pos2 >= 0) {
        int shift = line->pos2 == 0 ? 1 : line->pos2;
        line->len2 = line->len - shift;
        line->len = shift; 
    }

    //line_width = e.width;
    line->width = 0;
    //line->flush = true;
}

static int lib_pdf_body(run_pdf_context_t* ctx) {

    const char* pdf_version  = "1.5";
    const char* pdf_encoding = "WinAnsiEncoding";
    const char* font_name    = "Helvetica";
    const char* font_subtype = "Type1";
    int font_size            = lib_to_pt(LIB_PDF_FONT_SIZE);

    int len            = 0;
    int offset         = 0;
    int xref_size      = 0;
    int xref_offset    = 0;
    int ref            = 0;
    int page           = 0;
    int line           = 0;
    int page_count     = 0;

    // A4       : 595 x 842 pt, 210.0 x 297.0 mm
    // US Latter: 612 x 792 pt, 215.9 x 279.4 mm
    int page_width     = 612;
    int page_height    = 792;

    int margin = lib_to_pt(LIB_PDF_MARGIN); // by default: 72 pt = 1 inch 
    int margin_left    = margin;
    int margin_right   = margin;
    int margin_top     = margin;
    int margin_bottom  = margin;
    int line_offset    = 18; // by default: font size = 12 pt, line offset = font size x 1.5

    const char* encoding = ctx->encoding;
    int encoding_id      = ctx->encoding_id;
    bool use_unicode     = ctx->use_unicode;

    lib_unimap_t unimap;
    if (use_unicode) {
        if (encoding_id <= 0) {
            fprintf(stderr, "%s: Encoding %s is not supported\n", prog_name, encoding);
            return 1;
        }
        lib_unimap_get_unimap_by_id(&unimap, encoding_id);
    }
    
    // fprintf(stderr, ">>>unimap.id    : %d\n", unimap.id);
    // fprintf(stderr, ">>>unimap.start : %d\n", unimap.start);
    // fprintf(stderr, ">>>unimap.len   : %d\n", unimap.len);

    if (ctx->use_style) {
        if (ctx->use_margin) {
            margin = lib_to_pt(ctx->margin);
            margin_left    = margin;
            margin_right   = margin;
            margin_top     = margin;
            margin_bottom  = margin;
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

    char* BUF_BT = "BT\n";                              // BEGIN
    char  BUF_HD[128];                                  // HEADER
    char* BUF_LF = use_unicode ? "<> Tj\n" : "() Tj\n"; // LINE FIRST
    char  BUF_LN[128];                                  // LINE NEXT
    char* BUF_ET = "ET\n";                              // END

    //const char* BUF_HD = "/F1 12 Tf 72 720 Td\n";  // HEADER
    //const char* BUF_LN = "0 -18 Td\n() Tj\n";      // LINE NEXT
    //const char* BUF_LN = "() Tj\n";                // LINE NEXT

    int BUF_LEN    = 0; // template len
    int BUF_LN_LEN = 0; // tempate line next len

    BUF_LEN += strlen(BUF_BT);
    BUF_LEN += sprintf(BUF_HD, "/F1 %d Tf %d %d Td\n", font_size, margin_left, page_height - margin_top);
    BUF_LEN += strlen(BUF_LF);
    BUF_LEN += strlen(BUF_ET);

    BUF_LN_LEN = sprintf(BUF_LN, use_unicode ? "0 -%d Td\n<> Tj\n" : "0 -%d Td\n() Tj\n", line_offset);

    int MAX_PAGE = 8192;
    int MAX_XREF = 8192;

    int pages[MAX_PAGE];
    int xrefs[MAX_XREF];

    // Streams
    int page_len;
    bool break_line;
    bool new_line;
    bool new_page;
    //char c;

    // Calculate
    page_len   = 0;
    page       = 1;
    line       = 1;

    break_line = false;
    new_line   = true;
    new_page   = true;
    len        = 0;

    //int max_page = 1;
    page_len = BUF_LEN; // template len

    lib_pdf_cmap_t cmap[65536];
    int cmap_size       = 0; // CMap size
    int cmap_idx        = 0; // CMap index
    int carr_idx        = 0; // CMap array index
    int cmap_gap        = -1;// CMap gap index
    int cmap_start      = 0; // CMap start index
    int cmap_digits     = 2; // CMap output digits in index: <XX> or <XXXX>
    bool cmap_found     = false; // CMap found flag

    size_t i            = 0;
    int icode           = 0; // inp code
    int ucode           = 0; // uni code (code point)
    int next            = 0; // next uni code (code point)
    int seq             = 0; // sequence of char
    char* data = ctx->data;

    bool use_predef = true;

    if (use_unicode) {
        if (use_predef) {
            // Initialize predefined map (ASCII)
            cmap_start  = 0;
            cmap_size   = _cmap_init(cmap);
        } else {
            lib_pdf_cmap_t e;
            cmap_start  = 1;
            cmap_size   = 1;
            e.icode     = 0;
            e.ucode     = 0;
            e.width     = 0;
            e.idx       = 0;
            e.is_predef = false;
            cmap[0] = e;
        }
    }

    bool debug = false;
    if (debug) {
        fprintf(stderr, "CMap dump-1\n");
        _cmap_dump(cmap, cmap_size);
    }

    bool use_break_line = true;
    int body_width = use_unicode ? 38000 : 50000; // TODO: STUB

    //uint32_t line_buf[65536];
    //int line_width  = 0;
    //int line_idx    = -1;
    //int line_len    = 0;
    //int line_pos2   = -1;
    //int line_len2   = 0;
    //bool line_flush = false;
    
    lib_pdf_line_t line_buf_v;
    lib_pdf_line_t* line_buf = &line_buf_v;

    _line_init(line_buf);

    // Preprocessing
    while (i < ctx->size) {

        icode = _u8(*data); // TODO: For one byte only 
        if (use_unicode) {
            seq = lib_enc_to_code(&unimap, encoding_id, data, &ucode);
        } else {
            seq = 1;
            ucode = icode;
        }

        break_line = false;

        switch (ucode) {
        case '\n':
            break_line = true;
            break;
        case '\r':
            if (i + 1 < ctx->size) {
                // TODO: One byte only?
                next = _u8(*(data + 1)); // get next code
                if (next == '\n') {
                    data++;
                    i++;
                }
            }
            break_line = true;
            break;
        default:

            if (ucode == '(' || ucode == ')') {
                if (!use_unicode) {
                    len++;
                }
            }

            //>>
            if (use_unicode) {
                cmap_found = false;
                lib_pdf_cmap_t* p;
                lib_pdf_cmap_t  e;

                // >> REMOVE IT
                //int icode = _u8(*data);
                //if (icode != code) {
                //    fprintf(stderr, ">> NOTEQ-1: icode=%d, code=%d\n", icode, code);
                //}
                // >>

                p = _cmap_find_by_ucode(cmap, cmap_size, ucode);
                cmap_found = p != NULL;
                bool success = false;

                if (cmap_found) {
                    success = true;
                }

                if (debug) {
                    fprintf(stderr, ">> _cmap_find_by_ucode: ucode=%d [%s]\n", ucode, (cmap_found ? "+" : " "));
                }

                if (!success) {

                    if (ucode == NO_CHR) {
                        // Not found char in UniMap
                        fprintf(stderr, ">> Not found code in UniMap [NO_CHR]: %d\n", icode);
                    } else {

                        if (use_predef) {
                            cmap_idx = _cmap_find_gap(cmap, cmap_size, cmap_gap);

                            if (debug) {
                                fprintf(stderr, ">> _cmap_find_gap: cmap_size=%d, cmap_gap=%d, cmap_idx=%d\n", cmap_size, cmap_gap, cmap_idx);
                            }

                            if (cmap_idx < 0) {
                                fprintf(stderr, ">> Not found gap in CMap: %d, %d\n", cmap_size, cmap_gap);
                            } else {
                                success = true;
                                cmap_gap = cmap_idx; 
                                carr_idx = cmap_idx;
                            }
                        } else {
                            success = true;
                            carr_idx = cmap_size;
                            cmap_size++;
                            cmap_idx++;
                        }

                    }
                }

                if (success) {

                    //>>
                    if (!cmap_found) {
                        //e.icode = icode;
                        e.ucode = ucode;
                        e.width = 700;     // TODO: Use font info to get width of char
                        //e.idx   = cmap_size;
                        e.idx   = cmap_idx;
                        e.is_predef = false;
                        cmap[carr_idx] = e;
                    }
                    //>>

                    //>>
                    if (use_break_line) {

                        line_buf->buf[line_buf->len].ucode = ucode;
                        line_buf->buf[line_buf->len].idx   = cmap_idx;
                        line_buf->buf[line_buf->len].idx   = e.width;
                        line_buf->len++;
                        line_buf->width += e.width;
                        line_buf->flush = false;

                        // Break Line Algo
                        if (line_buf->width >= body_width) {
                            _line_proc(line_buf);
                            line_buf->flush = true;
                            break_line = true;
                        }

                        // FLUSH-1
                        if (line_buf->flush) {
                            _line_flush_1(line_buf, use_unicode);
                            line_buf->flush = false;
                        }
                        
                    } else {
                        len += 2; // <xx>
                    }
                    //>>

                }

            } else {

                //>>
                if (use_break_line) {

                    line_buf->buf[line_buf->len].ucode = ucode;
                    line_buf->buf[line_buf->len].idx   = cmap_idx;
                    line_buf->buf[line_buf->len].idx   = 700;
                    line_buf->len++;
                    line_buf->width += 700;
                    line_buf->flush = false;

                    // Break Line Algo
                    if (line_buf->width>= body_width) {
                        _line_proc(line_buf);
                        line_buf->flush = true;
                        break_line = true;
                    }

                    // FLUSH-1
                    if (line_buf->flush) {
                        _line_flush_1(line_buf, use_unicode);
                        line_buf->flush = false;
                    }

                } else {
                    len++;
                }
                //>>
                
            }
            //>>

            break;
        }

        new_line = break_line;
        new_page = false;
        if (break_line) {

            // FLUSH-1
            _line_flush_1(line_buf, use_unicode);
            line_buf->flush = false;

            len += BUF_LN_LEN;
            if (line > line_page) {
                //if (page + 1 > max_page) {
                //    break;
                //}

                // CLOSE CUR PAGE
                page_len += len;
                pages[page - 1] = page_len;

                // OPEN NEW PAGE
                new_page = true;
                page++;
                line = 1;
                len  = 0;
                page_len = BUF_LEN; // template len
            } else {
                line++;
            }

            // FLUSH-2
            if (line_buf->len2 > 0) {
                _line_flush_2(line_buf);

            }

        }

        data += seq;
        i += seq;
    }

    //>>
    // FLUSH-1
    _line_flush_1(line_buf, use_unicode);
     line_buf->flush = false;
    //>>

    if (debug) {
        fprintf(stderr, "\n");
        fprintf(stderr, "CMap dump-2\n");
        _cmap_dump(cmap, cmap_size);
    }

    //fprintf(stderr, ">> page_count: %d\n", page);

    // last page
    page_len += len;
    pages[page - 1] += page_len;

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
    page       = 1;
    line       = 1;
    
    break_line = false;
    new_line   = true;
    new_page   = true;
    len        = 0;

    // >>> Stream: start
    ref++; // 4-Stream<N>
    len += fprintf(ctx->out, "%d 0 obj << /Length %d >> stream\n", ref, pages[page - 1]);
    len += fprintf(ctx->out, "%s", BUF_BT);
    len += fprintf(ctx->out, "%s", BUF_HD);
    len += fprintf(ctx->out, use_unicode ? "<" : "(");
    offset  += len;
    // >>>

    i          = 0;
    icode      = 0; // inp code
    ucode      = 0; // uni code (code point)
    seq        = 0; // sequence of char
    data       = ctx->data;

    // line_width = 0;
    // line_idx   = -1;
    // line_len   = 0;
    
    // Output
    while (i < ctx->size) {

        icode = _u8(*data);
        if (use_unicode) {
            seq = lib_enc_to_code(&unimap, encoding_id, data, &ucode);
        } else {
            seq = 1;
            ucode = icode;
        }

        break_line = false;

        switch (ucode) {
        case '\n':
            //fprintf(stderr, "[N]: %d\n", i);
            break_line = true;
            break;
        case '\r':
            if (i + 1 < ctx->size) {
                // TODO: One byte only?
                next = _u8(*(data + 1)); // get next code
                if (next == '\n') {
                    data++;
                    i++;
                }
            }
            break_line = true;
            break;
        default:

            if (ucode == '(' || ucode == ')') {
                if (!use_unicode) {
                    len++;
                }
            }

            //>>
            if (use_unicode) {
                lib_pdf_cmap_t* p;
                cmap_found = false;
                p = _cmap_find_by_ucode(cmap, cmap_size, ucode);
                cmap_found = p != NULL;

                if (!cmap_found) {
                    // TODO: ERROR
                    fprintf(stderr, "Char not found in CMap: icode=%d, ucode=%d\n", icode, ucode);
                } else {
                    int idx = p->idx;

                    //>>
                    if (use_break_line) {

                        line_buf->buf[line_buf->len].ucode = ucode;
                        line_buf->buf[line_buf->len].idx   = cmap_idx;
                        line_buf->buf[line_buf->len].idx   = p->width;
                        line_buf->len++;
                        line_buf->width += p->width;
                        line_buf->flush = false;

                        // Break Line Algo
                        if (line_buf->width  >= body_width) {
                            //>>
                            _line_proc(line_buf);
                            line_buf->flush = true;
                            break_line = true;
                        }

                        // FLUSH-1
                        if (line_buf->flush) {
                            for (int k = 0; k < line_buf->len; k++) {
                                len += 2; // <xx>
                                int idx = line_buf->buf[k].idx;
                                fprintf(ctx->out, "%02X", idx);
                            }
                            //line_idx = -1;
                            line_buf->len = 0;
                            line_buf->flush = false;
                        }

                     } else {
                        len += 2; // <xx>
                        fprintf(ctx->out, "%02X", idx);
                    }
                    //>>

                }
            } else {

                //>>
                if (use_break_line) {

                        line_buf->buf[line_buf->len].ucode = ucode;
                        line_buf->buf[line_buf->len].idx   = cmap_idx;
                        line_buf->buf[line_buf->len].idx   = 700;
                        line_buf->len++;
                        line_buf->width += 700; //p->width;
                        line_buf->flush = false;

                    // Break Line Algo
                    if (line_buf->width >= body_width) {

                        //>>
                        _line_proc(line_buf);
                        line_buf->flush = true;
                        break_line = true;
                    }

                    // FLUSH-1
                    if (line_buf->flush) {
                        for (int k = 0; k < line_buf->len; k++) {
                            len++;
                            ucode = line_buf->buf[k].ucode;
                            if (ucode == '(') {
                                fprintf(ctx->out, "\\(");
                            } else if (ucode == ')') {
                                fprintf(ctx->out, "\\)");
                            } else {
                                fprintf(ctx->out, "%c", (char) ucode);
                            }
                        }
                        //line_idx = -1;
                        line_buf->len = 0;
                        line_buf->flush = false;
                    }

                } else {
                    len++;
                    if (ucode == '(') {
                        fprintf(ctx->out, "\\(");
                    } else if (ucode == ')') {
                        fprintf(ctx->out, "\\)");
                    } else {
                        fprintf(ctx->out, "%c", (char) ucode);
                    }
                }
                //>>
               
            }
            //>>
            
            break;
        }

        new_line = break_line;
        new_page = false;
        if (break_line) {

            // FLUSH-1
            if (use_unicode) {
                for (int k = 0; k < line_buf->len; k++) {
                                len += 2; // <xx>
                                int idx = line_buf->buf[k].idx;
                                fprintf(ctx->out, "%02X", idx);
                }

            } else {
                for (int k = 0; k < line_buf->len; k++) {
                            len++;
                            ucode = line_buf->buf[k].ucode;
                            if (ucode == '(') {
                                fprintf(ctx->out, "\\(");
                            } else if (ucode == ')') {
                                fprintf(ctx->out, "\\)");
                            } else {
                                fprintf(ctx->out, "%c", (char) ucode);
                            }
                }
            }

            line_buf->width = 0;
            //line_idx   = -1;
            line_buf->len   = 0;

            //fprintf(stderr, "[BR]: %d\n", i);
            if (line > line_page) {
                //if (page + 1 > max_page) {
                //    break;
                //}

                // CLOSE CUR PAGE

                // >>> Stream: end
                len += fprintf(ctx->out, use_unicode ? "> Tj\n" : ") Tj\n");
                len += fprintf(ctx->out, "%s", BUF_ET);
                len += fprintf(ctx->out, "endstream endobj\n");
                offset  += len;
                xrefs[ref] = offset;
                // >>>

                // OPEN NEW PAGE
                new_page = true;
                page++;
                line = 1;
                len  = 0;

                // >>> Stream: start
                ref++; // 4-Stream<N>
                len += fprintf(ctx->out, "%d 0 obj << /Length %d >> stream\n", ref, pages[page - 1]);
                len += fprintf(ctx->out, "%s", BUF_BT);
                len += fprintf(ctx->out, "%s", BUF_HD);
                len += fprintf(ctx->out, use_unicode ? "<" : "(");
                offset  += len;
                // >>>

            } else {
                line++;
            }
            
            len += fprintf(ctx->out, use_unicode ? "> Tj\n" : ") Tj\n");                        // BUF_LN_END
            len += fprintf(ctx->out, use_unicode ? "0 -%d Td\n<" : "0 -%d Td\n(", line_offset); // BUF_LN_BEGIN

            //len += fprintf(ctx->out, "0 -18 Td\n(");
            //len += fprintf(ctx->out, "(");


            //>>
            // FLUSH-2
            if (line_buf->len2 > 0) {
                _line_flush_2(line_buf);
            }
            //>>

        }

        data += seq;
        i += seq;

    }

    //>>
    // FLUSH-1

            if (use_unicode) {
                for (int k = 0; k < line_buf->len; k++) {
                                len += 2; // <xx>
                                int idx = line_buf->buf[k].idx;
                                fprintf(ctx->out, "%02X", idx);
                }

            } else {
                for (int k = 0; k < line_buf->len; k++) {
                            len++;
                            ucode = line_buf->buf[k].ucode;
                            if (ucode == '(') {
                                fprintf(ctx->out, "\\(");
                            } else if (ucode == ')') {
                                fprintf(ctx->out, "\\)");
                            } else {
                                fprintf(ctx->out, "%c", (char) ucode);
                            }
                }
            }

            line_buf->width = 0;
            //line_idx   = -1;
            line_buf->len   = 0;
    //>>


    // >>> Stream: end
    len += fprintf(ctx->out, use_unicode ? "> Tj\n" : ") Tj\n");
    len += fprintf(ctx->out, "%s", BUF_ET);
    len += fprintf(ctx->out, "endstream endobj\n");
    offset  += len;
    xrefs[ref] = offset;
    // >>>

    /*
      q 0 0 0 rg
      BT
      56 724 Td /F1 12 Tf[<01020304>]TJ
      ET
    */

    /*
      /Type
      /Font
      ...
      /FirstChar 0
      /LastChar 61
      /ToUnicode 20 0 R
      /Widths [
        0
        610
        443
        277
        333
        389
        250
        277
        500
        ...
      ]
      ...
      /FontDescriptor 19 0 R

    */

    /*
      20 0 obj
      <<
        /Length 21 0 R
      >>
      stream
      /CIDInit/ProcSet findresource begin
      12 dict begin
      begincmap
      /CIDSystemInfo<<
      /Registry (Adobe)
      /Ordering (UCS)
      /Supplement 0
      >> def
      /CMapName/Adobe-Identity-UCS def
      /CMapType 2 def
      1 begincodespacerange
      <00> <FF>
      endcodespacerange
      61 beginbfchar
      <01> <004C>
      <02> <0065>
      <03> <0074>
      <04> <2019>
      <05> <0073>
      <06> <0020>
      <07> <006A>
      <08> <0075>
      ...
      endbfchar
      endcmap
      CMapName currentdict /CMap defineresource pop
      end
      end
      endstream
      endobj
    */

    // Type: Font
    ref++; // 5-Font
    len = 0;
    //len += fprintf(ctx->out, "%d 0 obj << /Type /Font /Subtype /%s /BaseFont /%s /Encoding /%s", ref, font_subtype, font_name, pdf_encoding);
    //len += fprintf(ctx->out, "%d 0 obj << /Type /Font /Subtype /%s /BaseFont /%s ", ref, font_subtype, font_name);
    len += fprintf(ctx->out, "%d 0 obj << /Type /Font /BaseFont /%s", ref, font_name);

    // ToUnicode
    if (use_unicode) {
        len += fprintf(ctx->out, " /FirstChar 0");
        len += fprintf(ctx->out, " /LastChar %d", (cmap_size - 1));
        len += fprintf(ctx->out, " /ToUnicode %d 0 R", (ref + 1));
        //len += fprintf(ctx->out, " /Widths [0 700 700 700 700]");

        //>>>
        len += fprintf(ctx->out, " /Widths [");
        lib_pdf_cmap_t e;
        for (int i = 0; i < cmap_size; i++) {
            e = cmap[i];
            len += fprintf(ctx->out, " %d", e.width);
        }
        len += fprintf(ctx->out, "]");
        //>>>

    } else {
        len += fprintf(ctx->out, " /Subtype /%s", font_subtype);
        len += fprintf(ctx->out, " /Encoding /%s", pdf_encoding);
    }

    len += fprintf(ctx->out, " >> endobj\n");
    offset  += len;
    xrefs[ref] = offset;

    // Type: CMap
    if (use_unicode) {
        ref++; // 6-CMap
        len = 0;
        int cmap_len = 0;
        len += fprintf(ctx->out, "%d 0 obj << /Length %d 0 R >> stream\n", ref, (ref + 1));

        len += fprintf(ctx->out, "/CIDInit/ProcSet findresource begin\n");
        len += fprintf(ctx->out, "12 dict begin\n");
        len += fprintf(ctx->out, "begincmap\n");
        len += fprintf(ctx->out, "/CIDSystemInfo<<\n");
        len += fprintf(ctx->out, "/Registry (Adobe)\n");
        len += fprintf(ctx->out, "/Ordering (UCS)\n");
        len += fprintf(ctx->out, "/Supplement 0\n");
        len += fprintf(ctx->out, ">> def\n");
        len += fprintf(ctx->out, "/CMapName /Adobe-Identity-UCS def\n");
        len += fprintf(ctx->out, "/CMapType 2 def\n");
        len += fprintf(ctx->out, "1 begincodespacerange\n");
        len += fprintf(ctx->out, "<00> <FF>\n");
        len += fprintf(ctx->out, "endcodespacerange\n");

        //>>>
        len += fprintf(ctx->out, "%d beginbfchar\n", cmap_size);

        lib_pdf_cmap_t e;
        for (int i = cmap_start; i < cmap_size; i++) {
            e = cmap[i];
            fprintf(ctx->out, "<%02X> <%04X>\n", e.idx, e.ucode);
        }
        //>>>

        len += fprintf(ctx->out, "endbfchar\n");
        len += fprintf(ctx->out, "endcmap\n");
        len += fprintf(ctx->out, "CMapName currentdict /CMap defineresource pop\n");
        len += fprintf(ctx->out, "end\n");
        len += fprintf(ctx->out, "end\n");
        cmap_len = len;

        len += fprintf(ctx->out, "endstream\n");
        len += fprintf(ctx->out, "endobj\n");
        offset  += len;
        xrefs[ref] = offset;

        ref++; // 7-CMap: Length
        len = 0;
        len += fprintf(ctx->out, "%d 0 obj %d endobj\n", ref, cmap_len);
        offset  += len;
        xrefs[ref] = offset;
    }

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

// PDF Blog
// https://github.com/jberkenbilt/pdf-text-blog/tree/a6342d96a953d222a6bcd0bd0408615d66e6646b

// PDF Succinctly
// https://www.syncfusion.com/succinctly-free-ebooks/pdf/conceptual-overview
// https://www.syncfusion.com/succinctly-free-ebooks/pdf/text-operators

// Multi line
//
// 4 0 obj<</Length 84>>stream
// BT 150 700 Td /F0 32 Tf 30 TL(FIRST LINE)' T*(Second line)' (Third line)' ET
// endstream endobj
//
// https://stackoverflow.com/questions/17279712/what-is-the-smallest-possible-valid-pdf

// PDF
// https://habr.com/ru/articles/69568/

// RTF
// https://habr.com/ru/articles/70119/

// DOC, WCBFF
// https://habr.com/ru/articles/72745/

// DOCX, ODT
// https://habr.com/ru/articles/69417/
