#include "run_doclib.h"

#define LIB_HTML_CHARSET   "utf-8"
#define LIB_HTML_UNIT      "px"
#define LIB_HTML_MAARGIN   "5px"
#define LIB_HTML_FONT_NAME "Helvetica"
#define LIB_HTML_FONT_WEIGHT NULL
#define LIB_HTML_FONT_SIZE "12px"

typedef struct lib_html_config_t {
    const char* title;
    const char* margin;
    const char* font_name;
    const char* font_weight;
    const char* font_size;
} lib_html_config_t;

int lib_html_init(lib_html_config_t* config) {
    if (!config) {
        return 1;
    }
    config->title       = NULL;
    config->margin      = NULL;
    config->font_name   = NULL;
    config->font_weight = NULL;
    config->font_size   = NULL;
    return 0;
}
