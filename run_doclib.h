#ifndef PLAZMA_LIB_DOCIB_H
#define PLAZMA_LIB_DOCIB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define LIB_DOC_ENCODING        "utf-8"
#define LIB_DOC_CHARSET         LIB_DOC_ENCODING
#define LIB_DOC_TITLE           NULL
#define LIB_DOC_UNIT            "px"
#define LIB_DOC_MARGIN          "5px"
#define LIB_DOC_FONT_NAME       "Helvetica"
#define LIB_DOC_FONT_STYLE      NULL
#define LIB_DOC_FONT_WEIGHT     NULL
#define LIB_DOC_FONT_SIZE       "12px"

#define LIB_OPT_ENCODING        "encoding"
#define LIB_OPT_CHARSET         "charset"
#define LIB_OPT_TITLE           "title"
#define LIB_OPT_MARGIN          "margin"
#define LIB_OPT_FONT_NAME       "font-name"
#define LIB_OPT_FONT_STYLE      "font-style"
#define LIB_OPT_FONT_WEIGHT     "font-weight"
#define LIB_OPT_FONT_SIZE       "font-size"
#define LIB_OPT_WIDTH           "width"
#define LIB_OPT_HEIGHT          "height"

#define LIB_OPT_ENCODING_ID     1000
#define LIB_OPT_CHARSET_ID      1001
#define LIB_OPT_TITLE_ID        2001
#define LIB_OPT_MARGIN_ID       3001
#define LIB_OPT_FONT_NAME_ID    4001
#define LIB_OPT_FONT_STYLE_ID   4002
#define LIB_OPT_FONT_WEIGHT_ID  4003
#define LIB_OPT_FONT_SIZE_ID    4004
#define LIB_OPT_WIDTH_ID        5001
#define LIB_OPT_HEIGHT_ID       5002

typedef struct lib_margin_t {
    const char* value;
    const char* unit;
} lib_margin_t;

typedef struct lib_font_t {
    const char* name;
    const char* style;
    const char* weight;
    const char* size;
    const char* unit;
} lib_font_t;

#define LIB_DOC_CONFIG_0      \
    const char* charset;      \
    const char* title;        \
    const char* margin;       \
    const char* margin_unit;  \
    const char* font_name;    \
    const char* font_style;   \
    const char* font_weight;  \
    const char* font_size;    \
    const char* font_unit;    \

#define LIB_DOC_CONTEXT_0     \
    const char* charset;      \
    const char* title;        \
    const char* margin;       \
    const char* margin_unit;  \
    const char* font_name;    \
    const char* font_style;   \
    const char* font_weight;  \
    const char* font_size;    \
    const char* font_unit;    \
    bool use_charset;         \
    bool use_title;           \
    bool use_head;            \
    bool use_margin;          \
    bool use_font_name;       \
    bool use_font_style;      \
    bool use_font_weight;     \
    bool use_font_size;       \
    bool use_font;            \
    bool use_style;           \
    char* data;               \
    size_t size;              \


#define LIB_DOC_CONFIG        \
    const char* charset;      \
    const char* title;        \
    const char* margin;       \
    const char* margin_unit;  \
    lib_font_t* font;         \

#define LIB_DOC_CONTEXT       \
    const char* charset;      \
    const char* title;        \
    const char* margin;       \
    const char* margin_unit;  \
    lib_font_t* font;         \
    bool use_charset;         \
    bool use_title;           \
    bool use_head;            \
    bool use_margin;          \
    bool use_font_name;       \
    bool use_font_style;      \
    bool use_font_weight;     \
    bool use_font_size;       \
    bool use_font;            \
    bool use_style;           \
    char* data;               \
    size_t size;              \

/**
 * DOC Config
 */
typedef struct lib_doc_config_t {
    LIB_DOC_CONFIG
} lib_doc_config_t;

/**
 * DOC Context
 */
typedef struct lib_doc_context_t {
    LIB_DOC_CONTEXT
} lib_doc_context_t;


static bool _isdigit(char c) {
    return c == '0' 
    || c == '1'
    || c == '2'
    || c == '3'
    || c == '4'
    || c == '5'
    || c == '6'
    || c == '7'
    || c == '8'
    || c == '9';
}

static bool lib_is_digit(const char* str) {
    if (!str) {
        return false;
    }
    
    if (*str == '\0') {
        // End string
        return false;
    }

    const char* ptr = str;
    while (*ptr != '\0') {
        if (!_isdigit(*ptr)) {
            return false;
        }
        ptr++;
    }
    return true;
}

static bool lib_is_normal_digit(const char* str) {
    if (!str) {
        return false;
    }

    if (*str == '\0') {
        // End string
        return false;
    }

    if (*str == '0') {
        // Start with '0'
        // '0'                - correct
        // '01', '001', '000' - incorrect
        return *(str + 1) == '\0';
    }

    return lib_is_digit(str);
}

inline static const char* lib_ifs(bool flag, const char* s1, const char* s2) {
    return flag ? s1 : s2;
}

inline static char lib_ifc(bool flag, char c1, char c2) {
    return flag ? c1 : c2;
}

inline static int lib_ifi(bool flag, int i1, int i2) {
    return flag ? i1 : i2;
}

static long lib_ifl(bool flag, long l1, long l2) {
    return flag ? l1 : l2;
}

inline static const char* lib_defs(const char* s1, const char* s2) {
    return s1 ? s1 : s2;
}

////

int lib_stricmp(const char* str1, const char* str2) {
  if (!str1 && !str2) {
    return 0;
  }
  if (!str1) {
    return -1;
  }
  if (!str2) {
    return 1;
  }
  if (str1 == str2) {
    return 0;
  }

  /*
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);
  if (len1 < len2) {
    return -1;
  }
  if (len2 < len1) {
    return 1;
  }
  */

  /*
  for (size_t i = 0; i < len1; i++) {
    c1 = tolower(str1[i]);
    c2 = tolower(str2[i]);
    if (c1 < c2) {
      return -1;
    }
    if (c2 < c1) {
      return 1;
    }
  }
  */

  char* s1 = (char*) str1;
  char* s2 = (char*) str2;
  int c1   = 0;
  int c2   = 0;

  while (true) {
    if (*s1 == '\0') {
      return (*s2 == '\0') ? 0 : -1;
    }
    if (*s2 == '\0') {
      return (*s1 == '\0') ? 0 : 1;
    }
    c1 = tolower(*s1);
    c2 = tolower(*s2);
    if (c1 < c2) {
      return -1;
    }
    if (c2 < c1) {
      return 1;
    }
    s1++;
    s2++;
  }
  return 0;
}

////

static const char* lib_doc_unitdef(const char* value) {
    return lib_is_digit(value) ? LIB_DOC_UNIT : "";
}

static int lib_doc_font_init(lib_font_t* font, 
    const char* font_name, 
    const char* font_style,  
    const char* font_weight,
    const char* font_size
) {
    if (!font) {
        return 1;
    }
    font->name = font_name;
    font->style = font_style;
    font->weight = font_weight;
    font->size = font_size;
    return 0;
}

static int lib_doc_config_init(lib_doc_config_t* cnf) {
    if (!cnf) {
        return 1;
    }

    cnf->charset     = NULL;
    cnf->title       = NULL;
    cnf->margin      = NULL;
    cnf->margin_unit = NULL;

    cnf->font = NULL;
    return 0;
}

static int lib_doc_context_prepare(lib_doc_context_t* ctx) {
    if (!ctx) {
        return 1;
    }

    //ctx->margin_unit = lib_rtf_unitdef(ctx->margin);
    //ctx->font->unit   = lib_rtf_unitdef(ctx->font->size);

    ctx->use_charset        = ctx->charset;
    ctx->use_title          = ctx->title;
    ctx->use_head           = ctx->use_charset || ctx->use_title;

    ctx->use_margin         = ctx->margin;

    if (ctx->font) {
        ctx->use_font_name      = ctx->font->name;
        ctx->use_font_style     = ctx->font->style;
        ctx->use_font_weight    = ctx->font->weight;
        ctx->use_font_size      = ctx->font->size;
        ctx->use_font           = ctx->use_font_name || ctx->use_font_style || ctx->use_font_weight || ctx->use_font_size;
    } else {
        ctx->use_font_name      = false;
        ctx->use_font_style     = false;
        ctx->use_font_weight    = false;
        ctx->use_font_size      = false;
        ctx->use_font           = false;        
    }

    ctx->use_style          = ctx->use_margin || ctx->use_font;

    return 0;

}

#endif // PLAZMA_LIB_DOCLIB_H