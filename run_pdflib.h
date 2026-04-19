#include <stdlib.h>

#include <string.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>       // getuid (user_name, user_home), getcwd (current_dir), macos:confstr (tmp_dir)
#include <sys/utsname.h>  // os_name, os_version
#include <pwd.h>          // getpwuid (user_name, user_home)
#include <limits.h>       // PATH_MAX
#endif

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

static const int helvetica_ascii[] = {
0, 750, 750, 750, 750, 750, 750, 750, 750, 0, 0, 750, 861, 0, 750, 750,
750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750,
277, 278, 355, 555, 555, 889, 666, 190, 333, 333, 389, 584, 277, 333, 277, 277,
555, 555, 555, 555, 555, 555, 555, 555, 555, 555, 277, 277, 584, 584, 584, 555,
1015, 666, 666, 722, 722, 666, 610, 777, 722, 277, 500, 666, 555, 833, 722, 777,
666, 777, 722, 666, 610, 722, 666, 943, 667, 667, 610, 277, 277, 277, 470, 555,
333, 555, 555, 500, 555, 555, 277, 555, 555, 222, 222, 500, 222, 834, 555, 555,
555, 555, 333, 500, 277, 555, 499, 720, 498, 500, 500, 334, 260, 334, 584, 500,

750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750,
750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750,
277, 333, 555, 555, 555, 555, 260, 555, 333, 736, 370, 555, 584, 333, 736, 552,
400, 549, 333, 333, 333, 575, 537, 333, 333, 333, 365, 555, 834, 834, 834, 610,
666, 666, 666, 666, 666, 666, 1000, 722, 666, 666, 666, 666, 277, 277, 277, 277,
722, 722, 777, 777, 777, 777, 777, 584, 777, 722, 722, 722, 722, 667, 666, 610,
555, 555, 555, 555, 555, 555, 889, 500, 555, 555, 555, 555, 276, 276, 276, 276,
555, 555, 555, 555, 555, 555, 555, 549, 610, 555, 555, 555, 555, 500, 555, 500
};

static const int helvetica_cp1251[] = {
0, 750, 750, 750, 750, 750, 750, 750, 750, 0, 0, 750, 861, 0, 750, 750,
750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750, 750,
277, 278, 355, 555, 555, 889, 666, 190, 333, 333, 389, 584, 277, 333, 277, 277,
555, 555, 555, 555, 555, 555, 555, 555, 555, 555, 277, 277, 584, 584, 584, 555,
1015, 666, 666, 722, 722, 666, 610, 777, 722, 277, 500, 666, 555, 833, 722, 777,
666, 777, 722, 666, 610, 722, 666, 943, 667, 667, 610, 277, 277, 277, 470, 555,
333, 555, 555, 500, 555, 555, 277, 555, 555, 222, 222, 500, 222, 834, 555, 555,
555, 555, 333, 500, 277, 555, 499, 720, 498, 500, 500, 334, 260, 334, 584, 500,

865, 541, 222, 365, 333, 1000, 555, 555, 555, 1000, 1057, 333, 1010, 582, 854, 719,
555, 222, 222, 333, 333, 350, 555, 1000, 750, 1000, 906, 333, 812, 437, 555, 552,
277, 635, 500, 500, 555, 489, 260, 555, 666, 736, 719, 555, 584, 333, 736, 277,
400, 549, 277, 222, 410, 575, 537, 333, 555, 1072, 510, 555, 222, 666, 500, 276,
666, 656, 666, 541, 677, 666, 923, 604, 719, 719, 582, 656, 833, 722, 777, 719,
666, 722, 610, 635, 760, 667, 740, 666, 916, 937, 791, 885, 656, 719, 1010, 722,
555, 572, 531, 365, 583, 555, 669, 458, 558, 558, 437, 583, 687, 552, 555, 541,
555, 500, 458, 500, 822, 498, 572, 520, 802, 822, 625, 719, 520, 510, 750, 541
};

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
    bool use_internal;
    const int* font_widths;
} run_pdf_context_t;

typedef struct lib_font_info_t {
    const char* family;
    const char* name;
    bool bold;
    bool italic;
} lib_font_info_t;

typedef struct lib_pdf_char_t {
    uint8_t  icode; // inp code
    uint32_t ucode; // uni code
    uint16_t idx;   // index
    int width;      // char width
    bool is_predef;
} lib_pdf_char_t;

typedef struct lib_pdf_cmap_t {
    lib_pdf_char_t* buf;
    int size;        // CMap size
    int capacity;    // CMap capacity
    int idx;         // CMap index
    int carr_idx;    // CMap array index
    int gap;         // CMap gap index
    int start;       // CMap start index
    bool use_predef; // Use predefined mode
} lib_pdf_cmap_t;

typedef struct lib_pdf_line_t {
    lib_pdf_char_t* buf;
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

static int lib_to_pt(const char* value);

static int _line_init(lib_pdf_line_t* line);

static int _cmap_init(lib_pdf_cmap_t* cmap);

static int _cmap_predef(run_pdf_context_t* ctx, lib_pdf_cmap_t* cmap);

static void _cmap_dump(lib_pdf_cmap_t* cmap);

static int _cmap_get_width(run_pdf_context_t* ctx, lib_pdf_cmap_t* cmap, lib_pdf_char_t* c);

////

static int lib_pdf_document(run_pdf_context_t* ctx);

static int lib_pdf_head(run_pdf_context_t* ctx);

static int lib_pdf_margin(run_pdf_context_t* ctx);

static int lib_pdf_font(run_pdf_context_t* ctx);

int lib_pdf_body(run_pdf_context_t* ctx);


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

    ctx->use_internal  = false;
    ctx->font_widths   = NULL;

    return lib_pdf_prepare(ctx);
}

static int lib_pdf_document(run_pdf_context_t* ctx) {
    // DOCUMENT
    // HEAD
    if (ctx->use_head) {
        lib_pdf_head(ctx);
    }

    lib_pdf_body(ctx);
    return 0;
}

static int lib_pdf_head(run_pdf_context_t* ctx) {
    fprintf(stderr, ">> lib_pdf_head\n");

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


static int _cmap_init(lib_pdf_cmap_t* cmap) {
    if (!cmap) {
        return 1;
    }
    cmap->size       = 0;     // CMap size
    cmap->capacity   = 0;     // CMap capacity
    cmap->idx        = 0;     // CMap index
    cmap->carr_idx   = 0;     // CMap array index
    cmap->gap        = -1;    // CMap gap index
    cmap->start      = 0;     // CMap start index
    cmap->use_predef = false; // Use predefined mode
    return 0;
}

static lib_pdf_cmap_t* _cmap_new(size_t capacity) {
   lib_pdf_cmap_t* cmap = (lib_pdf_cmap_t*) malloc(sizeof(lib_pdf_cmap_t));
   if (cmap == NULL) {
      return NULL;
   }
   _cmap_init(cmap);
   cmap->buf = (lib_pdf_char_t*) malloc(capacity * sizeof(lib_pdf_char_t));
   cmap->capacity = capacity;
   return cmap;
}

/**
 * Create predefined CMap [0..255]
 * By default use ASCII code page.
 * Bu we can use other code pages.
 * For example: CP1251.
 */
static int _cmap_predef(run_pdf_context_t* ctx, lib_pdf_cmap_t* cmap) {
    
    int size = 256;
    lib_pdf_char_t e;
    int cmap_idx  = 0;

    for (int i = 0; i < size; i++) {

        cmap_idx = i;

        e.idx   = cmap_idx;
        e.icode = cmap_idx;
        e.ucode = cmap_idx;
        e.width = _cmap_get_width(ctx, cmap, &e);

        bool block_a =  (cmap_idx >= 32 && cmap_idx <= 127);  // [032..127] ASCII
        bool block_b =  (cmap_idx >= 160 && cmap_idx <= 255); // [160..255] NLS

        e.is_predef = block_a || block_b;

        cmap->buf[i] = e;
    }
    return size;
}

/**
 * Find icode in CMap
 */
static lib_pdf_char_t* _cmap_find_by_icode(lib_pdf_cmap_t* cmap, int icode) {
    lib_pdf_char_t* p = cmap->buf;
    for (int i = 0; i < cmap->size; i++) {
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
static lib_pdf_char_t* _cmap_find_by_ucode(lib_pdf_cmap_t* cmap, int ucode) {
    lib_pdf_char_t* p = cmap->buf;
    for (int i = 0; i < cmap->size; i++) {
        if (ucode == p->ucode) {
            return p;
        }
        p++;
    }
    return NULL;
}

static bool _cmap_has_icode(lib_pdf_cmap_t* cmap, int icode) {
    return _cmap_find_by_icode(cmap, icode) != NULL;
}

static bool _cmap_has_ucode(lib_pdf_cmap_t* cmap, int ucode) {
    return _cmap_find_by_ucode(cmap, ucode) != NULL;
}

/**
 * Find and return CMap index of first not predefined element
 */
static int _cmap_find_gap(lib_pdf_cmap_t* cmap) {
    lib_pdf_char_t* p = cmap->buf;
    int start = cmap->gap + 1;
    p += start;
    for (int i = start; i < cmap->size; i++) {
        if (!(p->is_predef)) {
            return i;
        }
        p++;
    }
    return -1;
}

static int _cmap_def_width() {
    return 600;
}

static int _cmap_get_width(run_pdf_context_t* ctx, lib_pdf_cmap_t* cmap, lib_pdf_char_t* c) {
    
    //fprintf(stderr, "_get_width: interanl=%d, icode=%d, ucode=%d \n", ctx->use_internal, c->icode, c->ucode);

    const int* font_widths = ctx->font_widths;
    
    if (font_widths == NULL) {
        //fprintf(stderr, "def-width\n");
        return _cmap_def_width();
    }

    if (ctx->use_internal) {
        for (int i = 0; i < 256; i++) {
            //if (i == c->ucode) {
            //    return font_widths[i];
            //}
            if (i == c->icode) {
                //fprintf(stderr, " i-width: %d\n", font_widths[i]);
                return font_widths[i];
            }
        }
    } else {
        for (int i = 0; i < 65536; i++) {
            if (i == c->ucode) {
                //fprintf(stderr, " u-width: %d\n", font_widths[i]);
                return font_widths[i];
            }
            //if (i == c->icode) {
            //    return font_widths[i];
            //}
        }
    }

    return _cmap_def_width();
}

/**
 * Add a char to CMap: icode, ucode.
 * We use 2 Predefined modes:
 * - Predefined = true : find gap
 * - Predefined = false: add new index 
 */
static lib_pdf_char_t* _cmap_add_char(run_pdf_context_t* ctx, lib_pdf_cmap_t* cmap, uint8_t icode, uint32_t ucode) {

    //fprintf(stderr, ">> _cmap_add_char: ucode=%d\n", ucode);

    //if (ucode == NO_CHR) {
    //    // Not found char in UniMap
    //    fprintf(stderr, ">> Not found code in UniMap [NO_CHR]: %d\n", icode);
    //}

    bool success = false;
    if (cmap->use_predef) {
        cmap->idx = _cmap_find_gap(cmap);

        //if (debug) {
        //    fprintf(stderr, ">> _cmap_find_gap: cmap_size=%d, cmap_gap=%d, cmap_idx=%d\n", cmap->size, cmap->gap, cmap->idx);
        //}

        if (cmap->idx < 0) {
            fprintf(stderr, ">> Not found gap in CMap: %d, %d\n", cmap->size, cmap->gap);
        } else {
            success = true;
            cmap->gap = cmap->idx; 
            cmap->carr_idx = cmap->idx;
        }
        //fprintf(stderr, ">> _cmap_add_char: Found gap\n");
    } else {
        
        if (cmap->size + 1 > cmap->capacity) {
            //fprintf(stderr, ">> _cmap_add_char: grow: size=%d, capacity=%d\n", cmap->size, cmap->capacity);
            cmap->capacity = cmap->capacity * 2;
            //fprintf(stderr, ">> _cmap_add_char: realloc starting...\n");

            // TODO: Check realloc result and trow error if it is NULL
            cmap->buf = (lib_pdf_char_t*) realloc(cmap->buf, cmap->capacity);

            //fprintf(stderr, ">> _cmap_add_char: realloc success\n");
        }
        success = true;
        cmap->carr_idx = cmap->size;
        cmap->size++;
        cmap->idx++;
    }

    if (!success) {
        return NULL;
    }

    lib_pdf_char_t* p = &(cmap->buf[cmap->carr_idx]);
    p->icode = icode;
    p->ucode = ucode;
    p->width = _cmap_get_width(ctx, cmap, p);
    p->idx   = cmap->idx;
    p->is_predef = false;

    return p;
}

/**
 * Dump CMap in DEBUG mode
 */
static void _cmap_dump(lib_pdf_cmap_t* cmap) {
    lib_pdf_char_t* p = cmap->buf;
    for (int i = 0; i < cmap->size; i++) {
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

static int _line_init(lib_pdf_line_t* line) {
    if (!line) {
        return 1;
    }
    line->width = 0;
    line->len   = 0;
    line->pos2  = -1;
    line->len2  = 0;
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


static int _print_idx(run_pdf_context_t* ctx, int idx, bool out_mode) {
    if (out_mode) {
        return fprintf(ctx->out, "%02X", idx);
    } else {
        return 2; // <xx>
    }
}

static int _print_ucode(run_pdf_context_t* ctx, uint32_t ucode, bool out_mode) {
    if (out_mode) {
        if (ucode == '(') {
            return fprintf(ctx->out, "\\(");
        } else if (ucode == ')') {
            return fprintf(ctx->out, "\\)");
        } else {
            return fprintf(ctx->out, "%c", (char) ucode);
        }
    } else {
        if (ucode == '(') {
            return 2;
        } else if (ucode == ')') {
            return 2;
        } else {
            return 1;
        }
    }
}

static int _line_add(lib_pdf_line_t* line, lib_pdf_char_t* p, int font_size) {
    if (!line) {
        return 1;
    }

    // Calculate real width of char for font size
    int width = p->width * font_size; // width in 1/1000 pt x font size

    line->buf[line->len].ucode = p->ucode;
    line->buf[line->len].idx   = p->idx;
    line->buf[line->len].width = width;
    line->len++;
    line->width += width;

    return 0;
}

/**
 * Flush line to output
 */
static int _line_flush(run_pdf_context_t* ctx, lib_pdf_line_t* line, bool out_mode) {
    if (!line) {
        return 0;
    }
    int size = line->len;
    int len = 0;
    if (ctx->use_unicode) {
        for (int k = 0; k < size; k++) {
            int idx = line->buf[k].idx;
            len += _print_idx(ctx, idx, out_mode);
        }
    } else {
        for (int k = 0; k < size; k++) {
            uint32_t ucode = line->buf[k].ucode;
            len += _print_ucode(ctx, ucode, out_mode);
        }
    }

    line->width = 0;
    line->len = 0;

    return len;
}

// SHIFT
static int _line_shift(lib_pdf_line_t* line) {
    if (!line) {
        return 1;
    }
    if (line->len2 <= 0 || line->pos2 < 0) {
        return 0;
    }

    // reset line width because new chars after shift
    line->width = 0;

    int shift = line->pos2 == 0 ? 1 : line->pos2; 
    for (int k = 0; k < line->len2; k++) {
        line->buf[k] = line->buf[k + shift];
        line->width += line->buf[k].width; 
    }
    line->len  = line->len2;
    line->pos2 = -1;
    line->len2 = 0;

    return 0;
}

static int _line_break(lib_pdf_line_t* line) {
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

    return 0;
}

////////////////////////////////////////////////////////////////////////////

// OS PATH_MAX
// https://habr.com/ru/companies/pvs-studio/articles/684636/

#ifdef WIN32

// wstrlib

static char* _wcstombs_win(UINT cp, const wchar_t* wstr, int wlen) {
    if (!wstr) {
        return NULL;
    }
    int len = WideCharToMultiByte(cp, 0, wstr, wlen, NULL, 0, NULL, NULL);
    char* str = malloc(len + 1); //lib_strnew(len);
    str[0] = '\0';
    WideCharToMultiByte(cp, 0, wstr, -1, str, len, NULL, NULL);
    str[len] = '\0';
    return str;
}

static char* lib_wcstombs(const wchar_t* wstr) {
    if (!wstr) {
        return NULL;
    }
    return _wcstombs_win(CP_UTF8, wstr, wcslen(wstr)); 
}

// dirlib

static wchar_t* getCurDirW() {
    /* Current directory */
    WCHAR buf[MAX_PATH];
    if (GetCurrentDirectoryW(sizeof(buf) / sizeof(WCHAR), buf) == 0) {
        return NULL;
    }
    return _wcsdup(buf);
}

static char* lib_get_cwd() {
    wchar_t* wdir = getCurDirW();
    if (wdir == NULL) {
        return NULL;
    }
    char* dir = lib_wcstombs(wdir);
    free(wdir);
    if (dir == NULL) {
        return NULL;
    }
    //free(wdir);
    return dir;
}

#else

static char* lib_get_cwd() {
  /* Current directory */
  char buf[PATH_MAX];
  //errno = 0;
  if (getcwd(buf, sizeof(buf)) == NULL) {
      return NULL;
  }
  return strdup(buf);
}

#endif

////////////////////////////////////////////////////////////////////////////


/**
 * Return file name by font name
 */
static char* _font_widths_file_name(const char* font_name) {

    //fprintf(stderr, ">> _font_widths_file_name\n");

    if (!font_name) {
        return NULL;
    }

    char* cwd = lib_get_cwd(); // [allocate]
    if (!cwd) {
        return NULL;
    }

    char* dir = "/res/fonts/";
    char* ext = ".width";

    int len = strlen(cwd);
    len += strlen(dir);
    len += strlen(font_name);
    len += strlen(ext);

    char* str = (char*) malloc(len + 1);
    strcpy(str, cwd);
    strcat(str, dir);
    strcat(str, font_name);
    strcat(str, ext);

    free(cwd);

    str[len] = '\0';

    #ifdef _WIN32
    for (int i = 0; i < len; i++) {
       if (str[i] == '/') {
            str[i] = '\\';
        }
    } 
    #endif

    //fprintf(stderr, ">> _font_widths_file_name: %s\n", str);

    return str;
}

static bool _is_space(char* str) {
    return (*str == ' ' || *str == '\t');
}

static bool _is_skip(char* str) {
    return (*str == '\0' || *str == '#');
}

static bool _is_shift(char* str) {
    return (!_is_space(str) && !_is_skip(str));
}

/**
 * Read line from font widths file
 */
static int _font_widths_read_line(int* width, int* count, char* line) {
    *width = 0;
    *count = 0;

    //fprintf(stderr, ">> read_line: %s\n", line);

    if (line == NULL) {
        return 1;
    }

    // skip
    if (_is_skip(line)) {
        //fprintf(stderr, ">> read_line: return-skip-1\n");
        return 0;
    }

    char* str = line;
    char* val = str;
    int len   = 0;
    char col[100];

    // trim left
    while (_is_space(str)) {
        str++;
    }

    // skip
    if (_is_skip(str)) {
        //fprintf(stderr, ">> read_line: return-skip-2\n");
        return 0;
    }

    val = str;

    // shift to separators or end string
    while (_is_shift(str)) {
        str++;
        len++;
    }

    strncpy(col, val, len);
    *width = atoi(col);
    *count = 1;

    // end
    if (_is_skip(str)) {
        //fprintf(stderr, ">> read_line: return-skip-3\n");
        return 0;
    }

    // trim left
    while (_is_space(str)) {
        str++;
    }

    // end
    if (_is_skip(str)) {
        //fprintf(stderr, ">> read_line: return-skip-4\n");
        return 0;
    }

    val = str;
    len = 0;

    // shift to separators or end string
    while (_is_shift(str)) {
        str++;
        len++;
    }

    strncpy(col, val, len);
    *count = atoi(col);

    return 0;
}

#ifdef _WIN32
#define GETLINE_MINSIZE 16
int _getline_win(char** line, size_t* n, FILE* file) {
    int ch;
    int i = 0;
    char free_on_err = 0;
    char *p;

    errno = 0;
    if (line == NULL || n == NULL || file == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (*line == NULL) {
        *n = GETLINE_MINSIZE;
        *line = (char*) malloc( sizeof(char) * (*n));
        if (*line == NULL) {
            errno = ENOMEM;
            return -1;
        }
        free_on_err = 1;
    }

    for (i = 0; ; i++) {
        ch = fgetc(file);
        while (i >= (*n) - 2) {
            *n *= 2;
            p = realloc(*line, sizeof(char) * (*n));
            if (p == NULL) {
                if (free_on_err)
                    free(*line);
                errno = ENOMEM;
                return -1;
            }
            *line = p;
        }
        if (ch == EOF) {
            if (i == 0) {
                if (free_on_err)
                     free(*line);
                return -1;
            }
            (*line)[i] = '\0';
            *n = i;
            return i;
        }

        if (ch == '\n') {
            (*line)[i] = '\n';
            (*line)[i+1] = '\0';
            *n = i+1;
            return i+1;
        }
        (*line)[i] = (char)ch;
    }
}
#else
ssize_t _getline_nix(char** line, size_t* cap, FILE* file) {
    return getline(line, cap, file);
}
#endif

ssize_t _getline(char** line, size_t* cap, FILE* file) {
    #ifdef _WIN32
    return _getline_win(line, cap, file);
    #else
    return _getline_nix(line, cap, file);
    #endif
}

static int _font_widths_init_ext(run_pdf_context_t* ctx, const char* font_name) {

    if (!font_name) {
        fprintf(stderr, "Font name is empty\n");
        return 1;
    }

    char* file_name = _font_widths_file_name(font_name);
    if (!file_name) {
        fprintf(stderr, "Font file name is empty\n");
        return 1;
    }

    FILE* file = fopen(file_name, "rb");
    if (!file) {
        fprintf(stderr, "Font file not found: %s\n", file_name);
        free(file_name);
        return 1;
    }

    int* font_widths = (int*) malloc(65536 * sizeof(int));
    if (font_widths == NULL) {
        free(file_name);
        return 1;
    }

    int i = 0;
    int width = 0;
    int count = 0;

    char* line   = NULL;
    size_t cap   = 0;
    size_t len   = 0;

    while ( (len = _getline(&line, &cap, file)) != -1 ) {
        if (_font_widths_read_line(&width, &count, line) != 0) {
            fprintf(stderr, "Read font line error\n");
            fclose(file);
            free(file_name);
            return 1;
        }

        if (count > 0) {
            for (int k = 0; k < count; k++) {
                font_widths[i + k] = width;
            }
            i += count;
        }
    }

    free(line);

    // use_internal = false
    ctx->font_widths = font_widths;

    fclose(file);
    free(file_name);
    return 0;
}

static int _font_widths_init_int(run_pdf_context_t* ctx, const char* font_name) {
    
    // Helvetica
    if (lib_stristr(font_name, "Helvetica") == font_name) {
        //fprintf(stderr, ">> Helvetica\n");
        if (ctx->encoding_id == LIB_ENC_CP1251_ID) {
            ctx->font_widths = helvetica_cp1251;
            return 0;
        } else if (ctx->encoding_id == 0) {
            ctx->font_widths = helvetica_ascii;
            return 0;
        }
    }

    return 1;
}

static int lib_pdf_init_font_widths(run_pdf_context_t* ctx, const char* font_name) {
    ctx->font_widths = NULL;

    if (font_name == NULL) {
        return 1;
    }

    //fprintf(stderr,">> lib_pdf_init_font_widths\n");

    int err = _font_widths_init_ext(ctx, font_name);
    if (err == 0) {
        ctx->use_internal = false;
    } else {
        ctx->use_internal = true;
        _font_widths_init_int(ctx, font_name);
    }

    return 0;
} 

int lib_pdf_body(run_pdf_context_t* ctx) {
    bool debug = false;
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

    lib_pdf_init_font_widths(ctx, font_name);

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
    bool break_line_force;
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

    lib_pdf_cmap_t* cmap = NULL;
    int cmap_digits     = 2; // CMap output digits in index: <XX> or <XXXX>
    bool cmap_found     = false; // CMap found flag

    size_t i            = 0;
    int icode           = 0; // inp code
    int ucode           = 0; // uni code (code point)
    int next            = 0; // next uni code (code point)
    int seq             = 0; // sequence of char
    char* data = ctx->data;

    bool use_predef     = use_unicode;
    bool use_break_line = true;
    bool use_cmap       = use_unicode || use_break_line;

    int body_width = content_width * 1000; // width in 1/1000 pt

    if (debug) {
        fprintf(stderr, ">> use_predef     =%s\n", (use_predef ? "true" : "false"));
        fprintf(stderr, ">> use_break_line =%s\n", (use_break_line ? "true" : "false"));
        fprintf(stderr, ">> use_cmap       =%s\n", (use_cmap ? "true" : "false"));
        fprintf(stderr, ">> content_width  =%d\n", content_width);
        fprintf(stderr, ">> body_width     =%d\n", body_width);
    }

    if (use_cmap) {

        cmap = _cmap_new(256);
        cmap->use_predef = use_predef;
        
        if (use_predef) {

            // Initialize predefined map (ASCII)
            if (debug) {
                fprintf(stderr, ">> Initialize predefined map (ASCII)...\n");
            }
            
            cmap->start  = 0;
            cmap->size   = _cmap_predef(ctx, cmap);
        } else {
            lib_pdf_char_t e;
            cmap->start  = 1;
            cmap->size   = 1;
            e.icode     = 0;
            e.ucode     = 0;
            e.width     = 0;
            e.idx       = 0;
            e.is_predef = false;
            cmap->buf[0] = e;
        }
    }

    if (debug) {
        fprintf(stderr, "CMap dump-1\n");
        _cmap_dump(cmap);
    }


    lib_pdf_line_t line_buf_v;
    lib_pdf_char_t char_buf[100];
    line_buf_v.buf = char_buf;

    lib_pdf_line_t* line_buf = &line_buf_v;

    _line_init(line_buf);

    lib_pdf_char_t* p = NULL;
    bool success = false;
                
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
        break_line_force = false;

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

            success = false;
            p = NULL;

            if (use_cmap) {
                cmap_found = false;
                p = _cmap_find_by_ucode(cmap, ucode);
                cmap_found = p != NULL;
                success = cmap_found;

                if (debug) {
                    fprintf(stderr, ">> PRE: _cmap_find_by_ucode: ucode=%d [%s]\n", ucode, (cmap_found ? "+" : " "));
                }

                if (!cmap_found) {
                    if (ucode == NO_CHR) {
                        // Not found char in UniMap
                        fprintf(stderr, ">> Not found code in UniMap [NO_CHR]: %d\n", icode);
                    } else {
                        p = _cmap_add_char(ctx, cmap, icode, ucode);
                        success = p != NULL;
                    }
                }
            } else {
                success = true;
            }

            if (!success) {
                break;
            }

            //>>
            if (use_break_line) {

                _line_add(line_buf, p, font_size);
                line_buf->flush = false;

                // Break Line Algo
                if (line_buf->width >= body_width) {
                    _line_break(line_buf);
                    line_buf->flush = true;
                    break_line = true;
                    break_line_force = true;
                }

                if (line_buf->flush) {
                    // FLUSH
                    len += _line_flush(ctx, line_buf, false);
                    line_buf->flush = false;

                    // SHIFT
                    _line_shift(line_buf);
                }
                        
            } else {
                len += (use_unicode ?  _print_idx(ctx, p->idx, false) : _print_ucode(ctx, ucode, false));
            }
            //>>

            break;
        }

        new_line = break_line;
        new_page = false;
        if (break_line) {

            if (use_break_line && !break_line_force) {
                // FORCE FLUSH
                len += _line_flush(ctx, line_buf, false);
                line_buf->flush = false;

                // SHIFT
                _line_shift(line_buf);
            }

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

        }

        data += seq;
        i += seq;
    }

    //>>
    if (use_break_line) {
        // FORCE FLUSH
        len += _line_flush(ctx, line_buf, false);
        line_buf->flush = false;

        // SHIFT
        _line_shift(line_buf);
    }
    //>>

    if (debug) {
        fprintf(stderr, "\n");
        fprintf(stderr, "CMap dump-2\n");
        _cmap_dump(cmap);
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
        break_line_force = false;

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

            success = false;
            p = NULL;

            if (use_cmap) {
                cmap_found = false;
                p = _cmap_find_by_ucode(cmap, ucode);
                cmap_found = p != NULL;
                success = cmap_found;

                if (debug) {
                    fprintf(stderr, ">> OUT: _cmap_find_by_ucode: ucode=%d [%s]\n", ucode, (cmap_found ? "+" : " "));
                }

                if (!cmap_found) {
                    fprintf(stderr, "Char not found in CMap: icode=%d, ucode=%d\n", icode, ucode);
                }

            } else {
                success = true;
            }

            if (!success) {
                break;
            }

            //>>
            if (use_break_line) {

                _line_add(line_buf, p, font_size);
                line_buf->flush = false;

                // Break Line Algo
                if (line_buf->width  >= body_width) {
                    //>>
                    _line_break(line_buf);
                    line_buf->flush = true;
                    break_line = true;
                    break_line_force = true;

                }

                if (line_buf->flush) {
                    // FLUSH
                    len += _line_flush(ctx, line_buf, true);
                    line_buf->flush = false;

                    // SHIFT
                    _line_shift(line_buf);
                }

            } else {
                len += (use_unicode ? _print_idx(ctx, p->idx, true) : _print_ucode(ctx, ucode, true));
            }
            //>>

            break;
        }

        new_line = break_line;
        new_page = false;
        if (break_line) {
            
            if (use_break_line && !break_line_force) {
                // FORCE FLUSH
                len += _line_flush(ctx, line_buf, true);
                line_buf->flush = false;

                // SHIFT
                _line_shift(line_buf);
            }

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

        }

        data += seq;
        i += seq;

    }

    //>>
    if (use_break_line) {
        // FORCE FLUSH
        len += _line_flush(ctx, line_buf, true);
        line_buf->flush = false;

        // SHIFT
        _line_shift(line_buf);
    }
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
        len += fprintf(ctx->out, " /LastChar %d", (cmap->size - 1));
        len += fprintf(ctx->out, " /ToUnicode %d 0 R", (ref + 1));
        //len += fprintf(ctx->out, " /Widths [0 700 700 700 700]");

        //>>>
        len += fprintf(ctx->out, " /Widths [");
        lib_pdf_char_t e;
        for (int i = 0; i < cmap->size; i++) {
            e = cmap->buf[i];
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
        len += fprintf(ctx->out, "%d beginbfchar\n", cmap->size);

        lib_pdf_char_t e;
        for (int i = cmap->start; i < cmap->size; i++) {
            e = cmap->buf[i];
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
    //fprintf(stderr, ">> run_pdf\n");
    
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
