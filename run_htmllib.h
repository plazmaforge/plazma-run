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

typedef struct lib_html_config_t {
    const char* charset;
    const char* title;
    const char* margin;
    const char* font_name;
    const char* font_style;
    const char* font_weight;
    const char* font_size;
} lib_html_config_t;

int lib_html_config_init(lib_html_config_t* config) {
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

static const char* lib_html_unitdef(const char* value) {
    return lib_is_digit(value) ? LIB_HTML_UNIT : "";
}

int run_html(lib_html_config_t* config, char* data, size_t size) {
    
    if (size == 0 || !data) {
        fprintf(stderr, "%s: No input data\n", prog_name);
        return 1;
    }

    const char* charset     = config->charset;
    const char* title       = config->title;
    const char* margin      = config->margin;
    const char* margin_unit = lib_html_unitdef(margin);    
    const char* font_name   = config->font_name;
    const char* font_style  = config->font_style;
    const char* font_weight = config->font_weight;
    const char* font_size   = config->font_size;
    const char* font_unit   = lib_html_unitdef(font_size);
    
    bool use_charset        = charset;
    bool use_title          = title;
    bool use_head           = use_charset || use_title;

    bool use_margin         = margin;
    bool use_font_name      = font_name;
    bool use_font_style     = font_style;
    bool use_font_weight    = font_weight;
    bool use_font_size      = font_size;
    bool use_style          = use_margin || use_font_name || use_font_weight || use_font_size;

    // DOCUMENT
    fprintf(stdout, "<!DOCTYPE html>\n");
    fprintf(stdout, "<html>\n");

    // HEAD
    if (use_head) {
        fprintf(stdout, "  <head>\n");

        if (use_charset) {
            fprintf(stdout, "  <meta charset=\"%s\">\n", charset);
        }
        if (use_title) {
            fprintf(stdout, "  <title>%s</title>\n", title);
        }

        fprintf(stdout, "  </head>\n");
    }

    // BODY
    if (use_style) {
        fprintf(stdout, "  <body style=\"");
        if (use_margin) {
            fprintf(stdout, "margin: %s%s;", margin, margin_unit);
        }
        if (use_font_name) {
            fprintf(stdout, "font-family: %s;", font_name);
        }
        if (use_font_style) {
            fprintf(stdout, "font-style: %s;", font_style);
        }
        if (use_font_weight) {
            fprintf(stdout, "font-weight: %s;", font_weight);
        }
        if (use_font_size) {
            fprintf(stdout, "font-size: %s%s;", font_size, font_unit);
        }
        fprintf(stdout, "\">\n");
    } else {
        fprintf(stdout, "  <body>\n");
    }    

    fprintf(stdout, "%s\n", data);
    fprintf(stdout, "  </body>\n");
    fprintf(stdout, "</html>\n");

    return 0;
}

